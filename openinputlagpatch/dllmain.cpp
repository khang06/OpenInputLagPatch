#include <Windows.h>
#include <stdio.h>
#include <winternl.h>
#include "patch_util.h"
#include "games.h"
#include "limiter.h"
#include "d3d9_hook.h"

// Allocates a console for debugging purposes
void create_console() {
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
}

// Register a DLL load callback to abort if vpatch gets loaded
typedef struct _LDR_DLL_LOADED_NOTIFICATION_DATA
{
    ULONG Flags;                   // Reserved.
    PCUNICODE_STRING FullDllName;  // The full path name of the DLL module.
    PCUNICODE_STRING BaseDllName;  // The base file name of the DLL module.
    PVOID DllBase;                 // A pointer to the base address for the DLL in memory.
    ULONG SizeOfImage;             // The size of the DLL image, in bytes.
} LDR_DLL_LOADED_NOTIFICATION_DATA, *PLDR_DLL_LOADED_NOTIFICATION_DATA;
#define LDR_DLL_NOTIFICATION_REASON_LOADED 1

// MessageBox is broken in dll_load_callback, so vpatch's entrypoint is redirected to this instead
void vpatch_abort() {
    MessageBox(
        NULL,
        L"vpatch and OpenInputLagPatch are incompatible.\nUninstall OpenInputLagPatch by deleting dinput8.dll or stop running the game via vpatch.exe.",
        L"OpenInputLagPatch",
        MB_ICONERROR
    );
    exit(1);
}

VOID CALLBACK dll_load_callback(ULONG NotificationReason, PLDR_DLL_LOADED_NOTIFICATION_DATA NotificationData, PVOID Context) {
    // Only notify on DLL load
    if (NotificationReason != LDR_DLL_NOTIFICATION_REASON_LOADED)
        return;

    // Check if the DLL's filename contains vpatch
    if (wcsstr(NotificationData->BaseDllName->Buffer, L"vpatch") != nullptr) {
        printf("vpatch detected: %S\n", NotificationData->FullDllName->Buffer);

        // Patch vpatch's entrypoint to abort instantly
        auto pe = (char*)NotificationData->DllBase;
        auto nt_header = (PIMAGE_NT_HEADERS32)(pe + ((PIMAGE_DOS_HEADER)pe)->e_lfanew);
        auto optional_header = (PIMAGE_OPTIONAL_HEADER32)(&nt_header->OptionalHeader);
        patch_call(pe + optional_header->AddressOfEntryPoint, vpatch_abort);
    }
}

// Typedef edited to remove LDR_DLL_UNLOADED_NOTIFICATION_DATA
typedef VOID NTAPI LDR_DLL_NOTIFICATION_FUNCTION(_In_ ULONG NotificationReason,
    _In_ PLDR_DLL_LOADED_NOTIFICATION_DATA NotificationData,
    _In_opt_ PVOID Context);
typedef LDR_DLL_NOTIFICATION_FUNCTION* PLDR_DLL_NOTIFICATION_FUNCTION;
typedef NTSTATUS(NTAPI* LdrRegisterDllNotification_t)(
    _In_ ULONG Flags, _In_ PLDR_DLL_NOTIFICATION_FUNCTION NotificationFunction,
    _In_opt_ PVOID Context, _Out_ PVOID* Cookie);
void register_dll_load_callback() {
    // Get LdrRegisterDllNotification from ntdll.dll
    auto ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll)
        return;
    auto LdrRegisterDllNotification = (LdrRegisterDllNotification_t)GetProcAddress(ntdll, "LdrRegisterDllNotification");
    if (!LdrRegisterDllNotification)
        return;

    // Register the DLL load callback
    void* cookie = nullptr;
    LdrRegisterDllNotification(0, dll_load_callback, NULL, &cookie);
}

// IAT hook timeBeginPeriod and timeEndPeriod to stub them
// The game calls these functions a bunch of times to set the system timer to 1ms
// It's not very effective because it only sets the timer for incredibly small periods of time
// This ensures that the system timer is set to 1ms for the entirety of the process' lifetime
MMRESULT WINAPI time_stub() {
    return MMSYSERR_NOERROR;
}

void hook_winmm_time_period() {
    // Set the system timer to 1ms
    timeBeginPeriod(1);

    // Replace timeBeginPeriod and timeEndPeriod with a stub
    iat_hook(NULL, "winmm.dll", "timeBeginPeriod", time_stub);
    iat_hook(NULL, "winmm.dll", "timeEndPeriod", time_stub);
}

// Actually patches everything
void install_patches() {
    auto game = detect_game();
    if (game == TouhouGame::Unknown)
        exit(1);
    else if (game == TouhouGame::Custom)
        return;

    limiter_init();
    hook_winmm_time_period();
    hook_d3d9();

    switch (game) {
        case TouhouGame::Th6:
            th6_install_patches();
            break;
        case TouhouGame::Th10:
            th10_install_patches();
            break;
        default:
            MessageBoxW(
                NULL,
                L"The game was detected properly, but it didn't have a patch handler. This shouldn't happen under any circumstances.\nPlease report this issue on the project Github page.",
                L"OpenInputLagPatch",
                MB_ICONERROR
            );
            exit(1);
    }
}

// Main entrypoint
void patcher_main() {
    //MessageBoxW(NULL, L"Waiting...", L"", 0);

    create_console();
    register_dll_load_callback();
    install_patches();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        patcher_main();
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

// Fake export to allow the dll to be loaded as dinput8.dll
extern "C" __declspec(dllexport) HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion,
    REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
    #pragma comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)
    wchar_t real_dinput8_path[MAX_PATH] = {};
    GetSystemDirectoryW(real_dinput8_path, sizeof(real_dinput8_path) / sizeof(wchar_t));
    real_dinput8_path[MAX_PATH - 1] = 0;
    wcscat_s(real_dinput8_path, L"\\dinput8.dll");

    auto real_dinput8 = LoadLibraryW(real_dinput8_path);
    if (real_dinput8) {
        auto real_create = (HRESULT(WINAPI*)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN))GetProcAddress(real_dinput8, "DirectInput8Create");
        if (real_create)
            return real_create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
    }

    return E_FAIL;
}