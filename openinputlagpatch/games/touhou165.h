#pragma once
// Touhou 16.5: Violet Detector v1.00a

#include <Windows.h>

namespace Touhou165 {
	auto CWindowManager__UpdateFast = (int(__thiscall*)(void*))0x00460F50;

	enum InputState {
		Shoot = 1,
		Focus = 8,
		Skip = 0x200,
	};

	class CReplayManager {
	public:
		static CReplayManager** InstancePtr;

		BYTE gap0[12];
		int mode;
		BYTE gap10[360];
	};
	static_assert(sizeof(CReplayManager) == 0x178, "Struct is the wrong size");
}