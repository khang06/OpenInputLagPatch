#pragma once
// Touhou 9.5: Shoot the Bullet v1.02a

#include <Windows.h>

namespace Touhou95 {
	enum InputState {
		Shoot = 2,
		Focus = 4,
		Skip = 0x100,
	};

	auto CWindowManager__Update = (int(__thiscall*)(void*))0x00420770;

	class CReplayManager {
	public:
		static CReplayManager** InstancePtr;

		int mode;
		BYTE gap4[292];
		int field_128;
	};
	static_assert(sizeof(CReplayManager) == 0x12C, "Struct is the wrong size");
}