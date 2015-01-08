#include "Physics\Vehicle.h"

#include <Ogre.h>

#include <btBulletDynamicsCommon.h>

#include "Physics\BtOgrePG.h"
#include "Physics\BtOgreGP.h"
#include "Physics\BtOgreExtras.h"

#include "World\GameObjectManager.h"
#include "Physics\PhysicsManager.h"
#include "Event\EventManager.h"
#include "Audio\AudioManager.h"
#include "Audio\SoundEffect.h"

#include "Network\NetworkManager.h"
#include "Player\PlayerManager.h"
#include "Player\RaceManager.h"
#include "Network\GameClient.h"
#include "Physics\Waypoint.h"
#include "Player\Player.h"
#include "GODLoader.h"
#include "InputMap.h"
#include "Utils.h"

#define FOLLOW_CAMERA_OFFSET Vector3(0, -12.5, 25)
#define LIGHT_OFFSET Vector3(0, -20, 20)

#define AIR_DENSITY 1.2f

#define CHASE_CAM_AUTO 1
#define CHASE_CAM_LIMITED 0

using namespace Ogre;

Vehicle::Vehicle(GameObjectManager* mng, SceneNode* node, Entity* entity, const String& id)
	: PhysicsObject(mng, node, entity, id),
	m_vehicleRaycaster(NULL), m_btVehicle(NULL), m_ownerPlayer(NULL),
	m_engineForce(0), m_brakeForce(0), m_steering(0), m_sceneMgr(NULL),
	m_engineSound(NULL), m_brakeSound(NULL), m_props(NULL), m_raceTimer(NULL),
	m_vehicleState(VehicleState::INIT), m_lastPassedWaypointIndex(-1), m_latestWaypointPos(NULL)
{
	m_sceneMgr = m_manager->getSceneManager();

	m_props = GODLoader::getSingleton().getGameObjectDefinitionByTag(m_tag);
	const String engineSound = m_props->getParameters()["engineSound"];
	m_engineSound = AudioManager::getSingleton().createSoundEffect(engineSound, AudioType::SOUND_2D_LOOPING);
	assert(m_engineSound != NULL && "Vehicle engine sound is NULL!");
	const String brakeSound = m_props->getParameters()["brakeSound"];
	m_brakeSound = AudioManager::getSingleton().createSoundEffect(brakeSound);
	assert(m_engineSound != NULL && "Vehicle brake sound is NULL!");
	updateSounds();

	// Get location indexes of Waypoints from GameObjectManager's CheckPoints
	for (const GameObject* cp : m_manager->getGameObjects())
	{
		if (cp->getType() == GameObjectType::TYPE_CHECKPOINT)
		{
			const int wpIndex = StringConverter::parseInt(cp->getID());
			Waypoint* waypoint = new Waypoint(wpIndex);
			m_trackWaypoints.insert(std::make_pair(wpIndex, waypoint));
		}
	}

	EventManager::getSingleton().registerListener(EventType::COLLISION_STARTED, this);
	EventManager::getSingleton().registerListener(EventType::COLLISION_TIME, this);
	EventManager::getSingleton().registerListener(EventType::COLLISION_ENDED, this);
	EventManager::getSingleton().registerListener(EventType::START_OFF_FIRE, this);
	EventManager::getSingleton().registerListener(EventType::VEHICLE_UPSIDE_DOWN, this);

	m_raceTimer = new Timer();

	m_logger->logMessage("Vehicle created");
}

Vehicle::~Vehicle()
{
	AudioManager::getSingleton().release(m_engineSound);
	AudioManager::getSingleton().release(m_brakeSound);

	SAFE_DELETE(m_btVehicle);
	SAFE_DELETE(m_vehicleRaycaster);

	SAFE_DELETE(m_raceTimer);

	for (auto& it : m_trackWaypoints)
	{
#if _DEBUG
		Waypoint* wp = NULL;
		wp = it.second;
		if (wp != NULL)
			m_logger->logMessage("Destroying Waypoint of index: " + wp->getLocationIndex());
#endif
		SAFE_DELETE(it.second);
	}
	m_trackWaypoints.clear();

	for (int i = 0; i < WHEEL_COUNT; i++)
	{
		m_sceneMgr->destroyRibbonTrail(m_trailMO[i]);
	}
	m_logger->logMessage("Tire trails destroyed");

	EventManager::getSingleton().unregisterListener(EventType::COLLISION_STARTED, this);
	EventManager::getSingleton().unregisterListener(EventType::COLLISION_TIME, this);
	EventManager::getSingleton().unregisterListener(EventType::COLLISION_ENDED, this);
	EventManager::getSingleton().unregisterListener(EventType::START_OFF_FIRE, this);
	EventManager::getSingleton().unregisterListener(EventType::VEHICLE_UPSIDE_DOWN, this);

	m_logger->logMessage("Vehicle destroyed");
}

