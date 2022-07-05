#pragma once

// BOOL is used instead of bool just in case someone tries to load a non-bool value into something that's supposed to be a bool
#include <Windows.h>
#include "games.h"

enum TargetRefreshRate {
	Max,
	Sixty,
	MultipleOfSixty,
};

enum SleepType {
	Spin,
	Vpatch,
};

// See the README for documentation on these values
class Config {
public:
	static bool Load();

	static UINT GameFPS;
	static BOOL ReplaySpeedControl;
	static UINT ReplaySkipFPS;
	static UINT ReplaySlowFPS;
	static UINT BltPrepareTime;
	static SleepType Sleep;
	static BOOL D3D9Ex;
	static TargetRefreshRate FullscreenRefreshRate;
	static BOOL ShowOverlay;
	static BOOL DebugConsole;
	static BOOL DebugWait;
	static BOOL FixInputGlitching;
	static TouhouGame GameOverride;
};