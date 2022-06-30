#pragma once
// Touhou 16: Hidden Star in Four Seasons v1.00a

#include <Windows.h>

namespace Touhou16 {
	auto CWindowManager__UpdateFast = (int(__thiscall*)(void*))0x0045AC50;

	enum InputState {
		Focus = 8,
		Skip = 0x200,
	};

	class CReplayManager {
	public:
		static CReplayManager** InstancePtr;

		BYTE gap0[12];
		int mode;
		BYTE gap10[780];
	};
	static_assert(sizeof(CReplayManager) == 0x31C, "Struct is the wrong size");
}