int Vehicle::getCurrentSpeed() const
{
	return m_btVehicle->getCurrentSpeedKmHour();
}

void Vehicle::applyDynamicAirPressure(int* currentSpeed)
{
	float v = *currentSpeed * (1.0f / 3.6f);
	float q = 0.5f * AIR_DENSITY * v * v;
	m_rigidBody->applyCentralForce(btVector3(0, -4.5 * q, 0));
}

void Vehicle::initFineTuningValues()
{
	m_maxReverse = -1 * StringConverter::parseInt(m_props->getParameters()["maxReverseSpeed"]);
	m_steeringIdeal = StringConverter::parseReal(m_props->getParameters()["steeringIncrementIdeal"]);
	m_idealBrakeGain = StringConverter::parseReal(m_props->getParameters()["brakeGainIdeal"]);
	m_boostLimit = StringConverter::parseInt(m_props->getParameters()["startUpBoostLimit"]);
	m_boostGain = StringConverter::parseInt(m_props->getParameters()["startUpBoost"]);

	m_logger->logMessage("Assigned initial Vehicle physics paramaters");
}

void Vehicle::update(Real delta)
{
	switch (m_vehicleState)
	{
	case VehicleState::INIT_COMPLETE:
		initFineTuningValues();
		setupVisuals();
		m_vehicleState = VehicleState::IDLE;
		break;
	case VehicleState::IDLE:
		if (m_rigidBody->getActivationState() != DISABLE_SIMULATION)
		{
			m_rigidBody->forceActivationState(DISABLE_SIMULATION);
			m_logger->logMessage("Vehicle chassis simulation disabled");
		}
		updatePhysics(delta);
		break;
	case VehicleState::IN_RACE:
		updatePhysics(delta);
		break;
	case VehicleState::END_RACE:
		break;
	default:
		break;
	}

	syncVehicle();

	updateSounds();
	updatePlayerStats();
}

void Vehicle::updatePhysics(Real delta)
{
	int currentSpeed = getCurrentSpeed();
	applyDynamicAirPressure(&currentSpeed);

	bool tireTrailVis = false;

	unsigned int keyFlag = NULL;
	if (!m_ownerPlayer->isLocal())
	{
		GameClient* remoteClient = NetworkManager::getSingleton().getGameClientByPlayer(m_ownerPlayer);
		assert(remoteClient && "Error finding remote GameClient by Player");
		keyFlag = remoteClient->getKeyFlags();
		m_logger->logMessage("Checking input with client data");
	}

	if (InputMap::isKeyActive(InputType::BRAKE, keyFlag))
	{
		if (currentSpeed <= 0.f)
		{
			if (currentSpeed > m_maxReverse)
			{
				m_brakeForce = 0.f;
				m_engineForce += -m_speedGain;
			}
			else {
				m_brakeForce = 0.f;
				handleNeutral(&currentSpeed);
			}
		}
		else {
			tireTrailVis = true;

			adjustBrake(&currentSpeed);

			m_brakeForce += m_brakeGain;
			m_engineForce = 0.f;
		}
	}
	else if (InputMap::isKeyActive(InputType::ACCELERATE, keyFlag))
	{
		adjustEngineForce(&currentSpeed);
		m_engineForce += m_speedGain;
		if (m_engineForce > m_maxEngineForce)
			m_engineForce = m_maxEngineForce;
		m_brakeForce = 0.f;
	}
	else
	{
		handleNeutral(&currentSpeed);
	}

	if (InputMap::isKeyActive(InputType::STEER_LEFT, keyFlag))
	{
		adjustSteering(&currentSpeed);

		m_steering += m_steeringIncrement;
		if (m_steering > m_steeringClamp)
			m_steering = m_steeringClamp;
	}
	else if (InputMap::isKeyActive(InputType::STEER_RIGHT, keyFlag))
	{
		adjustSteering(&currentSpeed);

		m_steering -= m_steeringIncrement;
		if (m_steering < -m_steeringClamp)
			m_steering = -m_steeringClamp;
	}
	else
	{
		// TODO: Further investigate steering relief reduction !!!
		static Real neutralSteering = 0.02;
		if (Math::Abs(m_steering) < neutralSteering)
			m_steering = 0.0f;
		else if (m_steering > 0.0f)
			m_steering -= neutralSteering;
		else
			m_steering += neutralSteering;
	}

	// DOUBT: Should unroll the loop ?!
	for (int i = 0; i < m_btVehicle->getNumWheels(); i++)
	{
		if (i < 2)
		{
			m_btVehicle->setSteeringValue(m_steering, i);
		}
		else
		{
			m_btVehicle->applyEngineForce(m_engineForce, i);
			m_btVehicle->setBrake(m_brakeForce, i);
		}

		RibbonTrail* trail = (RibbonTrail*) m_trailMO[i];
		assert(trail && "Error indexing RibbonTrail");
		if (!tireTrailVis && trail)
		{
			trail->clearAllChains();
		}
		trail->setVisible(tireTrailVis);
	}
}

