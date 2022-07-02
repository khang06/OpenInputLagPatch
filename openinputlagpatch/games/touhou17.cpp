// Touhou 17: Hidden Star in Four Seasons v1.00b

#include "patch_util.h"
#include "limiter.h"
#include "d3d9_hook.h"
#include "config.h"
#include "touhou17.h"

using namespace Touhou17;

CReplayManager** CReplayManager::InstancePtr = (CReplayManager**)0x004B77D8;

int __fastcall th17_window_update_hook(void* self) {
	Limiter::Tick();
	return CWindowManager__UpdateFast(self);
}

void th17_install_patches() {
	{
		// Skip the original frame limiter
		BYTE patch[] = { 0xEB, 0x53 };
		patch_bytes((void*)0x004621E5, patch, sizeof(patch));
	}
	{
		// Force fast input latency mode
		BYTE patch[] = { 0xE9, 0x2F, 0x01, 0x00, 0x00, 0x90 };
		patch_bytes((void*)0x0046119E, patch, sizeof(patch));
	}
	{
		// Hook window update
		patch_call((void*)0x004612D7, th17_window_update_hook);
	}
	if (Config::D3D9Ex) {
		// Redirect Direct3DCreate9 call
		// IAT is being hooked, but thcrap also hooks Direct3DCreate9 in the same place
		// This should force our Direct3DCreate9 hook to be loaded no matter what
		patch_call((void*)0x00460F1C, Direct3DCreate9_hook);

		// Extra NOP is needed because we're replacing a FF 15 call, which is 6 bytes long
		BYTE patch[] = { 0x90 };
		patch_bytes((void*)0x00460F21, patch, sizeof(patch));
	}
	if (Config::ReplaySpeedControl) {
		// Skip the original replay speed control stuff
		BYTE patch[] = { 0x90, 0x90 };
		patch_bytes((void*)0x0044E633, patch, sizeof(patch));
	}
}

FPSTarget th17_replay_callback() {
	if (*CReplayManager::InstancePtr && (*CReplayManager::InstancePtr)->mode == 1) {
		// TODO: Reverse engineer this struct instead of being lazy
		auto input = *(DWORD*)0x004B3230;
		if (input & InputState::Focus)
			return FPSTarget::ReplaySlow;
		else if (input & (InputState::Skip | InputState::Shoot))
			return FPSTarget::ReplaySkip;
	}
	return FPSTarget::Game;
}