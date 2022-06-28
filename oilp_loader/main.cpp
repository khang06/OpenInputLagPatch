#include <Windows.h>
#include <conio.h>
#include <shlwapi.h>

#include "sha256.h"
#include "games.h"
#include "games_def.h"
#include "util.h"

TouhouGame detect_game(const wchar_t* game_path) {
	FILE* game_file = _wfopen(game_path, L"rb");
	if (!game_file) {
		return TouhouGame::Unknown;
	}

	// Read it into memory
	fseek(game_file, 0, SEEK_END);
	int game_size = ftell(game_file);
	fseek(game_file, 0, SEEK_SET);
	BYTE* game = new BYTE[game_size];
	if (fread(game, 1, game_size, game_file) != game_size) {
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

	// Check if the game matches any known hashes
	for (size_t i = 0; i < game_hashes.size(); i++) {
		for (size_t j = 0; j < game_hashes[i].size(); j++) {
			if (!strcmp(hash_str, game_hashes[i][j])) {
				return (TouhouGame)i;
			}
		}
	}

	// Couldn't find it
	return TouhouGame::Unknown;
}

int run_game_and_inject(const wchar_t* path) {
	STARTUPINFOW si = { sizeof(si) };
	PROCESS_INFORMATION pi = {};
	if (!CreateProcessW(path, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi))
		return -1;
	LPVOID remoteStr = VirtualAllocEx(pi.hProcess, NULL, 44, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!remoteStr)
		return -2;
	defer(VirtualFreeEx(pi.hProcess, remoteStr, 0, MEM_RELEASE));
	DWORD byteRet;
	if (!WriteProcessMemory(pi.hProcess, remoteStr, L"openinputlagpatch.dll", 42, &byteRet)) {
		return -3;
	}
	HANDLE hRemoteThread = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)::LoadLibraryW, remoteStr, 0, NULL);
	if (hRemoteThread == NULL) {
		return -4;
	}
	defer(CloseHandle(hRemoteThread));
	WaitForSingleObject(hRemoteThread, INFINITE);
	DWORD exitCode = 0;
	if (!GetExitCodeThread(hRemoteThread, &exitCode) || !exitCode)
		return -5;
	ResumeThread(pi.hThread);
	return 0;
}

std::string cmd_inp() {
	std::string ret;
	char a;
	for (;;) {
		a = getchar();
		if (a == '\n') break;
		ret.push_back(a);
	}
	return ret;
}

int wmain() {
	struct _TouhouGameFind {
		TouhouGame game;
		std::wstring path;
	};
	WIN32_FIND_DATAW find;
	HANDLE hFind = FindFirstFileW(L"*.exe", &find);
	std::vector<_TouhouGameFind> games;
	std::wstring err;
	if (hFind == INVALID_HANDLE_VALUE) {
		puts("No valid games found");
		return 1;
	}
	do {
		TouhouGame game = detect_game(find.cFileName);
		if (game != TouhouGame::Unknown && game != TouhouGame::Custom) {
			games.push_back({ game, find.cFileName });
		}
	} while (FindNextFileW(hFind, &find));
	if (games.size() == 1)
		return run_game_and_inject(games[0].path.c_str());
	else if (games.size() == 0) {
		puts("No valid games found");
		return 1;
	}
	for (size_t i = 0; i < games.size(); i++) {
		wprintf(L"[%d]: %S (%s)\n", i, game_to_string[(size_t)games[i].game], games[i].path.c_str());
	}
	
	size_t sel = 0;
	do {
		printf("Pick your game: ");
		sel = std::stod(cmd_inp());
	} while (sel >= games.size());
	return run_game_and_inject(games[sel].path.c_str());
}