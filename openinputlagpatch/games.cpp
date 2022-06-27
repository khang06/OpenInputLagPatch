// Game detection

#include <Windows.h>
#include "sha256.h"
#include "games.h"

// SHA256 hashes of supported game executables
// Sourced from https://github.com/thpatch/thcrap-tsa/blob/master/base_tsa/versions.js
std::vector<std::vector<const char*>> game_hashes{
	// custom.exe
	{
		"5ab1f4e8bf31fc4f8484ab0882a882f0fb399ba758fc98601ae9a6f6e69b5cdf", // EoSD v1.02h original
		"27ee5bb9028c86789d66b8689b74e6cbf523dfaf28b28e47766357f662a0b9b0", // EoSD v1.02h English patch
	},
	// Touhou 6: Embodiment of Scarlet Devil v1.02h
	{
		"9f76483c46256804792399296619c1274363c31cd8f1775fafb55106fb852245", // Original
		"7f38496b31b8625196900a69cd1bfed243cab1f9612073e7881dc354b876fd39", // Original with 紅魔郷 removed from the .dat file names
		"0883c6433b77af87054d9d05731649c79926a55037840c72d33e50635d64d506", // English patch
		"fa6562ddfd81f3010d7d87792a69aaa950e0f60b00e42885ef2c30577d8dbe45", // English patch with a leftover .cfg removed
		"8a509709650a83db6850c3498e1e1051dddc6cd46791911c0d78438be8968195", // Russian patch
	},
};

const char* game_to_string[] = {
	"custom.exe",
	"Touhou 6 v1.02"
};

// Try to detect the current game
TouhouGame detect_game() {
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
	for (int i = 0; i < game_hashes.size(); i++) {
		for (int j = 0; j < game_hashes[i].size(); j++) {
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