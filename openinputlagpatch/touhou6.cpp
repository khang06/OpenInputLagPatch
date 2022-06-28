// Touhou 6: Embodiment of Scarlet Devil v1.02h

#include <stdio.h>
#include "patch_util.h"
#include "limiter.h"
#include "touhou6.h"

CEngine* CEngine::Instance = (CEngine*)0x006C6D18;
CWindowManager* CWindowManager::Instance = (CWindowManager*)0x006C6BD4;
CChainManager* CChainManager::Instance = (CChainManager*)0x0069D918;
CGame* CGame::Instance = (CGame*)0x0069BCA0;

// HACK: This should actually be taking in a parameter and using that in place of CChainManager::Instance
// However, if I did that, the edi register gets corrupted by CChainManager::UpdateCalcChain
// ...and that causes a crash because "this" is saved in that register in the generated assembly for this function
// Doing it this way seems to work fine. More investigation required...
int __fastcall update_calc_chain_hook() {
	auto engine = CEngine::Instance;
    Limiter::Tick();

    engine->viewport.X = 0;
    engine->viewport.Y = 0;
    engine->viewport.Width = 640;
    engine->viewport.Height = 480;
    engine->d3d8_device->SetViewport(&engine->viewport);
    auto ret = CChainManager__UpdateCalcChain(CChainManager::Instance);

    if (ret != 0 && ret != -1 && engine->config.frameskip <= (int)CWindowManager::Instance->frames_skipped) {
        if ((engine->config.engine_flags >> 4) & 1 | (engine->config.engine_flags >> 3) & 1) {
            D3DVIEWPORT8 viewport;
            viewport.X = 0;
            viewport.Y = 0;
            viewport.Width = 640;
            viewport.Height = 480;
            viewport.MinZ = 0.0;
            viewport.MaxZ = 1.0;
            engine->d3d8_device->SetViewport(&viewport);
            engine->d3d8_device->Clear(0, 0, 3, *(D3DCOLOR*)0x004B7B60, 1.0, 0);
            engine->d3d8_device->SetViewport(&engine->viewport);
        }
        engine->d3d8_device->BeginScene();
        CChainManager__UpdateDrawChain(CChainManager::Instance);
        engine->d3d8_device->EndScene();
        engine->d3d8_device->SetTexture(0, 0);
    }

    return ret;
}

int __fastcall window_update_hook() {
    if (!CWindowManager::Instance->activated) {
        Sleep(16);
        return 0;
    }
    return CWindowManager__Update(CWindowManager::Instance);
}

void th6_install_patches() {
	{
		// Skip the original frame limiter
		BYTE patch[] = { 0xE9, 0x9E, 0x00, 0x00, 0x00 };
		patch_bytes((void*)0x004208ED, patch, sizeof(patch));
	}
	{
		// Skip original draw chain update logic
		BYTE patch[] = { 0xE9, 0xEE, 0x00 };
		patch_bytes((void*)0x0042071A, patch, sizeof(patch));
	}
	{
		// Hook calc chain update
		// Shaves off 1 frame of input lag by calling calc chains BEFORE drawing
		patch_call((void*)0x00420853, update_calc_chain_hook);
	}
    {
        // Hook window updating
        // Avoid eating up CPU time while minimized
        patch_call((void*)0x004204FF, window_update_hook);
    }
}

FPSTarget th6_replay_callback() {
    if (CEngine::Instance->state == 2 && CGame::Instance->is_replay) {
        auto input = get_input();
        if (input & InputState::Focus)
            return FPSTarget::ReplaySlow;
        else if (input & InputState::Skip)
            return FPSTarget::ReplaySkip;
    }
    return FPSTarget::Game;
}