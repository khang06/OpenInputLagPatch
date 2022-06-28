#include <Windows.h>
#include <shlwapi.h>
#include "config.h"

UINT Config::GameFPS = 60;
UINT Config::ReplaySkipFPS = 240;
UINT Config::ReplaySlowFPS = 30;
UINT Config::BltPrepareTime = 4;
BOOL Config::D3D9Ex = TRUE;
BOOL Config::Force60Hz = FALSE;
BOOL Config::DebugConsole = FALSE;
BOOL Config::DebugWait = FALSE;
TouhouGame Config::GameOverride = TouhouGame::Unknown;

bool Config::Load() {
	// Get the config file path
	wchar_t config_path[1024] = {};
	if (!GetModuleFileNameW(NULL, config_path, MAX_PATH))
		return false;
	PathRemoveFileSpecW(config_path);
	PathAppendW(config_path, L"\\openinputlagpatch.ini");

	// Load the config
	Config::GameFPS = GetPrivateProfileInt(L"Option", L"GameFPS", Config::GameFPS, config_path);
	Config::ReplaySkipFPS = GetPrivateProfileInt(L"Option", L"ReplaySkipFPS", Config::ReplaySkipFPS, config_path);
	Config::ReplaySlowFPS = GetPrivateProfileInt(L"Option", L"ReplaySlowFPS", Config::ReplaySlowFPS, config_path);
	Config::BltPrepareTime = GetPrivateProfileInt(L"Option", L"BltPrepareTime", Config::BltPrepareTime, config_path);
	Config::D3D9Ex = GetPrivateProfileInt(L"Option", L"D3D9Ex", Config::D3D9Ex, config_path);
	Config::Force60Hz = GetPrivateProfileInt(L"Option", L"Force60Hz", Config::Force60Hz, config_path);
	Config::DebugConsole = GetPrivateProfileInt(L"Option", L"DebugConsole", Config::DebugConsole, config_path);
	Config::DebugWait = GetPrivateProfileInt(L"Option", L"DebugWait", Config::DebugWait, config_path);
	Config::GameOverride = (TouhouGame)GetPrivateProfileInt(L"Option", L"GameOverride", (int)Config::GameOverride, config_path);

	// Validate options
	if (Config::GameFPS < 60)
		Config::GameFPS = 60;
	if (Config::ReplaySkipFPS < 0)
		Config::ReplaySkipFPS = 240;
	if (Config::ReplaySlowFPS < 0)
		Config::ReplaySlowFPS = 30;
	Config::BltPrepareTime = max(0, min(Config::BltPrepareTime, 16));
	if ((int)Config::GameOverride < (int)TouhouGame::Unknown || (int)Config::GameOverride >= (int)TouhouGame::MaxValue)
		Config::GameOverride = TouhouGame::Unknown;

	return true;
}