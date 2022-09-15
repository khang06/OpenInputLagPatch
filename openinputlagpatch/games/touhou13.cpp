// Touhou 13: Ten Desires v1.00c

#include "patch_util.h"
#include "limiter.h"
#include "d3d9_hook.h"
#include "config.h"
#include "touhou13.h"

using namespace Touhou13;

CReplayManager** CReplayManager::InstancePtr = (CReplayManager**)0x004C22C8;

int __stdcall th13_window_update_hook(void* self) {
	Limiter::Tick();
	return CWindowManager__UpdateFast(self);
}

void th13_install_patches() {
	{
		// Skip the original frame limiter
		BYTE patch[] = { 0xEB, 0x5B };
		patch_bytes(0x0045D334, patch, sizeof(patch));
	}
	{
		// Force fast input latency mode
		BYTE patch[] = { 0xEB };
		patch_bytes(0x0045C5D7, patch, sizeof(patch)); // Skip over automatic
		patch_bytes(0x0045C5EB, patch, sizeof(patch)); // Skip over normal
	}
	{
		// Hook window update
		patch_call(0x0045C607, th13_window_update_hook);
	}
	if (Config::D3D9Ex) {
		// Redirect Direct3DCreate9 call
		// IAT is being hooked, but thcrap also hooks Direct3DCreate9 in the same place
		// This should force our Direct3DCreate9 hook to be loaded no matter what
		patch_call(0x0045C42F, Direct3DCreate9_hook);

		// Extra NOP is needed because we're replacing a FF 15 call, which is 6 bytes long
		BYTE patch[] = { 0x90 };
		patch_bytes(0x0045C434, patch, sizeof(patch));
	}
	if (Config::ReplaySpeedControl) {
		// Skip the original replay speed control stuff
		BYTE patch[] = { 0xEB, 0x1D };
		patch_bytes(0x00448E6F, patch, sizeof(patch));
	}
}

FPSTarget th13_replay_callback() {
	if (*CReplayManager::InstancePtr && (*CReplayManager::InstancePtr)->mode == 1) {
		// TODO: Reverse engineer this struct instead of being lazy
		auto input = *(DWORD*)0x004E49F0;
		if (input & InputState::Focus)
			return FPSTarget::ReplaySlow;
		else if (input & (InputState::Skip | InputState::Shoot))
			return FPSTarget::ReplaySkip;
	}
	return FPSTarget::Game;
}