// Touhou 12.8: Great Fairy Wars v1.00a

#include "patch_util.h"
#include "limiter.h"
#include "d3d9_hook.h"
#include "config.h"
#include "touhou128.h"

using namespace Touhou128;

CReplayManager** CReplayManager::InstancePtr = (CReplayManager**)0x004B8A84;

int __stdcall th128_window_update_hook(void* self) {
	Limiter::Tick();
	return CWindowManager__UpdateFast(self);
}

void th128_install_patches() {
	{
		// Skip the original frame limiter
		BYTE patch[] = { 0xEB, 0x5B };
		patch_bytes(0x00454C14, patch, sizeof(patch));
	}
	{
		// Force fast input latency mode
		BYTE patch[] = { 0xEB };
		patch_bytes(0x00453D42, patch, sizeof(patch)); // Skip over automatic
		patch_bytes(0x00453D56, patch, sizeof(patch)); // Skip over normal
	}
	{
		// Hook window update
		patch_call(0x00453D72, th128_window_update_hook);
	}
	if (Config::D3D9Ex) {
		// Redirect Direct3DCreate9 call
		// IAT is being hooked, but thcrap also hooks Direct3DCreate9 in the same place
		// This should force our Direct3DCreate9 hook to be loaded no matter what
		patch_call(0x00453B8B, Direct3DCreate9_hook);

		// Extra NOP is needed because we're replacing a FF 15 call, which is 6 bytes long
		BYTE patch[] = { 0x90 };
		patch_bytes(0x00453B90, patch, sizeof(patch));
	}
	if (Config::ReplaySpeedControl) {
		// Skip the original replay speed control stuff
		BYTE patch[] = { 0xEB };
		patch_bytes(0x0044204F, patch, sizeof(patch));
	}
}

FPSTarget th128_replay_callback() {
	if (*CReplayManager::InstancePtr && (*CReplayManager::InstancePtr)->mode == 1) {
		// TODO: Reverse engineer this struct instead of being lazy
		auto input = *(DWORD*)0x004DAFF8;
		if (input & InputState::Focus)
			return FPSTarget::ReplaySlow;
		else if (input & (InputState::Skip | InputState::Shoot))
			return FPSTarget::ReplaySkip;
	}
	return FPSTarget::Game;
}