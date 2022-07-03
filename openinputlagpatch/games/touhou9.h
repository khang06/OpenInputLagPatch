#pragma once
// Touhou 9: Phantasmagoria of Flower View v1.50a

#include <Windows.h>
#include "d3d8/d3d8.h"

namespace Touhou9 {
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
	auto get_input = ((__int16(__fastcall*)(int side))0x0042B850);

	auto CWindowManager__Update = (int(__thiscall*)(void*))0x0042DC40;

	struct config_file_t
	{
		BYTE gap0[178];
		unsigned __int8 frameskip;
		BYTE gapB3[21];
		int field_C8;
	};
	static_assert(sizeof(config_file_t) == 0xCC, "Struct is the wrong size");

	class CEngine
	{
	public:
		static CEngine* Instance;

		HINSTANCE hInstance;
		IDirect3D8* d3d8;
		IDirect3DDevice8* d3d8_device;
		BYTE gapC[892];
		config_file_t config;
		BYTE gap454[312];
		int last_state;
		int state;
		BYTE gap594[536];
		int field_7AC;
	};
	static_assert(sizeof(CEngine) == 0x7B0, "Struct is the wrong size");

	class CReplay
	{
	public:
		BYTE gap0[276];
		int is_replay;
		BYTE gap118[72];
		int field_160;
	};
	static_assert(sizeof(CReplay) == 0x164, "Struct is the wrong size");
	
	class CGame
	{
	public:
		static CGame* Instance;

		BYTE gap0[212];
		CReplay* replay;
		BYTE gapD8[92];
		char flags;
		BYTE gap135[599];
		int field_38C;
	};
	static_assert(sizeof(CGame) == 0x390, "Struct is the wrong size");
}