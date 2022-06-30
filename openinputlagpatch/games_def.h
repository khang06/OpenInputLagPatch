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
		"ce3ed8afede42074a993eb9d3114420ccb96e8717ae207d3257fb2cd94836126", // SA v1.00a original
		"405926e10d8bce204dd2b090ae827046b64c373d6d993289dfe40e16c81ad169", // SA v1.00a English patch
		"45730fa1f9463c65f44165a1b8682b628f4b5ee32e60db10f412a0fa4178b62f", // SA v1.00a Korean patch
		"80d404fa2f616e8043744ade0dd9441dda09f4c6be0bd60bb053ac37d4530da5", // UFO v1.00b original
		"ca124ef6ab5a7d5deb6a5dbf4b486f9b402d7d7d3193f6cb1eefe03c50ada687", // UFO v1.00b English patch
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
	},
	// Touhou 11: Subterranean Animism v1.00a
	{
		"2978b17f6184d100d249d4311348dd30c5c32ec75c014b667a525b797d3d8813", // Original
		"9ab285272251cae99984c9ba15320542ad20a642a0e98a4dd7827d64021ee4a0", // Drake's Boss Rush patch
		"18555e5055909570dbf46ca2a7cb796c50174fcdffb863a83357110d7f3f770b", // English patch
		"e333e600c27e3154f03001605ec3993456e20814212aa183d7ecbc4176a18539", // English patch + Drake's Boss Rush patch
		"f42c5d3540489e8c99ac81d88293ef7a830ad3a402d67dcd83f575816775dbd7", // Korean patch (Gungsuh font)
		"5cc754c4ddd52ee788910a479372330645969fffaade5b3a7a42c8b099e8dbbe", // Korean patch (Nanum Gothic font)
		"df6edcf5649b1df917ef91000e3047d49ba3be33301d8d6789b74b1f8b78b435", // Riri's practice patch v1.03
		"e42c8df5ba2d704fb6bd5c50d9fb35c49ebf3122dab75b19b7ed17b6bc84166d", // Old Steam release (6454988231458882848)
		"47cbcfdba984e32ba2887bc0a8c35e21aa8a039c1d87675da8493544d86e2a42", // Steam release
	},
	// Touhou 12: Undefined Fantastic Object v1.00b
	{
		"d8d644d2e64957a3031b1a1399d0502e1ddaa5252d2c4e492770ad6717827628", // Original
		"0c9ac1c756eaef681f7538af72b297877ac13f06c2ca2df743e8a29d96672d08", // Score counter fix
		"9bcc0675792a057d97a5f560c3d0cf21d07cb02f01d47a9207dcbe1c5efffee5", // English patch
		"d116fed3eef093607f8c4b55fd0ec45dfc99139a7e1913f88570db0968341ba2", // English patch + score counter fix
		"7b3d64c41cb078b5cbf5ccce11b14278690e90689f1a1c14dce15b0880efb06c", // English patch + autobomb patch
		"b851680ff45c15ae39257ee7590fcf3c8aba76c839289d94d5a5c30daadb00a1", // English patch + autobomb patch + score counter fix
		"91919e9bc48babdcb6c6eab2906d670a25634d9f8e6e4884f35dc4a61fb8bde3", // Old Steam release (6604501887647785241)
		"988f5ea067b247cde91754cb62f66ffdf71b4d5b874da8000e579b928a71829b", // Old Steam release (7325231149869118532)
		"eb275a8c285fd8450fa5ac79fa730a759aeca8b2904aeb16d30e65859372a459", // Steam release
	},
	// Touhou 13: Ten Desires v1.00c
	{
		"422f40aa176c9aab7d0178ce6033802b4143921a5e0e463ef99259ef6d100a58", // Original
		"2368f892d2d15ce96e546de33290501bccd03188b875649012d43616314d21c3", // English patch
		"1a6e3eb3b65238a739d7250606e9cb233d16b505c3340a11c4584b583388930d", // Steam release
	},
};

const char* game_to_string[] = {
	"custom.exe",
	"Touhou 6 v1.02",
	"Touhou 10 v1.00a",
	"Touhou 7 v1.00b",
	"Touhou 8 v1.00d",
	"Touhou 9 v1.50a",
	"Touhou 11 v1.00a",
	"Touhou 12 v1.00b",
	"Touhou 13 v1.00c",
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
	th11_install_patches,
	th12_install_patches,
	th13_install_patches,
};
static_assert(sizeof(game_to_patch_function) / sizeof(GamePatchFunction) == (size_t)TouhouGame::MaxValue, "Forgot to update an array");

ReplayCallback game_to_replay_callback[] = {
	nullptr,
	th6_replay_callback,
	th10_replay_callback,
	th7_replay_callback,
	th8_replay_callback,
	nullptr, // TODO: Can't just read a pointer for the input values in replay mode, we have to read input from DirectInput/GetKeyboardState manually
	th11_replay_callback,
	th12_replay_callback,
	th13_replay_callback,
};
static_assert(sizeof(game_to_replay_callback) / sizeof(ReplayCallback) == (size_t)TouhouGame::MaxValue, "Forgot to update an array");
#endif