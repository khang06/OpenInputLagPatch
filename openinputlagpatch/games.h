#pragma once

#include <vector>

// Games are added as support is implemented
// Not ordered in chronological order of when they were released
enum class TouhouGame {
	Unknown = -1,
	Custom = 0,
	Th06 = 1,
};

TouhouGame detect_game();

void th6_install_patches();