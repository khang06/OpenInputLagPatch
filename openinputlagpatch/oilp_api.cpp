// Exported functions to allow compatibility with external tools such as thprac

#include <Windows.h>
#include "config.h"
#include "limiter.h"

bool __stdcall oilp_set_game_fps(int fps) {
	return Limiter::SetGameFPS(fps);
}

bool __stdcall oilp_set_replay_skip_fps(int fps) {
	Config::ReplaySkipFPS = fps;
	return true;
}

bool __stdcall oilp_set_replay_slow_fps(int fps) {
	Config::ReplaySlowFPS = fps;
	return true;
}

int oilp_get_game_fps() {
	return Config::GameFPS;
}


int oilp_get_replay_skip_fps() {
	return Config::ReplaySkipFPS;
}


int oilp_get_replay_slow_fps() {
	return Config::ReplaySlowFPS;
}