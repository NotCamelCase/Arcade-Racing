/*
The MIT License (MIT)

Copyright (c) 2015 Tayfun Kayhan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

enum EventType
{
	// Physics collisions
	COLLISION_STARTED,
	COLLISION_TIME,
	COLLISION_ENDED,

	// Triggered when CountdownEvent is finished
	START_OFF_FIRE,
	
	// Race events
	LAP_COMPLETE,
	TRACK_COMPLETE,

	// Vehicle physics events
	VEHICLE_UPSIDE_DOWN,
	PLAYER_VEHICLE_READY,

	// Network events
	NET_CONNECTION_FAILED,
	NET_READY_IN_LOBBY,
	NET_WAITING_IN_ROOM,
	NET_READY_TO_JOIN_GAME
};

class IEventListener
{
public:
	IEventListener() {}
	virtual ~IEventListener() {}

	virtual void onEvent(int eventType, void* data = 0) = 0;
};