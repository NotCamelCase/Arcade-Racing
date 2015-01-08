#pragma once

#include <queue>

#include "World\PhysicsObject.h"

#include "Event\CollisionListener.h"

#define WHEEL_COUNT 4

class GOD;
class Player;
class Waypoint;
class GameObjectManager;
class SoundEffect;
class btRaycastVehicle;
class btVehicleTuning;
struct btVehicleRaycaster;

namespace Ogre { class SceneManager; class SceneNode; class RibbonTrail; class Camera; class Timer; class Vector3; }

enum class VehicleState
{
	INIT, // Initial mode
	INIT_COMPLETE, // Assigned when GameObjectManager completes vehicle setup
	IDLE, // During Count Down or out-of-track moments
	IN_RACE, // If neither of above, vehicle's racing
	END_RACE // When m_ownerPlayer completed the race
};

class Vehicle : public PhysicsObject, public CollisionListener
{
public:
	Vehicle(GameObjectManager* mng, Ogre::SceneNode* node, Ogre::Entity* entity, const Ogre::String& id);
	~Vehicle();

	void update(Ogre::Real delta);

	btVehicleRaycaster* getVehicleRaycaster() const { return m_vehicleRaycaster; }
	btRaycastVehicle* getVehicle() const { return m_btVehicle; }

	void setVehicle(btRaycastVehicle* vehicle) { m_btVehicle = vehicle; }
	void setVehicleRaycaster(btVehicleRaycaster* vc) { m_vehicleRaycaster = vc; }

	void onEvent(int type, void* data = 0);
	void onCollisionStart(CollisionEventData* data);
	void onCollisionTime(CollisionEventData* data);
	void onCollisionEnd(CollisionEventData* data);

	int getCurrentSpeed() const;

	void setEngineForce(Ogre::Real val) { m_engineForce = val; }
	void setMaxEngineForce(Ogre::Real val) { m_maxEngineForce = val; }
	void setSpeedGain(Ogre::Real val) { m_speedGain = val; }

	void setBrakeForce(Ogre::Real val) { m_brakeForce = val; }
	void setBrakeGain(Ogre::Real val) { m_brakeGain = val; }
	void setBrakeReduced(Ogre::Real val) { m_brakeReduced = val; }
	void setBrakeReductionSpeed(Ogre::Real val) { m_brakeReductionSpeed = val; }

	void setSteeringReduced(Ogre::Real v) { m_steeringReduced = v; }
	void setSteeringIncrement(Ogre::Real val) { m_steeringIncrement = val; }
	void setSteeringReductionSpeed(Ogre::Real v) { m_steeringReductionSpeed = v; }
	void setSteeringClamp(Ogre::Real val) { m_steeringClamp = val; }

	Player* getOwnerPlayer() const { return m_ownerPlayer; }
	void setOwnerPlayer(Player* p) { m_ownerPlayer = p; }

	bool operator==(Vehicle* other) { return m_ownerPlayer == other->getOwnerPlayer(); }

	VehicleState getState() const { return m_vehicleState; }
	void setState(VehicleState state) { m_vehicleState = state; }

	std::vector<GameObject*>& getWheels() { return m_wheels; }

	void syncVehicle();
	bool readjustVehicle();

private:
	Ogre::SceneManager* m_sceneMgr;

	Ogre::RibbonTrail* m_trailMO[WHEEL_COUNT];

	GOD* m_props;

	btVehicleRaycaster*	m_vehicleRaycaster;
	btRaycastVehicle* m_btVehicle;

	Ogre::Real m_engineForce;
	Ogre::Real m_maxEngineForce;
	Ogre::Real m_speedGain;

	Ogre::Real m_maxReverse;
	Ogre::Real m_steeringIdeal;
	Ogre::Real m_idealBrakeGain;
	Ogre::Real m_boostLimit;
	Ogre::Real m_boostGain;

	Ogre::Real m_brakeForce;
	Ogre::Real m_brakeGain;
	Ogre::Real m_brakeReduced;
	Ogre::Real m_brakeReductionSpeed;

	Ogre::Real m_steering;
	Ogre::Real m_steeringIncrement;
	Ogre::Real m_steeringClamp;
	Ogre::Real m_steeringReduced;
	Ogre::Real m_steeringReductionSpeed;

	std::vector<GameObject*> m_wheels;

	void updateSounds();
	bool updatePlayerStats();
	void updatePhysics(Ogre::Real delta);

	void setupVisuals();
	void initFineTuningValues();

	void adjustEngineForce(int* currentSpeed);
	void adjustSteering(int* currentSpeed);
	void adjustBrake(int* currentSpeed);

	void applyDynamicAirPressure(int* currentSpeed);
	void handleNeutral(int* currentSpeed);

	Player* m_ownerPlayer;

	Ogre::Timer* m_raceTimer;

	SoundEffect* m_engineSound;
	SoundEffect* m_brakeSound;

	VehicleState m_vehicleState;

	// Location Index -> Waypoint mapping
	std::map<int, Waypoint*> m_trackWaypoints;
	// To prevent illegal CheckPoint passes
	int m_lastPassedWaypointIndex;

	// Store pos of last Waypoint to be able to spawn back when crashed or sth
	Ogre::Vector3* m_latestWaypointPos;
};