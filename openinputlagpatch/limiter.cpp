// Basic frame limiter
// Heavily simplified version of vpatch's limiter with no fancy autobltpreparetime

#include <Windows.h>
#include <stdio.h>
#include "d3d9_hook.h"
#include "config.h"
#include "common.h"
#include "d3d9_overlay.h"
#include "limiter.h"

bool Limiter::initialized = false;
LARGE_INTEGER Limiter::start_time;
unsigned int Limiter::frame_num = 0;
LARGE_INTEGER Limiter::wait_amount;
LARGE_INTEGER Limiter::last_wait_amount;
LARGE_INTEGER Limiter::blt_prepare_time;
LARGE_INTEGER Limiter::perf_freq;
ReplayCallback Limiter::replay_callback = nullptr;
LARGE_INTEGER Limiter::frame_start;
LARGE_INTEGER Limiter::frame_end;

// Initializes the limiter's timers, settings, etc
void Limiter::Initialize(ReplayCallback callback) {
	replay_callback = callback;
	QueryPerformanceFrequency(&perf_freq);
	QueryPerformanceCounter(&start_time);

	last_wait_amount.QuadPart = 0;
	frame_start.QuadPart = 0;
	frame_end.QuadPart = 0;

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

// Simple spinwait function
// As precise as possible, but eats up lots of CPU
inline void spin_wait(__int64 target) {
	LARGE_INTEGER cur_time;
	QueryPerformanceCounter(&cur_time);
	while (cur_time.QuadPart < target)
		QueryPerformanceCounter(&cur_time);
}

// Half-spinwait, half-timer wait from vpatch
// Creates a waitable timer until 1 ms before the target, then spins for the rest
// Timer accuracy check not included
bool half_spin_wait_inited = false;
__int64 timer_1ms = 0; // 1 ms relative to the performance counter frequency
double timer_freq_scale = 0; // Used for converting from performance counter -> FILETIME
HANDLE waitable_timer = NULL;
inline void half_spin_wait(__int64 target) {
	if (!half_spin_wait_inited) {
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		timer_1ms = freq.QuadPart / 1000;
		timer_freq_scale = 10000000.0 / (double)freq.QuadPart;
		waitable_timer = CreateWaitableTimer(NULL, TRUE, NULL);
		if (waitable_timer == NULL)
			panic_msgbox(L"Failed to create waitable timer. GLE: 0x%x", GetLastError());
		half_spin_wait_inited = true;
	}

	LARGE_INTEGER cur_time;
	QueryPerformanceCounter(&cur_time);
	auto diff = target - cur_time.QuadPart;
	if (diff >= 0) {
		if (diff >= timer_1ms) {
			// Negative to indicate relative time for SetWaitableTimer
			auto wait_amount = (__int64)((double)(diff - timer_1ms) * -timer_freq_scale);
			if (wait_amount < 0) {
				SetWaitableTimer(waitable_timer, (LARGE_INTEGER*)&wait_amount, 0, NULL, NULL, FALSE);
				WaitForSingleObject(waitable_timer, INFINITE);
			}
		}
		spin_wait(target);
	}
}

// TODO: Implement https://blat-blatnik.github.io/computerBear/making-accurate-sleep-function/

// Performs the actual frame limiting
void Limiter::Tick() {
	if (!initialized)
		panic_msgbox(L"Tried to tick the limiter before initialization.");

	// Calculate how much time it took for the game to process this frame
	__int64 frame_elapsed = 0;
	if (frame_start.QuadPart != 0) {
		LARGE_INTEGER frame_end;
		QueryPerformanceCounter(&frame_end);
		frame_elapsed = frame_end.QuadPart - frame_start.QuadPart;

		if (D3D9Overlay::Instance && frame_num % 30 == 0) {
			D3D9Overlay::Instance->SetText("%.2f/%.2fms", frame_elapsed / (float)perf_freq.QuadPart * 1000.0, (float)Config::BltPrepareTime);
			D3D9Overlay::Instance->text_color = frame_elapsed > blt_prepare_time.QuadPart ? 0xFFFF0000 : 0xFFFFFFFF;
		}
	}

	// Set up the target time before returning
	bool temp_fps_change = UpdateTargetFPS();
	__int64 target = start_time.QuadPart + ++frame_num * wait_amount.QuadPart;
	if (!temp_fps_change) // Don't care about input latency if skipping/slowing down a replay
		target -= blt_prepare_time.QuadPart;

	// Perform the frame limiting
	LARGE_INTEGER cur_time;
	QueryPerformanceCounter(&cur_time);

	// Only resync the timer if a full frame has been skipped
	if (target + wait_amount.QuadPart >= cur_time.QuadPart && last_wait_amount.QuadPart == wait_amount.QuadPart) {
		switch (Config::Sleep) {
			case SleepType::Spin:
				spin_wait(target);
				break;
			case SleepType::Vpatch:
				half_spin_wait(target);
				break;
		}
	} else {
		printf("Frame limiter fell behind or target FPS has changed. Resyncing...\n");
		last_wait_amount.QuadPart = wait_amount.QuadPart;
		if (Config::D3D9Ex && d3d9_device && !temp_fps_change)
			((IDirect3DDevice9Ex*)d3d9_device)->WaitForVBlank(0);
		QueryPerformanceCounter(&cur_time);
		start_time.QuadPart = cur_time.QuadPart;
		frame_num = 0;
	}

	// Record the frame start time
	QueryPerformanceCounter(&frame_start);
}