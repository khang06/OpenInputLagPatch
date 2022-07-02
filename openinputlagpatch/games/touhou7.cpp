// Touhou 7: Perfect Cherry Blossom v1.00b

#include <stdio.h>
#include "patch_util.h"
#include "limiter.h"
#include "touhou7.h"
#include "config.h"

using namespace Touhou7;

CEngine* CEngine::Instance = (CEngine*)0x00575950;
CWindowManager* CWindowManager::Instance = (CWindowManager*)0x00575C20;
CChainManager* CChainManager::Instance = (CChainManager*)0x00626218;
CGame* CGame::Instance = (CGame*)0x00626270;
CTextureManager** CTextureManager::InstancePtr = (CTextureManager**)0x004B9E44;

int __fastcall th7_update_calc_chain_hook() {
	auto engine = CEngine::Instance;
	Limiter::Tick();

    auto ret = CChainManager__UpdateCalcChain(CChainManager::Instance);
    if (ret != 0 && ret != -1 && engine->config.frameskip <= (char)CWindowManager::Instance->frames_skipped) {
        engine->d3d8_device->BeginScene();
        CTextureManager__Unk44F580(*CTextureManager::InstancePtr);
        engine->field_2BC = 255;
        CEngine__UpdateFog(engine);
        CChainManager__UpdateDrawChain(CChainManager::Instance);
        CTextureManager__Flush(*CTextureManager::InstancePtr);
        engine->d3d8_device->SetTexture(0, 0);
        engine->d3d8_device->EndScene();
    }

    return ret;
}

int __fastcall th7_window_update_hook() {
    if (!CWindowManager::Instance->activated) {
        Sleep(16);
        return 0;
    }
    return CWindowManager__Update(CWindowManager::Instance);
}

void th7_install_patches() {
    {
        // Skip the original frame limiter
        BYTE patch[] = { 0xE9, 0x70, 0x01, 0x00, 0x00, 0x90 };
        patch_bytes((void*)0x00434854, patch, sizeof(patch));
    }
    {
        // Skip original draw chain update logic
        BYTE patch[] = { 0xEB };
        patch_bytes((void*)0x00434718, patch, sizeof(patch));
    }
    {
        // Hook calc chain update
        // Shaves off 1 frame of input lag by calling calc chains BEFORE drawing
        patch_call((void*)0x004347DF, th7_update_calc_chain_hook);
    }
    {
        // Hook window updating
        // Avoid eating up CPU time while minimized
        patch_call((void*)0x004341F7, th7_window_update_hook);
    }
    if (Config::FixInputGlitching) {
        // Fix input glitching
        BYTE patch[] = { 0x00, 0x00, 0x00, 0x00, 0x74 };
        patch_bytes((void*)0x00430F03, patch, sizeof(patch));
    }
}

FPSTarget th7_replay_callback() {
    if (CEngine::Instance->state == 2 && ((CGame::Instance->flags >> 3) & 1) != 0) {
        auto input = get_input();
        if (input & InputState::Focus)
            return FPSTarget::ReplaySlow;
        else if (input & (InputState::Skip | InputState::Shoot))
            return FPSTarget::ReplaySkip;
    }
    return FPSTarget::Game;
}