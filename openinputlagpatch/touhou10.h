// Touhou 10: Mountain of Faith v1.00a

#pragma once

namespace Touhou10 {
	auto CWindowManager__Update = (int(__stdcall*)(void*))0x00439390;

	// TODO: Copy-pasted from my Touhou 6 RE work
	// This is probably completely wrong
	enum InputState {
		Shoot = 1,
		Bomb = 2,
		Focus = 4,
		Pause = 8,
		Up = 0x10,
		Down = 0x20,
		Left = 0x40,
		Right = 0x80,
		Skip = 0x100,
		Q = 0x200,
		S = 0x400,
		Enter = 0x800
	};

	class CReplayManager {
	public:
		static CReplayManager** InstancePtr;

		BYTE gap0[16];
		int mode;
		BYTE gap14[704];
	};
	static_assert(sizeof(CReplayManager) == 0x2D4, "Struct is the wrong size");
}