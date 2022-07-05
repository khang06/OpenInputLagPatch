#pragma once

#include <Windows.h>
#include "games.h"

// Frame limiter based on vpatch
class Limiter {
public:
	static void Initialize(ReplayCallback replay_callback);
	static void Tick();

	static bool SetGameFPS(int fps);

private:
	static bool UpdateTargetFPS();

	static bool initialized;
	static LARGE_INTEGER start_time;
	static unsigned int frame_num;
	static LARGE_INTEGER wait_amount;
	static LARGE_INTEGER last_wait_amount;
	static LARGE_INTEGER blt_prepare_time;
	static LARGE_INTEGER perf_freq;
	static ReplayCallback replay_callback;
	static LARGE_INTEGER frame_start;
	static LARGE_INTEGER frame_end;
};