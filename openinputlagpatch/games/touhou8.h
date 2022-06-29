#pragma once
// Touhou 8: Imperishable Night v1.00d

#include <Windows.h>
#include "d3d8/d3d8.h"

namespace Touhou8 {
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
	auto get_input = ((__int16(__cdecl*)())0x0043D970);

	auto CWindowManager__Update = (int(__thiscall*)(void*))0x00441E70;

	struct config_file_t
	{
		BYTE gap0[35];
		unsigned __int8 frameskip;
		BYTE gap24[20];
		int field_38;
	};
	static_assert(sizeof(config_file_t) == 0x3C, "Struct is the wrong size");

	class CEngine
	{
	public:
		static CEngine* Instance;

		HINSTANCE hInstance;
		int d3d8;
		IDirect3DDevice8* d3d8_device;
		BYTE gapC[188];
		D3DVIEWPORT8 field_C8;
		D3DPRESENT_PARAMETERS present_params;
		BYTE gap114[4];
		config_file_t config;
		int field_154;
		int last_state;
		int state;
		BYTE gap160[496];
		int field_350;
		BYTE gap354[12];
		int field_360;
	};
	static_assert(sizeof(CEngine) == 0x364, "Struct is the wrong size");

	class CGame
	{
	public:
		static CGame* Instance;

		BYTE gap0[252844];
		unsigned int flags;
		BYTE gap3DBB0[648];
		int field_3DE38;
	};
	static_assert(sizeof(CGame) == 0x3DE3C, "Struct is the wrong size");
}