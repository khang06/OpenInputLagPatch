// Touhou 6: Embodiment of Scarlet Devil v1.02h

#include <stdio.h>
#include "patch_util.h"
#include "limiter.h"
#include "touhou6.h"

CEngine* CEngine::Instance = (CEngine*)0x006C6D18;
CWindowManager* CWindowManager::Instance = (CWindowManager*)0x006C6BD4;
CChainManager* CChainManager::Instance = (CChainManager*)0x0069D918;

int __fastcall update_calc_chain_hook() {
	auto engine = CEngine::Instance;
    limiter_tick();

    engine->viewport.X = 0;
    engine->viewport.Y = 0;
    engine->viewport.Width = 640;
    engine->viewport.Height = 480;
    engine->d3d8_device->SetViewport(&engine->viewport);
    auto ret = CChainManager__UpdateCalcChain(CChainManager::Instance);

    if (engine->config.frameskip <= (int)CWindowManager::Instance->frames_skipped) {
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
}