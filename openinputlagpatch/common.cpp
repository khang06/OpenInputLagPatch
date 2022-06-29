#include <Windows.h>
#include <stdio.h>
#include "sha256.h"

// Gets the SHA256 of a file
// Returns true on success
bool sha256_file(wchar_t* path, char* hash_str) {
	// Open it
	FILE* file = _wfopen(path, L"rb");
	if (!file) {
		return false;
	}

	// Read it into memory
	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);
	BYTE* data = new BYTE[size];
	if (fread(data, 1, size, file) != size) {
		return true;
	}
	fclose(file);

	// Hash it
	SHA256_CTX ctx = {};
	BYTE hash[SHA256_BLOCK_SIZE] = {};
	sha256_init(&ctx);
	sha256_update(&ctx, data, size);
	sha256_final(&ctx, hash);
	delete[] data;

	// Convert the hash to a string
	for (int i = 0; i < SHA256_BLOCK_SIZE; i++) {
		hash_str[i * 2] = "0123456789abcdef"[hash[i] >> 4];
		hash_str[i * 2 + 1] = "0123456789abcdef"[hash[i] & 0xF];
	}
	return true;
}

void __declspec(noreturn) panic_msgbox(const wchar_t* format, ...) {
	wchar_t buf[1024] = {};
	va_list args;
	va_start(args, format);
	vswprintf_s(buf, format, args);
	va_end(args);

	MessageBoxW(NULL, buf, L"OpenInputLagPatch", MB_ICONERROR);
	exit(1);
}