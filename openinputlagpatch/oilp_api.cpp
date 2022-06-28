// Exported functions to allow compatibility with external tools such as thprac

#include <Windows.h>
#include "limiter.h"

#define OILP_API extern "C" __declspec(dllexport)

OILP_API bool oilp_set_game_fps(int fps) {
	return Limiter::SetGameFPS(fps);
}