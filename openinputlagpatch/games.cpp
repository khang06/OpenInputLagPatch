// Game detection

#include <Windows.h>
#include <vector>
#include "sha256.h"
#include "config.h"
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

	// Open it
	FILE* game_file = _wfopen(game_path, L"rb");
	if (!game_file) {
		MessageBox(NULL, L"Failed to open the game executable.", L"OpenInputLagPatch", MB_ICONERROR);
		return TouhouGame::Unknown;
	}

	// Read it into memory
	fseek(game_file, 0, SEEK_END);
	int game_size = ftell(game_file);
	fseek(game_file, 0, SEEK_SET);
	BYTE* game = new BYTE[game_size];
	if (fread(game, 1, game_size, game_file) != game_size) {
		MessageBox(NULL, L"Failed to read the game executable.", L"OpenInputLagPatch", MB_ICONERROR);
		return TouhouGame::Unknown;
	}
	fclose(game_file);

	// Hash it
	SHA256_CTX ctx = {};
	BYTE hash[SHA256_BLOCK_SIZE] = {};
	sha256_init(&ctx);
	sha256_update(&ctx, game, game_size);
	sha256_final(&ctx, hash);
	delete[] game;

	// Convert the hash to a string
	char hash_str[SHA256_BLOCK_SIZE * 2 + 1] = {};
	for (int i = 0; i < SHA256_BLOCK_SIZE; i++) {
		hash_str[i * 2] = "0123456789abcdef"[hash[i] >> 4];
		hash_str[i * 2 + 1] = "0123456789abcdef"[hash[i] & 0xF];
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
	MessageBox(NULL, L"Failed to detect the game. Please make sure that the game you're trying to play is supported.\nIf you believe this is a mistake, please open an issue on Github.", L"OpenInputLagPatch", MB_ICONERROR);
	return TouhouGame::Unknown;
}

// Get the replay callback for a specified game
// If replay detection is unimplemented, this will return null
ReplayCallback get_replay_callback(TouhouGame game) {
	return game_to_replay_callback[(size_t)game];
}