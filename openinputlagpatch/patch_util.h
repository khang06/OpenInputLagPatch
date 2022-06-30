#pragma once
#include <Windows.h>

void patch_bytes(void* dst, void* src, size_t len);
void patch_call(void* target, void* func);
void* iat_hook(LPCWSTR target, LPCSTR dll, LPCSTR func, void* hook);