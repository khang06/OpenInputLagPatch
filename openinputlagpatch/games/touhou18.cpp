// Touhou 18: Unconnected Marketeers v1.00a

#include "patch_util.h"
#include "limiter.h"
#include "d3d9_hook.h"
#include "config.h"
#include "touhou18.h"

using namespace Touhou18;

CReplayManager** CReplayManager::InstancePtr = (CReplayManager**)0x004CF418;

int __fastcall th18_window_update_hook(void* self) {
	Limiter::Tick();
	return CWindowManager__UpdateFast(self);
}

void th18_install_patches() {
	{
		// Skip the original frame limiter
		BYTE patch[] = { 0xEB, 0x53 };
		patch_bytes((void*)0x00472E25, patch, sizeof(patch));
	}
	{
		// Force fast input latency mode
		BYTE patch[] = { 0xE9, 0xB2, 0x01, 0x00, 0x00, 0x90 };
		patch_bytes((void*)0x00471A9E, patch, sizeof(patch));
	}
	{
		// Hook window update
		patch_call((void*)0x00471C5A, th18_window_update_hook);
	}
	if (Config::D3D9Ex) {
		// Redirect Direct3DCreate9 call
		// IAT is being hooked, but thcrap also hooks Direct3DCreate9 in the same place
		// This should force our Direct3DCreate9 hook to be loaded no matter what
		patch_call((void*)0x00471634, Direct3DCreate9_hook);

		// Extra NOP is needed because we're replacing a FF 15 call, which is 6 bytes long
		BYTE patch[] = { 0x90 };
		patch_bytes((void*)0x00471639, patch, sizeof(patch));
	}
	if (Config::ReplaySpeedControl) {
		// Skip the original replay speed control stuff
		BYTE patch[] = { 0x90, 0x90 };
		patch_bytes((void*)0x00461DD3, patch, sizeof(patch));
	}
}

FPSTarget th18_replay_callback() {
	if (*CReplayManager::InstancePtr && (*CReplayManager::InstancePtr)->mode == 1) {
		// TODO: Reverse engineer this struct instead of being lazy
		auto input = *(DWORD*)0x004CA210;
		if (input & InputState::Focus)
			return FPSTarget::ReplaySlow;
		else if (input & InputState::Shoot) // LCTRL isn't read at all in this game
			return FPSTarget::ReplaySkip;
	}
	return FPSTarget::Game;
}