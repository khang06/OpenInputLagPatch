#include <Windows.h>
#include <stdio.h>
#include "patch_util.h"
#include "games.h"
#include "limiter.h"
#include "d3d9_hook.h"

// Allocates a console for debugging purposes
void create_console() {
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
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
        case TouhouGame::Th06:
            th6_install_patches();
            break;
    }
}

// Main entrypoint
void patcher_main() {
    //MessageBoxW(NULL, L"Waiting...", L"", 0);

    create_console();
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