bool Vehicle::updatePlayerStats()
{
	PlayerStats* stats = m_ownerPlayer->getPlayerStats();
	stats->vehicleSpeed = getCurrentSpeed();

	const unsigned long currentTiming = m_raceTimer->getMilliseconds();
	const unsigned long sumSec = currentTiming / 1000;

	stats->min = sumSec / 60;
	stats->sec = sumSec % 60;
	stats->splitSec = (currentTiming - (sumSec * 1000)) / 10;

	return true;
}

void Vehicle::syncVehicle()
{
	for (int i = 0; i < m_wheels.size(); i++)
	{
		m_btVehicle->updateWheelTransform(i, true);

		const btTransform& w = m_btVehicle->getWheelInfo(i).m_worldTransform;
		SceneNode* node = m_wheels[i]->getNode();
		node->setPosition(w.getOrigin()[0], w.getOrigin()[1], w.getOrigin()[2]);
		node->setOrientation(w.getRotation().getW(), w.getRotation().getX(), w.getRotation().getY(), w.getRotation().getZ());
	}
}

void Vehicle::adjustSteering(int* currentSpeed)
{
	static const int fd = 1000;
	static const int bd = 500;

	if (*currentSpeed >= m_steeringReductionSpeed)
	{
		const float step = (m_steeringIdeal - m_steeringReduced) / fd;
		const float asi = std::max(m_steeringReduced + step, (m_steeringIncrement - step));

		m_steeringIncrement = asi;
	}
	else
	{
		if (*currentSpeed <= (m_steeringReductionSpeed / 2)) m_steeringIncrement = m_steeringIdeal;
		else {
			const float step = (m_steeringIdeal - m_steeringReduced) / bd;
			const float asi = std::min(m_steeringIdeal, (m_steeringIncrement + step));

			m_steeringIncrement = asi;
		}
	}
}

void Vehicle::adjustBrake(int* currentSpeed)
{
	static const int fd = 100;
	static const int bd = 75;

	if (*currentSpeed >= m_brakeReductionSpeed)
	{
		const float step = (m_idealBrakeGain - m_brakeReduced) / fd;
		const float abf = std::max(m_brakeReduced + step, (m_brakeGain - step));

		m_brakeGain = abf;
	}
	else {
		const float step = (m_idealBrakeGain - m_steeringReduced) / bd;
		const float abf = std::min(m_idealBrakeGain, (m_brakeGain + step));

		m_brakeGain = abf;
	}
}

void Vehicle::adjustEngineForce(int* currentSpeed)
{
	if (*currentSpeed < m_boostLimit)
	{
		m_speedGain += m_boostGain;
	}
}

