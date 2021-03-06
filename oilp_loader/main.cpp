#include <Windows.h>
#include <conio.h>

#include "sha256.h"
#include "games.h"
#include "games_def.h"
#include "common.h"

TouhouGame detect_game(wchar_t* game_path) {
	// Hash the game
	char hash_str[SHA256_BLOCK_SIZE * 2 + 1] = {};
	if (!sha256_file(game_path, hash_str)) {
		return TouhouGame::Unknown;
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
		try {
			sel = std::stoul(cmd_inp());
		} catch (...) {
			sel = games.size();
		}
	} while (sel >= games.size());
	return run_game_and_inject(games[sel].path.c_str());
}