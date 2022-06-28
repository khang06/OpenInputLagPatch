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
static_assert(sizeof(CWindowManager) == 0x20, "Struct is the wrong size");
auto CWindowManager__Update = (int(__thiscall*)(CWindowManager*))0x004206E0;

class CGame
{
public:
	static CGame* Instance;

	unsigned int visible_score;
	int score;
	int visible_score_increment;
	int high_score;
	int difficulty;
	BYTE gap14[8];
	int is_replay;
	BYTE gap20[4];
	int bombs_used;
	BYTE gap28[4];
	char field_2C;
	BYTE gap2D[4111];
	int field_103C;
	BYTE gap1040[2000];
	__int16 power;
	__declspec(align(8)) char field_1818;
	char field_1819;
	char lives;
	char bombs;
	char life_bonus_idx;
	unsigned __int8 is_marisa;
	unsigned __int8 weapon;
	char pause_state;
	char game_over;
	char field_1821;
	char field_1822;
	char field_1823;
	char title_demo;
	int title_demo_timer;
	int field_182C;
	BYTE gap1830[508];
	__int16 field_1A2C;
	int field_1A30;
	int stage;
	BYTE gap1A38[4];
	float field_1A3C;
	float field_1A40;
	float field_1A44;
	float field_1A48;
	BYTE gap1A4C[36];
	int rank;
	int max_rank;
	int min_rank;
	int subrank;
};
static_assert(sizeof(CGame) == 0x1A80, "Struct is the wrong size");

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
auto get_input = ((__int16(__cdecl*)())0x0041D820);