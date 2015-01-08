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