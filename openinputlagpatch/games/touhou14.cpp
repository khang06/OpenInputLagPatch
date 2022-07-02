// Touhou 14: Double Dealing Character v1.00b

#include "patch_util.h"
#include "limiter.h"
#include "d3d9_hook.h"
#include "config.h"
#include "touhou14.h"

using namespace Touhou14;

CReplayManager** CReplayManager::InstancePtr = (CReplayManager**)0x004DB688;

int __fastcall th14_window_update_hook(void* self) {
	Limiter::Tick();
	return CWindowManager__UpdateFast(self);
}

void th14_install_patches() {
	{
		// Skip the original frame limiter
		BYTE patch[] = { 0xEB, 0x4A };
		patch_bytes((void*)0x0046A76E, patch, sizeof(patch));
	}
	{
		// Force fast input latency mode
		BYTE patch[] = { 0xEB };
		patch_bytes((void*)0x00469A20, patch, sizeof(patch)); // Skip over automatic
		patch_bytes((void*)0x00469A35, patch, sizeof(patch)); // Skip over normal
	}
	{
		// Hook window update
		patch_call((void*)0x00469A51, th14_window_update_hook);
	}
	if (Config::D3D9Ex) {
		// Redirect Direct3DCreate9 call
		// IAT is being hooked, but thcrap also hooks Direct3DCreate9 in the same place
		// This should force our Direct3DCreate9 hook to be loaded no matter what
		patch_call((void*)0x0046952C, Direct3DCreate9_hook);

		// Extra NOP is needed because we're replacing a FF 15 call, which is 6 bytes long
		BYTE patch[] = { 0x90 };
		patch_bytes((void*)0x00469531, patch, sizeof(patch));
	}
	if (Config::ReplaySpeedControl) {
		// Skip the original replay speed control stuff
		BYTE patch[] = { 0xEB, 0x1D };
		patch_bytes((void*)0x00455E82, patch, sizeof(patch));
	}
}

FPSTarget th14_replay_callback() {
	if (*CReplayManager::InstancePtr && (*CReplayManager::InstancePtr)->mode == 1) {
		// TODO: Reverse engineer this struct instead of being lazy
		auto input = *(DWORD*)0x004D6878;
		if (input & InputState::Focus)
			return FPSTarget::ReplaySlow;
		else if (input & (InputState::Skip | InputState::Shoot))
			return FPSTarget::ReplaySkip;
	}
	return FPSTarget::Game;
}