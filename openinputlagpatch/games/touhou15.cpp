// Touhou 15: Legacy of Lunatic Kingdom v1.00b

#include "patch_util.h"
#include "limiter.h"
#include "d3d9_hook.h"
#include "config.h"
#include "touhou15.h"

using namespace Touhou15;

CReplayManager** CReplayManager::InstancePtr = (CReplayManager**)0x004E9BC4;

int __fastcall th15_window_update_hook(void* self) {
	Limiter::Tick();
	return CWindowManager__UpdateFast(self);
}

void th15_install_patches() {
	{
		// Skip the original frame limiter
		BYTE patch[] = { 0xEB, 0x4A };
		patch_bytes((void*)0x004727DE, patch, sizeof(patch));
	}
	{
		// Force fast input latency mode
		BYTE patch[] = { 0xEB };
		patch_bytes((void*)0x00471A86, patch, sizeof(patch)); // Skip over automatic
		patch_bytes((void*)0x00471A9B, patch, sizeof(patch)); // Skip over normal
	}
	{
		// Hook window update
		patch_call((void*)0x00471AB7, th15_window_update_hook);
	}
	if (Config::D3D9Ex) {
		// Redirect Direct3DCreate9 call
		// IAT is being hooked, but thcrap also hooks Direct3DCreate9 in the same place
		// This should force our Direct3DCreate9 hook to be loaded no matter what
		patch_call((void*)0x0047158C, Direct3DCreate9_hook);

		// Extra NOP is needed because we're replacing a FF 15 call, which is 6 bytes long
		BYTE patch[] = { 0x90 };
		patch_bytes((void*)0x00471591, patch, sizeof(patch));
	}
	if (Config::ReplaySpeedControl) {
		// Skip the original replay speed control stuff
		BYTE patch[] = { 0xEB, 0x1D };
		patch_bytes((void*)0x0045CED2, patch, sizeof(patch));
	}
}

FPSTarget th15_replay_callback() {
	if (*CReplayManager::InstancePtr && (*CReplayManager::InstancePtr)->mode == 1) {
		// TODO: Reverse engineer this struct instead of being lazy
		auto input = *(DWORD*)0x004E6D10;
		if (input & InputState::Focus)
			return FPSTarget::ReplaySlow;
		else if (input & (InputState::Skip | InputState::Shoot))
			return FPSTarget::ReplaySkip;
	}
	return FPSTarget::Game;
}