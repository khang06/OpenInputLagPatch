// Exported functions to allow compatibility with external tools such as thprac

#include <Windows.h>
#include "limiter.h"

#define OILP_API extern "C" __declspec(dllexport)

OILP_API bool oilp_set_game_fps(int fps) {
	if (!limiter_initialized)
		return false;
	if (fps <= 0)
		return false;
	// x86 32-bit writes are always atomic as long as they're aligned, so this should work fine from another thread
	queued_wait_amount = (DWORD)(perf_freq.QuadPart / (double)fps);
	return true;
}