#pragma once

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
	MaxValue, // Dummy value used for config validation
};

enum class FPSTarget {
	Game,
	ReplaySkip,
	ReplaySlow,
};

// Per-game callback to check if we're currently in a replay and if the player wants to change speeds
typedef FPSTarget(*ReplayCallback)();

TouhouGame detect_game();

#ifndef OILP_LOADER
ReplayCallback get_replay_callback(TouhouGame game);
#endif

void th6_install_patches();
FPSTarget th6_replay_callback();
void th10_install_patches();
FPSTarget th10_replay_callback();
void th7_install_patches();
FPSTarget th7_replay_callback();