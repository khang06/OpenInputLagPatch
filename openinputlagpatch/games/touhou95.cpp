// Touhou 9.5: Shoot the Bullet v1.02a

#include "patch_util.h"
#include "limiter.h"
#include "d3d9_hook.h"
#include "config.h"
#include "touhou95.h"

using namespace Touhou95;

CReplayManager** CReplayManager::InstancePtr = (CReplayManager**)0x004C4E74;

int __fastcall th95_window_update_hook(void* self) {
	Limiter::Tick();
	return CWindowManager__Update(self);
}

void th95_install_patches() {
	{
		// Skip the original frame limiter
		BYTE patch[] = { 0xEB };
		patch_bytes(0x004207DA, patch, sizeof(patch));
	}
	{
		// Don't sleep in the main loop
		BYTE patch[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
		patch_bytes(0x004207C3, patch, sizeof(patch));
	}
	{
		// Hook window update
		patch_call(0x0042046F, th95_window_update_hook);
	}
}

FPSTarget th95_replay_callback() {
	if (*CReplayManager::InstancePtr && (*CReplayManager::InstancePtr)->mode == 1) {
		// TODO: Reverse engineer this struct instead of being lazy
		auto input = *(DWORD*)0x004BE218;
		if (input & InputState::Focus)
			return FPSTarget::ReplaySlow;
		else if (input & (InputState::Skip | InputState::Shoot))
			return FPSTarget::ReplaySkip;
	}
	return FPSTarget::Game;
}