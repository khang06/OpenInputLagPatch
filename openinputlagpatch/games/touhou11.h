#pragma once
// Touhou 11: Subterranean Animism v1.00a

#include <Windows.h>

namespace Touhou11 {
	auto CWindowManager__UpdateFast = (int(__stdcall*)(void*))0x00446420;

	enum InputState {
		Focus = 8,
		Skip = 0x200,
	};

	class CReplayManager {
	public:
		static CReplayManager** InstancePtr;

		BYTE gap0[16];
		int mode;
		BYTE gap14[712];
	};
	static_assert(sizeof(CReplayManager) == 0x2DC, "Struct is the wrong size");
}