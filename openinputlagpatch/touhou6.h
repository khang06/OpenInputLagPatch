#pragma once
// Touhou 6: Embodiment of Scarlet Devil v1.02h

#include <Windows.h>
#include "d3d8/d3d8.h"

typedef struct DIDEVCAPS {
	DWORD dwSize;
	DWORD dwFlags;
	DWORD dwDevType;
	DWORD dwAxes;
	DWORD dwButtons;
	DWORD dwPOVs;
	DWORD dwFFSamplePeriod;
	DWORD dwFFMinTimeResolution;
	DWORD dwFirmwareRevision;
	DWORD dwHardwareRevision;
	DWORD dwFFDriverVersion;
} DIDEVCAPS, *LPDIDEVCAPS;

struct config_file_t
{
	int field_0;
	int field_4;
	int field_8;
	int field_C;
	int field_10;
	int field_14;
	unsigned __int8 initial_bombs;
	unsigned __int8 initial_lives;
	unsigned __int8 use_16bit_textures;
	unsigned __int8 music;
	unsigned __int8 field_1C;
	unsigned __int8 field_1D;
	unsigned __int8 fullscreen;
	unsigned __int8 frameskip;
	__int16 pad_x_axis;
	__int16 pad_y_axis;
	BYTE gap24[16];
	unsigned int engine_flags;
};
static_assert(sizeof(config_file_t) == 0x38, "Struct is the wrong size");

class CEngine
{
public:
	static CEngine* Instance;

	HINSTANCE hInstance;
	IDirect3D8* d3d8;
	IDirect3DDevice8* d3d8_device;
	void* dinput8;
	void* kb_device;
	void* joypad_device;
	DIDEVCAPS joypad_caps;
	HWND hwnd;
	D3DMATRIX look_at_matrix;
	D3DMATRIX perspective_matrix;
	D3DVIEWPORT8 viewport;
	D3DPRESENT_PARAMETERS present_params;
	config_file_t config;
	BYTE gap14C[25];
	char field_165;
	BYTE gap166[30];
	int field_184;
	int last_state;
	int state;
	int field_190;
	__declspec(align(8)) int field_198;
	BYTE gap19C[20];
	int field_1B0;
	BYTE gap1B4[8];
	void* loaded_pbg3s[16];
	char loaded_pbg3_names[16][32];
	__declspec(align(8)) int init_time;
	BYTE gap404[208];
	int field_4D4;
};
static_assert(sizeof(CEngine) == 0x4D8, "Struct is the wrong size");

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
auto CChainManager__UpdateCalcChain = (int(__thiscall*)(CChainManager*))0x0041CA10;
auto CChainManager__UpdateDrawChain = (int(__thiscall*)(CChainManager*))0x0041CAD0;

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
auto CWindowManager__Update = (int(__thiscall*)(CWindowManager*))0x004206E0;