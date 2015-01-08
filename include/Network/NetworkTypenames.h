#pragma once

#include "Common-cpp\inc\Common.h"

struct NetworkTypenames
{
	// Event type names
	static const nByte EV_GAME_CLIENT_STATE_CHANGE = 101;
	static const nByte EV_GAME_CLIENT_INPUT_UPDATE = 102;
	static const nByte EV_GAME_CLIENT_POST_LOADING = 103;
	
	// Parameter names
	static const nByte PARAMS_GAME_CLIENT_STATE = 1;
	static const nByte PARAMS_CLIENT_KEY_FLAGS = 2;
};