#pragma once
// Touhou 15: Legacy of Lunatic Kingdom v1.00b

#include <Windows.h>

namespace Touhou15 {
	auto CWindowManager__UpdateFast = (int(__thiscall*)(void*))0x00472790;

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