// Basic frame limiter
// Heavily simplified version of vpatch's limiter with no timers or fancy autobltpreparetime
// TODO: IDirect3DDevice9Ex::WaitForVBlank looks useful

#include <Windows.h>
#include <stdio.h>

LARGE_INTEGER limiter_start;
unsigned int frame_num;
LARGE_INTEGER wait_amount;
LARGE_INTEGER blt_prepare_time;

void limiter_init() {
	LARGE_INTEGER perf_freq;
	QueryPerformanceFrequency(&perf_freq);
	QueryPerformanceCounter(&limiter_start);
	wait_amount.QuadPart = (LONGLONG)((double)perf_freq.QuadPart / 60.0);
	blt_prepare_time.QuadPart = perf_freq.QuadPart / 1000 * 2; // 2 ms
}

void limiter_tick() {
	__int64 target = limiter_start.QuadPart + ++frame_num * wait_amount.QuadPart;
	LARGE_INTEGER cur_time;
	QueryPerformanceCounter(&cur_time);
	if (target - blt_prepare_time.QuadPart >= cur_time.QuadPart) {
		while (cur_time.QuadPart < target)
			QueryPerformanceCounter(&cur_time);
	} else {
		printf("Frame limiter fell behind!!! Resyncing...\n");
		limiter_start.QuadPart = cur_time.QuadPart;
		frame_num = 0;
	}
}