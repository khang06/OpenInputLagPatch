// Game detection

#include <Windows.h>
#include <vector>
#include "sha256.h"
#include "config.h"
#include "common.h"
#include "games.h"
#include "games_def.h"

// Try to detect the current game
TouhouGame detect_game() {
	if (Config::GameOverride != TouhouGame::Unknown) {
		printf("Game is being overridden to %s, things are probably about to break!\n", game_to_string[(size_t)Config::GameOverride]);
		return Config::GameOverride;
	}

	// Get the path to the game's executable
	wchar_t game_path[1024] = {};
	if (!GetModuleFileName(NULL, game_path, 1024)) {
		MessageBox(NULL, L"Unable to get the game's path.", L"OpenInputLagPatch", MB_ICONERROR);
		return TouhouGame::Unknown;
	}

	// Hash it
	char hash_str[SHA256_BLOCK_SIZE * 2 + 1] = {};
	if (!sha256_file(game_path, hash_str)) {
		MessageBox(NULL, L"Failed to hash the game.", L"OpenInputLagPatch", MB_ICONERROR);
		return TouhouGame::Unknown;
	}
	printf("Game hash: %s\n", hash_str);

	// Check if the game matches any known hashes
	for (size_t i = 0; i < game_hashes.size(); i++) {
		for (size_t j = 0; j < game_hashes[i].size(); j++) {
			if (!strcmp(hash_str, game_hashes[i][j])) {
				printf("Detected game: %s\n", game_to_string[i]);
				return (TouhouGame)i;
			}
		}
	}

	// Couldn't find it
	MessageBox(
		NULL,
		L"Failed to detect the game. Please make sure that the game you're trying to play is supported.\n"
		L"If you believe this is a mistake, please open an issue on Github.",
		L"OpenInputLagPatch",
		MB_ICONERROR
	);
	return TouhouGame::Unknown;
}

// Get the per-game patch function for a specified game
GamePatchFunction get_patch_function(TouhouGame game) {
	return game_to_patch_function[(size_t)game];
}

// Get the replay callback for a specified game
// If replay detection is unimplemented, this will return null
ReplayCallback get_replay_callback(TouhouGame game) {
	return game_to_replay_callback[(size_t)game];
}