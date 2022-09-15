// Touhou 11: Subterranean Animism v1.00a

#include "patch_util.h"
#include "limiter.h"
#include "d3d9_hook.h"
#include "config.h"
#include "touhou11.h"

using namespace Touhou11;

CReplayManager** CReplayManager::InstancePtr = (CReplayManager**)0x004A8EB8;

int __stdcall th11_window_update_hook(void* self) {
	Limiter::Tick();
	return CWindowManager__UpdateFast(self);
}

void th11_install_patches() {
	{
		// Skip the original frame limiter
		BYTE patch[] = { 0xEB, 0x43 };
		patch_bytes(0x00446454, patch, sizeof(patch));
	}
	{
		// Force fast input latency mode
		BYTE patch[] = { 0xEB };
		patch_bytes(0x00445877, patch, sizeof(patch)); // Skip over automatic
		patch_bytes(0x0044588B, patch, sizeof(patch)); // Skip over normal
	}
	{
		// Hook window update
		patch_call(0x004458A7, th11_window_update_hook);
	}
	if (Config::D3D9Ex) {
		// Redirect Direct3DCreate9 call
		// IAT is being hooked, but thcrap also hooks Direct3DCreate9 in the same place
		// This should force our Direct3DCreate9 hook to be loaded no matter what
		patch_call(0x0044570E, Direct3DCreate9_hook);
	}
	if (Config::ReplaySpeedControl) {
		// Skip the original replay speed control stuff
		BYTE patch[] = { 0xEB };
		patch_bytes(0x00436D5F, patch, sizeof(patch));
	}
}

FPSTarget th11_replay_callback() {
	if (*CReplayManager::InstancePtr && (*CReplayManager::InstancePtr)->mode == 1) {
		// TODO: Reverse engineer this struct instead of being lazy
		auto input = *(DWORD*)0x004C92A8;
		if (input & InputState::Focus)
			return FPSTarget::ReplaySlow;
		else if (input & (InputState::Skip | InputState::Shoot))
			return FPSTarget::ReplaySkip;
	}
	return FPSTarget::Game;
}