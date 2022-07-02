// Touhou 16.5: Violet Detector v1.00a

#include "patch_util.h"
#include "limiter.h"
#include "d3d9_hook.h"
#include "config.h"
#include "touhou165.h"

using namespace Touhou165;

CReplayManager** CReplayManager::InstancePtr = (CReplayManager**)0x004B565C;

int __fastcall th165_window_update_hook(void* self) {
	Limiter::Tick();
	return CWindowManager__UpdateFast(self);
}

void th165_install_patches() {
	{
		// Skip the original frame limiter
		BYTE patch[] = { 0xEB, 0x51 };
		patch_bytes((void*)0x00460FA7, patch, sizeof(patch));
	}
	{
		// Force fast input latency mode
		BYTE patch[] = { 0xE9, 0xE9, 0x01, 0x00, 0x00, 0x90 };
		patch_bytes((void*)0x004602BD, patch, sizeof(patch));
	}
	{
		// Hook window update
		patch_call((void*)0x004604B0, th165_window_update_hook);
	}
	if (Config::D3D9Ex) {
		// Redirect Direct3DCreate9 call
		// IAT is being hooked, but thcrap also hooks Direct3DCreate9 in the same place
		// This should force our Direct3DCreate9 hook to be loaded no matter what
		patch_call((void*)0x00460037, Direct3DCreate9_hook);

		// Extra NOP is needed because we're replacing a FF 15 call, which is 6 bytes long
		BYTE patch[] = { 0x90 };
		patch_bytes((void*)0x0046003C, patch, sizeof(patch));
	}
	if (Config::ReplaySpeedControl) {
		// Skip the original replay speed control stuff
		BYTE patch[] = { 0xEB };
		patch_bytes((void*)0x0045053C, patch, sizeof(patch));
	}
}

FPSTarget th165_replay_callback() {
	if (*CReplayManager::InstancePtr && (*CReplayManager::InstancePtr)->mode == 1) {
		// TODO: Reverse engineer this struct instead of being lazy
		auto input = *(DWORD*)0x004B0FF0;
		if (input & InputState::Focus)
			return FPSTarget::ReplaySlow;
		else if (input & (InputState::Skip | InputState::Shoot))
			return FPSTarget::ReplaySkip;
	}
	return FPSTarget::Game;
}