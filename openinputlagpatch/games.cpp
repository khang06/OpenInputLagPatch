// Game detection

#include <Windows.h>
#include "sha256.h"
#include "config.h"
#include "games.h"

// SHA256 hashes of supported game executables
// Sourced from https://github.com/thpatch/thcrap-tsa/blob/master/base_tsa/versions.js
std::vector<std::vector<const char*>> game_hashes{
	// custom.exe
	{
		"5ab1f4e8bf31fc4f8484ab0882a882f0fb399ba758fc98601ae9a6f6e69b5cdf", // EoSD v1.02h original
		"27ee5bb9028c86789d66b8689b74e6cbf523dfaf28b28e47766357f662a0b9b0", // EoSD v1.02h English patch
		"814373224e8dba9b43886bd0663f44148c314a4cea9af63dce37bf2b7f65590d", // MoF v1.00a original
		"01557b314306c2af5f03cacb13217c1b158c36f4ce61e9d978d036b4c3dda62e", // MoF v1.00a English patch
	},
	// Touhou 6: Embodiment of Scarlet Devil v1.02h
	{
		"9f76483c46256804792399296619c1274363c31cd8f1775fafb55106fb852245", // Original
		"7f38496b31b8625196900a69cd1bfed243cab1f9612073e7881dc354b876fd39", // Original with 紅魔郷 removed from the .dat file names
		"0883c6433b77af87054d9d05731649c79926a55037840c72d33e50635d64d506", // English patch
		"fa6562ddfd81f3010d7d87792a69aaa950e0f60b00e42885ef2c30577d8dbe45", // English patch with a leftover .cfg removed
		"8a509709650a83db6850c3498e1e1051dddc6cd46791911c0d78438be8968195", // Russian patch
	},
	// Touhou 10: Mountain of Faith v1.00a
	{
		"2f14760b6fbbf57549541583283badb9a19a4222b90f0a146d5aa17f01dc9040", // Original
		"d28e36a6596a4c64b441c148d65a8c12b68e4a683f1d14e7df83d43f92c57d71", // English patch
		"3bdb72cf3d7c33c183359d368c801490dbcf54e6b3b2f060b95d72250b6866a3", // Steam release
	},
};

const char* game_to_string[] = {
	"custom.exe",
	"Touhou 6 v1.02",
	"Touhou 10 v1.00a",
};

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