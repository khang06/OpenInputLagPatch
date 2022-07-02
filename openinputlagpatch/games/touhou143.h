#pragma once
// Touhou 14.3: Impossible Spell Card v1.00a

#include <Windows.h>

namespace Touhou143 {
	auto CWindowManager__UpdateFast = (int(__thiscall*)(void*))0x0046EFE0;

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
		BYTE gap14[360];
	};
	static_assert(sizeof(CReplayManager) == 0x17C, "Struct is the wrong size");
}