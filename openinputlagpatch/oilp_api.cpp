// Exported functions to allow compatibility with external tools such as thprac

#include <Windows.h>
#include "config.h"
#include "limiter.h"

#define OILP_API extern "C" __declspec(dllexport)

OILP_API bool __stdcall oilp_set_game_fps(int fps) {
	return Limiter::SetGameFPS(fps);
}

OILP_API int __stdcall oilp_get_game_fps() {
	return Config::GameFPS;
}