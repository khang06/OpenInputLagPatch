#pragma once
// Touhou 14: Double Dealing Character v1.00b

#include <Windows.h>

namespace Touhou14 {
	auto CWindowManager__UpdateFast = (int(__thiscall*)(void*))0x0046A720;

	enum InputState {
		Focus = 8,
		Skip = 0x200,
	};

	class CReplayManager {
	public:
		static CReplayManager** InstancePtr;

		BYTE gap0[16];
		int mode;
		BYTE gap14[780];
	};
	static_assert(sizeof(CReplayManager) == 0x320, "Struct is the wrong size");
}