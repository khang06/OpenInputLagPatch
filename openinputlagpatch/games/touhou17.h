#pragma once
// Touhou 17: Hidden Star in Four Seasons v1.00b

#include <Windows.h>

namespace Touhou17 {
	auto CWindowManager__UpdateFast = (int(__thiscall*)(void*))0x00462190;

	enum InputState {
		Focus = 8,
		Skip = 0x200,
	};

	class CReplayManager {
	public:
		static CReplayManager** InstancePtr;

		BYTE gap0[12];
		int mode;
		BYTE gap10[784];
	};
	static_assert(sizeof(CReplayManager) == 0x320, "Struct is the wrong size");
}