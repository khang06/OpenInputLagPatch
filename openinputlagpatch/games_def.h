#pragma once
#include <vector>

// SHA256 hashes of supported game executables
// Sourced from https://github.com/thpatch/thcrap-tsa/blob/master/base_tsa/versions.js
std::vector<std::vector<const char*>> game_hashes{
	// custom.exe
	{
		"5ab1f4e8bf31fc4f8484ab0882a882f0fb399ba758fc98601ae9a6f6e69b5cdf", // EoSD v1.02h original
		"27ee5bb9028c86789d66b8689b74e6cbf523dfaf28b28e47766357f662a0b9b0", // EoSD v1.02h English patch
		"814373224e8dba9b43886bd0663f44148c314a4cea9af63dce37bf2b7f65590d", // MoF v1.00a original
		"01557b314306c2af5f03cacb13217c1b158c36f4ce61e9d978d036b4c3dda62e", // MoF v1.00a English patch
		"ae36be0c4ff88b5100c65ff63819442e34d60df818da5cc05fdbb61e77ae9fb7", // PCB v1.00b original
		"ea4e79061eca4904a5ec0f2803a93bf0853d559d70a3b7bb84a041c0f29350ab", // PCB v1.00b Chinese patch
		"aff0f325a34559f9f8df5778bdf30f2f538a039689323f670410f6d6357a2845", // IN v1.00d original
		"ca38af8553e3912df4a0badf81760538bbd07785380a02823c849454d776fe84", // IN v1.00d English patch
		"e5a394da95e69678dca7f4d44ce5bf80101f48d1903c9e95ebcbdf3b5199e6f4", // IN v1.00d Chinese patch
		"13206ce957266aac12cbe3d7b59076c12d17f17acca31fc4a8e642f61f31b7c3", // PoFV v1.50a original
		"a804e44167ed460598372f1893a87791592e389cea9a54b22d6e6cb0b23915b6", // PoFV v1.50a English patch
	},
	// Touhou 6: Embodiment of Scarlet Devil v1.02h
	{
		"9f76483c46256804792399296619c1274363c31cd8f1775fafb55106fb852245", // Original
		"7f38496b31b8625196900a69cd1bfed243cab1f9612073e7881dc354b876fd39", // Original with 紅魔郷 removed from the .dat file names
		"6770316f1ef5b70ff67f5966bb9d56c751145511aa942fb39290a9cc5ee6b1da", // Original with 紅魔郷 removed from the .dat file names and openinputlagpatch.dll import
		"0883c6433b77af87054d9d05731649c79926a55037840c72d33e50635d64d506", // English patch
		"fa6562ddfd81f3010d7d87792a69aaa950e0f60b00e42885ef2c30577d8dbe45", // English patch with a leftover .cfg removed
		"8a509709650a83db6850c3498e1e1051dddc6cd46791911c0d78438be8968195", // Russian patch
	},
	// Touhou 10: Mountain of Faith v1.00a
	{
		"2f14760b6fbbf57549541583283badb9a19a4222b90f0a146d5aa17f01dc9040", // Original
		"1c1d780bdc5db77b5e1f5397c0ad65fec65ae1dfb06f88d3a118651922f5ced4", // Original, openinputlagpatch.dll import
		"d28e36a6596a4c64b441c148d65a8c12b68e4a683f1d14e7df83d43f92c57d71", // English patch
		"3bdb72cf3d7c33c183359d368c801490dbcf54e6b3b2f060b95d72250b6866a3", // Steam release
	},
	// Touhou 7: Perfect Cherry Blossom v1.00b
	{
		"35467eaf8dc7fc85f024f16fb2037255f151cefda33cf4867bc9122aaa2e80ca", // Original
		"275cfdcab6ad463be4d3d646ddc21811dc65cb1c7e88c7b18bb8a89f25316655", // English patch
	},
	// Touhou 8: Imperishable Night v1.00d
	{
		"330fbdbf58a710829d65277b4f312cfbb38d5448b3df523e79350b879213d924", // Original
		"467085c3c85ceed4b6f5be410e14cd56f1af0b5afa4e0c752270b56ecbc470cf", // English patch
	},
	// Touhou 9: Phantasmagoria of Flower View v1.50a
	{
		"10350095bcf95edb59e03bee9849a2dc8a7714b4927ad5909c569c550fce6822", // Original
		"f402287326bcee15b6eda743f1edb7882294257d65c8d828d849110cb0c53bb6", // English patch
	}
};

const char* game_to_string[] = {
	"custom.exe",
	"Touhou 6 v1.02",
	"Touhou 10 v1.00a",
	"Touhou 7 v1.00b",
	"Touhou 8 v1.00d",
	"Touhou 9 v1.50a",
};
static_assert(sizeof(game_to_string) / sizeof(const char*) == (size_t)TouhouGame::MaxValue, "Forgot to update an array");

#ifndef OILP_LOADER
#include "games.h"
GamePatchFunction game_to_patch_function[] = {
	nullptr,
	th6_install_patches,
	th10_install_patches,
	th7_install_patches,
	th8_install_patches,
	th9_install_patches,
};
static_assert(sizeof(game_to_patch_function) / sizeof(GamePatchFunction) == (size_t)TouhouGame::MaxValue, "Forgot to update an array");

ReplayCallback game_to_replay_callback[] = {
	nullptr,
	th6_replay_callback,
	th10_replay_callback,
	th7_replay_callback,
	th8_replay_callback,
	nullptr, // TODO: Can't just read a pointer for the input values in replay mode, we have to read input from DirectInput/GetKeyboardState manually
};
static_assert(sizeof(game_to_replay_callback) / sizeof(ReplayCallback) == (size_t)TouhouGame::MaxValue, "Forgot to update an array");
#endif