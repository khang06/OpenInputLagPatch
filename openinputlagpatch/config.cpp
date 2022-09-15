#include <Windows.h>
#include <shlwapi.h>
#include "config.h"

// Default config values
UINT Config::GameFPS = 60;
BOOL Config::ReplaySpeedControl = TRUE;
UINT Config::ReplaySkipFPS = 240;
UINT Config::ReplaySlowFPS = 30;
UINT Config::BltPrepareTime = 2;
SleepType Config::Sleep = SleepType::Vpatch;
BOOL Config::D3D9Ex = TRUE;
TargetRefreshRate Config::FullscreenRefreshRate = TargetRefreshRate::MultipleOfSixty;
BOOL Config::ShowOverlay = TRUE;
BOOL Config::DebugConsole = FALSE;
BOOL Config::DebugWait = FALSE;
BOOL Config::FixInputGlitching = FALSE;
TouhouGame Config::GameOverride = TouhouGame::Unknown;

// Helper macro for loading a specific setting value
#define LOAD_SETTING(x) Config::x = (decltype(Config::x))GetPrivateProfileInt(TEXT("Option"), TEXT(#x), (int)Config::x, config_path)

bool Config::Load() {
	// Get the config file path
	wchar_t config_path[1024] = {};
	if (!GetModuleFileNameW(NULL, config_path, MAX_PATH))
		return false;
	PathRemoveFileSpecW(config_path);
	PathAppendW(config_path, L"\\openinputlagpatch.ini");

	// Load the config
	LOAD_SETTING(GameFPS);
	LOAD_SETTING(ReplaySpeedControl);
	LOAD_SETTING(ReplaySkipFPS);
	LOAD_SETTING(ReplaySlowFPS);
	LOAD_SETTING(BltPrepareTime);
	LOAD_SETTING(Sleep);
	LOAD_SETTING(D3D9Ex);
	LOAD_SETTING(FullscreenRefreshRate);
	LOAD_SETTING(ShowOverlay);
	LOAD_SETTING(DebugConsole);
	LOAD_SETTING(DebugWait);
	LOAD_SETTING(FixInputGlitching);
	LOAD_SETTING(GameOverride);

	// Validate options
	if (Config::GameFPS < 60)
		Config::GameFPS = 60;
	if (Config::ReplaySkipFPS < 0)
		Config::ReplaySkipFPS = 240;
	if (Config::ReplaySlowFPS < 0)
		Config::ReplaySlowFPS = 30;
	Config::BltPrepareTime = max(0, min(Config::BltPrepareTime, 16));
	if ((int)Config::Sleep > (int)SleepType::Vpatch)
		Config::Sleep = SleepType::Vpatch;
	if ((int)Config::FullscreenRefreshRate > (int)TargetRefreshRate::MultipleOfSixty)
		Config::FullscreenRefreshRate = TargetRefreshRate::MultipleOfSixty;
	if ((int)Config::GameOverride < (int)TouhouGame::Unknown || (int)Config::GameOverride >= (int)TouhouGame::MaxValue)
		Config::GameOverride = TouhouGame::Unknown;

	return true;
}

#undef LOAD_SETTING