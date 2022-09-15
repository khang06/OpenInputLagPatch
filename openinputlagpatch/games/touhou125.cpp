// Touhou 12.5: Double Spoiler v1.00a

#include "patch_util.h"
#include "limiter.h"
#include "d3d9_hook.h"
#include "config.h"
#include "touhou125.h"

using namespace Touhou125;

CReplayManager** CReplayManager::InstancePtr = (CReplayManager**)0x004B68CC;

int __stdcall th125_window_update_hook(void* self) {
	Limiter::Tick();
	return CWindowManager__UpdateFast(self);
}

void th125_install_patches() {
	{
		// Skip the original frame limiter
		BYTE patch[] = { 0xEB, 0x5B };
		patch_bytes(0x0044F113, patch, sizeof(patch));
	}
	{
		// Force fast input latency mode
		BYTE patch[] = { 0xEB };
		patch_bytes(0x0044E2F7, patch, sizeof(patch)); // Skip over automatic
		patch_bytes(0x0044E30B, patch, sizeof(patch)); // Skip over normal
	}
	{
		// Hook window update
		patch_call(0x0044E327, th125_window_update_hook);
	}
	if (Config::D3D9Ex) {
		// Redirect Direct3DCreate9 call
		// IAT is being hooked, but thcrap also hooks Direct3DCreate9 in the same place
		// This should force our Direct3DCreate9 hook to be loaded no matter what
		patch_call(0x0044E179, Direct3DCreate9_hook);
	}
	if (Config::ReplaySpeedControl) {
		// Skip the original replay speed control stuff
		BYTE patch[] = { 0xEB };
		patch_bytes(0x0043C86C, patch, sizeof(patch));
	}
}

FPSTarget th125_replay_callback() {
	if (*CReplayManager::InstancePtr && (*CReplayManager::InstancePtr)->mode == 1) {
		// TODO: Reverse engineer this struct instead of being lazy
		auto input = *(DWORD*)0x004D8DA0;
		if (input & InputState::Focus)
			return FPSTarget::ReplaySlow;
		else if (input & (InputState::Skip | InputState::Shoot))
			return FPSTarget::ReplaySkip;
	}
	return FPSTarget::Game;
}