#pragma once

#include <Windows.h>

extern bool limiter_initialized;
extern LARGE_INTEGER perf_freq;
extern DWORD queued_wait_amount;

void limiter_init();
void limiter_tick();