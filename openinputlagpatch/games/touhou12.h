#pragma once
// Touhou 12: Undefined Fantastic Object v1.00b

#include <Windows.h>

namespace Touhou12 {
	auto CWindowManager__UpdateFast = (int(__stdcall*)(void*))0x004503F0;

	enum InputState {
		Shoot = 1,
		Focus = 8,
		Skip = 0x200,
	};

	class CReplayManager {
	public:
		static CReplayManager** InstancePtr;

		BYTE gap0[16];
		int mode;
		BYTE gap14[716];
	};
	static_assert(sizeof(CReplayManager) == 0x2E0, "Struct is the wrong size");
}