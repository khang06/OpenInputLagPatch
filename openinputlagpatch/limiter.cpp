// Basic frame limiter
// Heavily simplified version of vpatch's limiter with no timers or fancy autobltpreparetime

#include <Windows.h>
#include <stdio.h>
#include "d3d9_hook.h"
#include "config.h"
#include "common.h"
#include "limiter.h"

/*
bool limiter_initialized = false;
LARGE_INTEGER limiter_start;
unsigned int frame_num;
LARGE_INTEGER wait_amount;
LARGE_INTEGER blt_prepare_time;
LARGE_INTEGER perf_freq;
DWORD queued_wait_amount = 0;

void limiter_init() {
	QueryPerformanceFrequency(&perf_freq);
	QueryPerformanceCounter(&limiter_start);
	wait_amount.QuadPart = (LONGLONG)((double)perf_freq.QuadPart / (double)Config::GameFPS);
	queued_wait_amount = wait_amount.LowPart;
	blt_prepare_time.QuadPart = perf_freq.QuadPart / 1000 * (LONGLONG)Config::BltPrepareTime;
	limiter_initialized = true;
}

void limiter_tick() {
	__int64 target = limiter_start.QuadPart + ++frame_num * wait_amount.QuadPart;
	LARGE_INTEGER cur_time;
	QueryPerformanceCounter(&cur_time);
	if (target - blt_prepare_time.QuadPart >= cur_time.QuadPart && queued_wait_amount == wait_amount.LowPart) {
		while (cur_time.QuadPart < target)
			QueryPerformanceCounter(&cur_time);
	} else {
		printf("Frame limiter fell behind or target FPS has changed. Resyncing...\n");
		wait_amount.LowPart = queued_wait_amount;
		if (d3d9ex_device)
			d3d9ex_device->WaitForVBlank(0);
		QueryPerformanceCounter(&cur_time);
		limiter_start.QuadPart = cur_time.QuadPart;
		frame_num = 0;
	}
}
*/

bool Limiter::initialized = false;
LARGE_INTEGER Limiter::start_time;
unsigned int Limiter::frame_num = 0;
LARGE_INTEGER Limiter::wait_amount;
LARGE_INTEGER Limiter::last_wait_amount;
LARGE_INTEGER Limiter::blt_prepare_time;
LARGE_INTEGER Limiter::perf_freq;
ReplayCallback Limiter::replay_callback = nullptr;

// Initializes the limiter's timers, settings, etc
void Limiter::Initialize(ReplayCallback callback) {
	replay_callback = callback;
	QueryPerformanceFrequency(&perf_freq);
	QueryPerformanceCounter(&start_time);

	last_wait_amount.QuadPart = 0;

	initialized = true;
}

// Updates the limiter's parameters to reflect things such as replay skipping or external FPS changes via the API
// Returns true if the player is skipping or slowing down a replay
bool Limiter::UpdateTargetFPS() {
	UINT target = Config::GameFPS;
	if (Config::ReplaySpeedControl && replay_callback) {
		switch (replay_callback()) {
			case FPSTarget::Game:
				target = Config::GameFPS;
				break;
			case FPSTarget::ReplaySkip:
				target = Config::ReplaySkipFPS;
				break;
			case FPSTarget::ReplaySlow:
				target = Config::ReplaySlowFPS;
				break;
		}
	}
	wait_amount.QuadPart = (LONGLONG)((double)perf_freq.QuadPart / (double)target);
	blt_prepare_time.QuadPart = min(wait_amount.QuadPart / 2, perf_freq.QuadPart / 1000 * (LONGLONG)Config::BltPrepareTime);
	return target != Config::GameFPS;
}

// Exposed function for outside tools such as thprac to set the framerate
bool Limiter::SetGameFPS(int fps) {
	if (!initialized)
		return false;
	if (fps <= 0)
		return false;
	Config::GameFPS = fps;
	return true;
}

// Performs the actual frame limiting
void Limiter::Tick() {
	if (!initialized)
		panic_msgbox(L"Tried to tick the limiter before initialization.");

	// Set up the target time before returning
	bool temp_fps_change = UpdateTargetFPS();
	__int64 target = start_time.QuadPart + ++frame_num * wait_amount.QuadPart;
	if (!temp_fps_change) // Don't care about input latency if skipping/slowing down a replay
		target -= blt_prepare_time.QuadPart;

	// Perform the frame limiting
	LARGE_INTEGER cur_time;
	QueryPerformanceCounter(&cur_time);
	if (target >= cur_time.QuadPart && last_wait_amount.QuadPart == wait_amount.QuadPart) {
		while (cur_time.QuadPart < target)
			QueryPerformanceCounter(&cur_time);
	} else {
		printf("Frame limiter fell behind or target FPS has changed. Resyncing...\n");
		last_wait_amount.QuadPart = wait_amount.QuadPart;
		if (d3d9ex_device && !temp_fps_change)
			d3d9ex_device->WaitForVBlank(0);
		QueryPerformanceCounter(&cur_time);
		start_time.QuadPart = cur_time.QuadPart;
		frame_num = 0;
	}
}