void Vehicle::updateSounds()
{
	if (m_btVehicle && m_vehicleState != VehicleState::END_RACE)
	{
		const int cv = getCurrentSpeed();

		if (InputMap::isKeyActive(InputType::BRAKE))
		{
			if (!m_brakeSound->isPlaying() && cv >= 30.f) m_brakeSound->play();
		}
		else {
			m_brakeSound->stop();
		}

		if (!m_engineSound->isPlaying()) m_engineSound->play(); m_engineSound->setVolume(.6);

		const float factor = cv / 100.f;
		m_engineSound->setPitch(std::max(0.2f, std::min(factor, 2.f)));
	}
}

void Vehicle::onEvent(int type, void* data)
{
	CollisionListener::onEvent(type, data);

	switch (type)
	{
	case EventType::VEHICLE_UPSIDE_DOWN:
		if (m_ownerPlayer->isLocal())
			readjustVehicle();
		break;
	case EventType::START_OFF_FIRE:
		if (m_raceTimer == NULL)
			m_raceTimer = new Timer();
		else
			m_raceTimer->reset();
		m_rigidBody->forceActivationState(DISABLE_DEACTIVATION);
		m_logger->logMessage("Vehicle chassis simulation enabled");
		m_vehicleState = VehicleState::IN_RACE;
		m_ownerPlayer->setPlayerState(PlayerState::RACING);
		break;
	default:
		break;
	}
}

void Vehicle::onCollisionStart(CollisionEventData* colData)
{
	PhysicsObject* other = getOtherOffCollision(colData);
	switch (other->getType())
	{
	case GameObjectType::TYPE_CHECKPOINT:
		if (m_vehicleState != VehicleState::IN_RACE) // Skip CountDown CheckPoint collisions
		{
			break;
		}

		const int wpIndex = StringConverter::parseInt(other->getID());
		if (m_lastPassedWaypointIndex < 0)
		{
			if (wpIndex != 2)
				break;
		}
		else if (m_lastPassedWaypointIndex == wpIndex)
		{
			break;
		}

		// Store last checkpoint's position so when crashed, vehicle will be moved onto it
		m_latestWaypointPos = const_cast<Vector3*> (&other->getNode()->_getDerivedPosition());

		Waypoint* passedWaypoint = NULL;
		auto finder = m_trackWaypoints.find(wpIndex);
		if (finder == m_trackWaypoints.end())
		{
			assert(passedWaypoint != NULL && "Indexed Waypoint couldn't be fetched!");
			
			break;
		}
		passedWaypoint = finder->second;
		passedWaypoint->onPass();
		m_logger->logMessage("Waypoint " + StringConverter::toString(wpIndex) + " visited");
		if (passedWaypoint->isPassed())
		{
			m_trackWaypoints.erase(wpIndex);
			m_manager->destroy(other);
		}

		PlayerStats* stats = m_ownerPlayer->getPlayerStats();
		if (m_trackWaypoints.empty()) // Player completed track
		{
			m_engineSound->stop();
			m_brakeSound->stop();
			m_logger->logMessage("Vehicle Sound FX stopped");

			SoundEffect* celebration = AudioManager::getSingleton().createSoundEffect("finish_line.aiff");
			celebration->play();

			stats->currentLap++;
			assert(stats->currentLap <= MAX_LAP_COUNT);
			const unsigned long overallTime = m_raceTimer->getMilliseconds();
			// Check to prevent 1-lapped track ending
			if (stats->currentLap == 1)
			{
				stats->lapTimes[0] = overallTime;
			}
			else
			{
				// If the track is not 1-lapped, previous lap timing is current - sum of all previous laps' timing
				stats->lapTimes[stats->currentLap - 1] = overallTime - RaceManager::getSingleton().computePrevLapsSum(m_ownerPlayer);
			}

			m_vehicleState = VehicleState::END_RACE;
			m_manager->getPhysicsManager()->destroyVehicle(this);
			EventManager::getSingleton().notify(EventType::TRACK_COMPLETE, m_ownerPlayer);
		}
		else
		{
			if (wpIndex == 1) // Passing first waypoint whose location index is 1 means one lap's completed.
			{
				if (m_lastPassedWaypointIndex > 0)
				{
					stats->currentLap++;
					assert(stats->currentLap <= MAX_LAP_COUNT && "Lap count exceeds PlayerStats!");
					const unsigned long overallTime = m_raceTimer->getMilliseconds();
					if (stats->currentLap == 1)
					{
						stats->lapTimes[0] = overallTime;
					}
					else
					{
						stats->lapTimes[stats->currentLap - 1] = overallTime - RaceManager::getSingleton().computePrevLapsSum(m_ownerPlayer);
					}
					EventManager::getSingleton().notify(EventType::LAP_COMPLETE, m_ownerPlayer);
				}
			}
		}

		m_lastPassedWaypointIndex = wpIndex;

		break;
	}
}

