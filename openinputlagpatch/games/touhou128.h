#pragma once
// Touhou 12.8: Great Fairy Wars v1.00a

#include <Windows.h>

namespace Touhou128 {
	auto CWindowManager__UpdateFast = (int(__stdcall*)(void*))0x00454BD0;

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
		BYTE gap14[1180];
		int field_4B0;
	};
	static_assert(sizeof(CReplayManager) == 0x4B4, "Struct is the wrong size");
}