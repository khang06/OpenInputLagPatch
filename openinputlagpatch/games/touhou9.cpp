// Touhou 9: Phantasmagoria of Flower View v1.50a

#include <stdio.h>
#include "patch_util.h"
#include "limiter.h"
#include "touhou9.h"
#include "config.h"

using namespace Touhou9;

CEngine* CEngine::Instance = (CEngine*)0x004B3100;
CGame* CGame::Instance = (CGame*)0x004A7D90;

int __fastcall th9_window_update_hook(void* self) {
    Limiter::Tick();
    return CWindowManager__Update(self);
}

void th9_install_patches() {
    {
        // Skip the original frame limiter
        BYTE patch[] = { 0xEB };
        patch_bytes((void*)0x0042DC72, patch, sizeof(patch));
    }
    {
        // Don't sleep in the main loop
        BYTE patch[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
        patch_bytes((void*)0x0042DC64, patch, sizeof(patch));
    }
    {
        // Hook window update
        patch_call((void*)0x0042E6A6, th9_window_update_hook);
    }
}

FPSTarget th9_replay_callback() {
    if (CGame::Instance->replay && CGame::Instance->replay->is_replay) {
        auto input = get_input(2);
        if (input & InputState::Focus)
            return FPSTarget::ReplaySlow;
        else if (input & (InputState::Skip | InputState::Shoot))
            return FPSTarget::ReplaySkip;
    }
    return FPSTarget::Game;
}