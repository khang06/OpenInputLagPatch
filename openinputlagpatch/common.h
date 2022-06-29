#pragma once

bool sha256_file(wchar_t* path, char* hash_str);
void __declspec(noreturn) panic_msgbox(const wchar_t* format, ...);