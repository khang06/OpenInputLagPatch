// Basic frame limiter
// Heavily simplified version of vpatch's limiter with no timers or fancy autobltpreparetime

#include <Windows.h>
#include <stdio.h>
#include "d3d9_hook.h"

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
	wait_amount.QuadPart = (LONGLONG)((double)perf_freq.QuadPart / 60.0);
	queued_wait_amount = wait_amount.LowPart;
	blt_prepare_time.QuadPart = perf_freq.QuadPart / 1000 * 2; // 2 ms
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