void Vehicle::onCollisionTime(CollisionEventData* colData)
{
	PhysicsObject* other = getOtherOffCollision(colData);
}

void Vehicle::onCollisionEnd(CollisionEventData* data)
{
	PhysicsObject* other = getOtherOffCollision(data);
}

void Vehicle::handleNeutral(int* currentSpeed)
{
	// TODO: Handle neutral vehicle !!!
	m_engineForce = 0.f;
	if (*currentSpeed != 0)
	{
		if (*currentSpeed >= 100)
			m_brakeForce += (m_brakeGain * .005);
		else
			m_brakeForce += (m_brakeGain * .0025);
	}
}

bool Vehicle::readjustVehicle()
{
	static const btVector3& zero = btVector3(0, 0, 0);
	m_rigidBody->forceActivationState(DISABLE_SIMULATION);
	m_logger->logMessage("Vehicle chassis disabled !");

	if (m_vehicleState != VehicleState::IN_RACE) return false;

	m_rigidBody->setLinearVelocity(zero);
	m_rigidBody->setAngularVelocity(zero);

	btTransform currentTransform = m_btVehicle->getChassisWorldTransform();
	btVector3& pos = currentTransform.getOrigin();
	btQuaternion rot = currentTransform.getRotation();
	pos.setY(pos.y() + 2.5);
	float angle = 0.f;
	if (pos.x() * pos.x() + pos.z() * pos.z() > 0)
	{
		angle += Math::ATan2(-pos.x(), -pos.z()).valueDegrees();
	}
	rot.setRotation(btVector3(0, 1, 0), angle);
	currentTransform.setRotation(rot);

	if (m_lastPassedWaypointIndex < 0 || m_latestWaypointPos == NULL)
	{
		const Vector3& pos = m_manager->getGameObjectByID(StringConverter::toString(m_trackWaypoints.find(1)->second->getLocationIndex()))->getNode()->_getDerivedPosition();
		m_latestWaypointPos = const_cast<Vector3*> (&pos);
	}

	currentTransform.setOrigin(BtOgre::Convert::toBullet(*m_latestWaypointPos));
	m_rigidBody->setWorldTransform(currentTransform);

	m_rigidBody->forceActivationState(DISABLE_DEACTIVATION);
	m_logger->logMessage("Vehicle chassis enabled!");

	m_logger->logMessage("Vehicle readjusted!");

	return false;
}

void Vehicle::setupVisuals()
{
	for (size_t i = 0; i < WHEEL_COUNT; i++)
	{
		NameValuePairList params;
		params["numberOfChains"] = "1";
		params["maxElements"] = "100";
		RibbonTrail* trail = (RibbonTrail*) m_sceneMgr->createMovableObject("RibbonTrail", &params);
		m_sceneMgr->getRootSceneNode()->attachObject(trail);
		trail->setMaterialName("TireTrail");
		trail->setTrailLength(100);
		trail->setInitialColour(0, 0.1, 0.1, 0.1);
		//trail->setInitialColour(0, 0.1, 0.1, 0.1, 0);
		trail->setColourChange(0, 0.08, 0.08, 0.08, 0.08);
		trail->setFaceCamera(false, Vector3::UNIT_Y);
		//trail->setUseTextureCoords(true);
		trail->setCastShadows(false);
		trail->setUseVertexColours(true);
		trail->setInitialWidth(0, 1);
		trail->setRenderQueueGroup(m_entity->getRenderQueueGroup() + 1);
		trail->addNode(m_wheels[i]->getNode());

		m_trailMO[i] = trail;
	}

	m_logger->logMessage("Vehicle visuals set up");
}