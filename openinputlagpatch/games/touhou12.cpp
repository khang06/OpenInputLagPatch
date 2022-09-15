// Touhou 12: Undefined Fantastic Object v1.00b

#include "patch_util.h"
#include "limiter.h"
#include "d3d9_hook.h"
#include "config.h"
#include "touhou12.h"

using namespace Touhou12;

CReplayManager** CReplayManager::InstancePtr = (CReplayManager**)0x004C22C8;

int __stdcall th12_window_update_hook(void* self) {
	Limiter::Tick();
	return CWindowManager__UpdateFast(self);
}

void th12_install_patches() {
	{
		// Skip the original frame limiter
		BYTE patch[] = { 0xEB, 0x43 };
		patch_bytes(0x00450424, patch, sizeof(patch));
	}
	{
		// Force fast input latency mode
		BYTE patch[] = { 0xEB };
		patch_bytes(0x0044F87A, patch, sizeof(patch)); // Skip over automatic
		patch_bytes(0x0044F88E, patch, sizeof(patch)); // Skip over normal
	}
	{
		// Hook window update
		patch_call(0x0044F8AA, th12_window_update_hook);
	}
	if (Config::D3D9Ex) {
		// Redirect Direct3DCreate9 call
		// IAT is being hooked, but thcrap also hooks Direct3DCreate9 in the same place
		// This should force our Direct3DCreate9 hook to be loaded no matter what
		patch_call(0x0044F6FC, Direct3DCreate9_hook);
	}
	if (Config::ReplaySpeedControl) {
		// Skip the original replay speed control stuff
		BYTE patch[] = { 0xEB };
		patch_bytes(0x0043C54F, patch, sizeof(patch));
	}
}

FPSTarget th12_replay_callback() {
	if (*CReplayManager::InstancePtr && (*CReplayManager::InstancePtr)->mode == 1) {
		// TODO: Reverse engineer this struct instead of being lazy
		auto input = *(DWORD*)0x004D48B8;
		if (input & InputState::Focus)
			return FPSTarget::ReplaySlow;
		else if (input & (InputState::Skip | InputState::Shoot))
			return FPSTarget::ReplaySkip;
	}
	return FPSTarget::Game;
}