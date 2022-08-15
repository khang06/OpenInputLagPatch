// Touhou 18.5: 100th Black Market v1.00a

#include "patch_util.h"
#include "limiter.h"
#include "d3d9_hook.h"
#include "config.h"
#include "touhou185.h"

using namespace Touhou185;

int __fastcall th185_window_update_hook(void* self) {
	Limiter::Tick();
	return CWindowManager__UpdateFast(self);
}

void th185_install_patches() {
	{
		// Skip the original frame limiter
		BYTE patch[] = { 0xEB, 0x42 };
		patch_bytes((void*)0x004752DE, patch, sizeof(patch));
	}
	{
		// Force fast input latency mode
		BYTE patch[] = { 0xE9, 0xB2, 0x01, 0x00, 0x00, 0x90 };
		patch_bytes((void*)0x00471A9E, patch, sizeof(patch));
	}
	{
		// Hook window update
		patch_call((void*)0x004741E8, th185_window_update_hook);
	}
	if (Config::D3D9Ex) {
		// Redirect Direct3DCreate9 call
		// IAT is being hooked, but thcrap also hooks Direct3DCreate9 in the same place
		// This should force our Direct3DCreate9 hook to be loaded no matter what
		patch_call((void*)0x00473BD9, Direct3DCreate9_hook);

		// Extra NOP is needed because we're replacing a FF 15 call, which is 6 bytes long
		BYTE patch[] = { 0x90 };
		patch_bytes((void*)0x00473BDE, patch, sizeof(patch));
	}
}