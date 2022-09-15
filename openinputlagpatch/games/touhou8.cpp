// Touhou 8: Imperishable Night v1.00d

#include <stdio.h>
#include "patch_util.h"
#include "limiter.h"
#include "touhou8.h"
#include "config.h"

using namespace Touhou8;

CEngine* CEngine::Instance = (CEngine*)0x017CE758;
CGame* CGame::Instance = (CGame*)0x0160F508;

int __fastcall th8_window_update_hook(void* self) {
    Limiter::Tick();
    return CWindowManager__Update(self);
}

void th8_install_patches() {
    {
        // Skip the original frame limiter
        BYTE patch[] = { 0xEB };
        patch_bytes(0x00441EE6, patch, sizeof(patch));
    }
    {
        // Don't sleep in the main loop
        BYTE patch[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
        patch_bytes(0x00441ECF, patch, sizeof(patch));
    }
    {
        // Hook window update
        patch_call(0x00441B15, th8_window_update_hook);
    }
}

FPSTarget th8_replay_callback() {
    if (CEngine::Instance->state == 2 && ((CGame::Instance->flags >> 3) & 1) != 0) {
        auto input = get_input();
        if (input & InputState::Focus)
            return FPSTarget::ReplaySlow;
        else if (input & (InputState::Skip | InputState::Shoot))
            return FPSTarget::ReplaySkip;
    }
    return FPSTarget::Game;
}