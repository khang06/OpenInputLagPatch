#include <Windows.h>
#include <stdio.h>
#include <winternl.h>
#include <shlwapi.h>
#include "patch_util.h"
#include "games.h"
#include "limiter.h"
#include "d3d9_hook.h"
#include "config.h"
#include "sha256.h"
#include "common.h"

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
    panic_msgbox(L"vpatch and OpenInputLagPatch are incompatible.\nStop running the game via vpatch.exe or uninstall OpenInputLagPatch by deleting dinput8.dll.");
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

// Check if vpatch is loaded and register a DLL load callback to detect it being loaded later
void check_vpatch() {
    // On older Touhou games, vpatch remotely creates a thread that calls LoadLibraryA on itself
    // Since the DLL isn't actually loaded by the time this executes, a callback is registered to detect it being loaded later
    register_dll_load_callback();

    // On modern Touhou games, vpatch itself loads dinput8.dll
    // Get the executable's path
    wchar_t exe_path[1024] = {};
    if (!GetModuleFileName(NULL, exe_path, 1024))
        return;

    // Strip away the path to get the filename itself
    wchar_t* filename = PathFindFileNameW(exe_path);

    // Check if it contains vpatch
    if (wcsstr(filename, L"vpatch") != nullptr) {
        printf("vpatch detected: %S\n", exe_path);
        vpatch_abort();
    }
}

// Check if the ENBSeries DX8 to DX9 Convertor [sic] is installed and complain to the user if it is
// TODO: This should also check if the vanilla d3d8.dll is being used, but we can't just hash that
// Maybe we should try checking the publisher information?
void check_enb_dx8() {
    // Get the dll's path
    wchar_t config_path[1024] = {};
    if (!GetModuleFileNameW(NULL, config_path, MAX_PATH))
        return;
    PathRemoveFileSpecW(config_path);
    PathAppendW(config_path, L"\\d3d8.dll");

    // Hash it
    char hash_str[SHA256_BLOCK_SIZE * 2 + 1] = {};
    if (!sha256_file(config_path, hash_str))
        return;

    // Check if the DLL is the wrong one
    // This hash is for v0.036
    if (!strcmp(hash_str, "1f0471c8fa53b035aa27d6d6505275e2ee0db55b6538b4e31fd79e54ce065759")) {
        MessageBox(
            NULL,
            L"ENBSeries DX8 to DX9 Convertor detected.\nThe game will run fine, but D3D9Ex features will be unavailable and you'll have worse input lag.\nPlease install d3d8to9 for best results.",
            L"OpenInputLagPatch",
            MB_ICONWARNING
        );
    }
}

// IAT hook timeBeginPeriod and timeEndPeriod to stub them
// The game calls these functions a bunch of times to set the system timer to 1ms
// It's not very effective because it only sets the timer for incredibly small periods of time
// This ensures that the system timer is set to 1ms for the entirety of the process' lifetime
MMRESULT WINAPI time_stub(UINT uPeriod) {
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

    Limiter::Initialize(get_replay_callback(game));
    hook_winmm_time_period();
    if (Config::D3D9Ex)
        hook_d3d9();

    switch (game) {
        case TouhouGame::Th6:
            th6_install_patches();
            break;
        case TouhouGame::Th10:
            th10_install_patches();
            break;
        case TouhouGame::Th7:
            th7_install_patches();
            break;
        case TouhouGame::Th8:
            th8_install_patches();
            break;
        default:
            panic_msgbox(L"The game was detected properly, but it didn't have a patch handler.");
    }
}

// Main entrypoint
void patcher_main() {
    Config::Load();
    if (Config::DebugWait)
        MessageBoxW(NULL, L"Waiting...\nIf you don't want to see this, set DebugWait to 0 in the config.", L"OpenInputLagPatch", 0);
    if (Config::DebugConsole)
        create_console();
    check_vpatch();
    if (Config::D3D9Ex)
        check_enb_dx8();
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
