#pragma once
// Touhou 7: Perfect Cherry Blossom v1.00b

#include <Windows.h>
#include "d3d8/d3d8.h"

namespace Touhou7 {
	struct config_file_t
	{
		BYTE gap0[35];
		unsigned __int8 frameskip;
		BYTE gap24[16];
		int field_34;
	};
	static_assert(sizeof(config_file_t) == 0x38, "Struct is the wrong size");

	class CWindowManager
	{
	public:
		static CWindowManager* Instance;

		HWND hwnd;
		int exiting;
		int activated;
		int deactivated;
		unsigned __int8 frames_skipped;
		int field_14;
		int field_18;
		int field_1C;
	};
	static_assert(sizeof(CWindowManager) == 0x20, "Struct is the wrong size");
	auto CWindowManager__Update = (int(__thiscall*)(CWindowManager*))0x004346E0;

	class CEngine
	{
	public:
		static CEngine* Instance;

		HINSTANCE hInstance;
		IDirect3D8* d3d8;
		IDirect3DDevice8* d3d8_device;
		BYTE gapC[188];
		D3DVIEWPORT8 viewport;
		D3DPRESENT_PARAMETERS present_params;
		__declspec(align(8)) config_file_t config;
		int field_150;
		int last_state;
		int state;
		BYTE gap15C[352];
		int field_2BC;
		BYTE gap2C0[12];
		int field_2CC;
	};
	static_assert(sizeof(CEngine) == 0x2D0, "Struct is the wrong size");
	auto CEngine__UpdateFog = (void(__thiscall*)(CEngine*))0x0043A207;

	struct CGame
	{
	public:
		static CGame* Instance;

		BYTE gap0[37848];
		unsigned int flags;
		BYTE gap93DC[612];
		int field_9640;
	};
	static_assert(sizeof(CGame) == 0x9644, "Struct is the wrong size");

	struct CChainNode
	{
		__int16 priority;
		__int16 flags;
		int(__cdecl* update)(void*);
		int(__cdecl* init)(void*);
		void(__cdecl* destroy)(void*);
		CChainNode* back;
		CChainNode* next;
		CChainNode* field_18;
		void* data;
	};
	static_assert(sizeof(CChainNode) == 0x20, "Struct is the wrong size");

	class CChainManager
	{
	public:
		static CChainManager* Instance;

		CChainNode calc_root;
		CChainNode draw_root;
	};
	static_assert(sizeof(CChainManager) == 0x40, "Struct is the wrong size");
	auto CChainManager__UpdateCalcChain = (int(__thiscall*)(CChainManager*))0x0042FD60;
	auto CChainManager__UpdateDrawChain = (int(__thiscall*)(CChainManager*))0x0042FE20;

	class CTextureManager
	{
	public:
		static CTextureManager** InstancePtr;

		BYTE gap0[1566044];
		int field_17E55C;
	};
	static_assert(sizeof(CTextureManager) == 0x17e560, "Struct is the wrong size");
	auto CTextureManager__Unk44F580 = (void(__thiscall*)(CTextureManager*))0x0044F580;
	auto CTextureManager__Flush = (void(__thiscall*)(CTextureManager*))0x0044F5C0;

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
	auto get_input = ((__int16(__cdecl*)())0x00430B50);
}