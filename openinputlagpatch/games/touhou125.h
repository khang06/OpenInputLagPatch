#pragma once
// Touhou 12.5: Double Spoiler v1.00a

#include <Windows.h>

namespace Touhou125 {
	auto CWindowManager__UpdateFast = (int(__stdcall*)(void*))0x0044F0D0;

	enum InputState {
		Shoot = 1,
		Focus = 4,
		Skip = 0x80,
	};

	class CReplayManager {
	public:
		static CReplayManager** InstancePtr;

		BYTE gap0[16];
		int mode;
		BYTE gap14[7576];
		int field_1DAC;
	};
	static_assert(sizeof(CReplayManager) == 0x1DB0, "Struct is the wrong size");
}