#pragma once
// Touhou 13: Ten Desires v1.00c

#include <Windows.h>

namespace Touhou13 {
	auto CWindowManager__UpdateFast = (int(__stdcall*)(void*))0x0045D2F0;

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