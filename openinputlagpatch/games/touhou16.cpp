// Touhou 16: Hidden Star in Four Seasons v1.00a

#include "patch_util.h"
#include "limiter.h"
#include "d3d9_hook.h"
#include "config.h"
#include "touhou16.h"

using namespace Touhou16;

CReplayManager** CReplayManager::InstancePtr = (CReplayManager**)0x004A6F08;

int __fastcall th16_window_update_hook(void* self) {
	Limiter::Tick();
	return CWindowManager__UpdateFast(self);
}

void th16_install_patches() {
	{
		// Skip the original frame limiter
		BYTE patch[] = { 0xEB, 0x4B };
		patch_bytes(0x0045AC9D, patch, sizeof(patch));
	}
	{
		// Force fast input latency mode
		BYTE patch[] = { 0xEB };
		patch_bytes(0x00459F72, patch, sizeof(patch)); // Skip over automatic
		patch_bytes(0x00459F87, patch, sizeof(patch)); // Skip over normal
	}
	{
		// Hook window update
		patch_call(0x00459FA3, th16_window_update_hook);
	}
	if (Config::D3D9Ex) {
		// Redirect Direct3DCreate9 call
		// IAT is being hooked, but thcrap also hooks Direct3DCreate9 in the same place
		// This should force our Direct3DCreate9 hook to be loaded no matter what
		patch_call(0x00459A84, Direct3DCreate9_hook);

		// Extra NOP is needed because we're replacing a FF 15 call, which is 6 bytes long
		BYTE patch[] = { 0x90 };
		patch_bytes(0x00459A89, patch, sizeof(patch));
	}
	if (Config::ReplaySpeedControl) {
		// Skip the original replay speed control stuff
		BYTE patch[] = { 0xEB };
		patch_bytes(0x00448E62, patch, sizeof(patch));
	}
}

FPSTarget th16_replay_callback() {
	if (*CReplayManager::InstancePtr && (*CReplayManager::InstancePtr)->mode == 1) {
		// TODO: Reverse engineer this struct instead of being lazy
		auto input = *(DWORD*)0x004A50B0;
		if (input & InputState::Focus)
			return FPSTarget::ReplaySlow;
		else if (input & (InputState::Skip | InputState::Shoot))
			return FPSTarget::ReplaySkip;
	}
	return FPSTarget::Game;
}