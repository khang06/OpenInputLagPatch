﻿#pragma once

#include <vector>
#include <string>

// Games are added as support is implemented
// Not ordered in chronological order of when they were released
enum class TouhouGame {
	Unknown = -1,
	Custom = 0,
	Th6 = 1,
	Th10 = 2,
	Th7 = 3,
	Th8 = 4,
	Th9 = 5,
	Th11 = 6,
	Th12 = 7,
	Th13 = 8,
	Th14 = 9,
	Th15 = 10,
	Th16 = 11,
	Th17 = 12,
	Th18 = 13,
	Th95 = 14,
	Th125 = 15,
	Th128 = 16,
	Th143 = 17,
	Th165 = 18,
	Th185 = 19,
	MaxValue, // Dummy value used for config validation
};

enum class FPSTarget {
	Game,
	ReplaySkip,
	ReplaySlow,
};

// Function type for per-game patch functions
typedef void(*GamePatchFunction)();

// Per-game callback to check if we're currently in a replay and if the player wants to change speeds
typedef FPSTarget(*ReplayCallback)();

TouhouGame detect_game();

#ifndef OILP_LOADER
GamePatchFunction get_patch_function(TouhouGame game);
ReplayCallback get_replay_callback(TouhouGame game);
#endif

void th6_install_patches();
FPSTarget th6_replay_callback();
void th10_install_patches();
FPSTarget th10_replay_callback();
void th7_install_patches();
FPSTarget th7_replay_callback();
void th8_install_patches();
FPSTarget th8_replay_callback();
void th9_install_patches();
FPSTarget th9_replay_callback();
void th11_install_patches();
FPSTarget th11_replay_callback();
void th12_install_patches();
FPSTarget th12_replay_callback();
void th13_install_patches();
FPSTarget th13_replay_callback();
void th14_install_patches();
FPSTarget th14_replay_callback();
void th15_install_patches();
FPSTarget th15_replay_callback();
void th16_install_patches();
FPSTarget th16_replay_callback();
void th17_install_patches();
FPSTarget th17_replay_callback();
void th18_install_patches();
FPSTarget th18_replay_callback();
void th95_install_patches();
FPSTarget th95_replay_callback();
void th125_install_patches();
FPSTarget th125_replay_callback();
void th128_install_patches();
FPSTarget th128_replay_callback();
void th143_install_patches();
FPSTarget th143_replay_callback();
void th165_install_patches();
FPSTarget th165_replay_callback();
void th185_install_patches();