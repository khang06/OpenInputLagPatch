#pragma once
// Touhou 18: Unconnected Marketeers v1.00a

#include <Windows.h>

namespace Touhou18 {
	auto CWindowManager__UpdateFast = (int(__thiscall*)(void*))0x00472DD0;

	enum InputState {
		Shoot = 1,
		Focus = 8,
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