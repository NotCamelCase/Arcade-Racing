#include "Physics\PhysicsManager.h"

#include <Ogre.h>

#include "Physics\BtOgrePG.h"
#include "Physics\BtOgreGP.h"
#include "Physics\BtOgreExtras.h"
#include "GODLoader.h"

#include "World\GameObject.h"
#include "World\PhysicsObject.h"
#include "World\GameObjectManager.h"
#include "Physics\Vehicle.h"

#include "Event\CollisionListener.h"
#include "Event\EventManager.h"

#include "Game.h"
#include "Utils.h"

using namespace Ogre;
using namespace BtOgre;

#define GRAVITY -9.8

class IsolatedRayResultCallback : public btCollisionWorld::ClosestRayResultCallback
{
public:
	IsolatedRayResultCallback(const btVector3& from, const btVector3& to, btCollisionObject* isolated)
		: btCollisionWorld::ClosestRayResultCallback(from, to), m_isolated(isolated)
	{
	}

	btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
	{
		if (rayResult.m_collisionObject == m_isolated)
		{
			return 1.0f;
		}

		return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
	}

private:
	btCollisionObject* m_isolated;
};

class IsolatedVehicleRaycaster : public btVehicleRaycaster
{
public:
	IsolatedVehicleRaycaster(btDynamicsWorld* world, btCollisionObject* isolated)
		: m_world(world), m_isolated(isolated)
	{
	}

	void* castRay(const btVector3& from, const btVector3& to, btVehicleRaycasterResult& result)
	{
		IsolatedRayResultCallback rayCallback(from, to, m_isolated);
		m_world->rayTest(from, to, rayCallback);
		if (rayCallback.hasHit())
		{
			const btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);
			if (body && body->hasContactResponse())
			{
				result.m_hitPointInWorld = rayCallback.m_hitPointWorld;
				result.m_hitNormalInWorld = rayCallback.m_hitNormalWorld;
				result.m_hitNormalInWorld.normalize();
				result.m_distFraction = rayCallback.m_closestHitFraction;

				return (void*)body;
			}
		}

		return NULL;
	}

private:
	btDynamicsWorld* m_world;
	btCollisionObject* m_isolated;
};

static void internalTick(btDynamicsWorld* world, btScalar timeStep)
{
	PhysicsManager* physics = static_cast<PhysicsManager*> (world->getWorldUserInfo());
	physics->checkCollisions(timeStep);
}

PhysicsManager::PhysicsManager(GameObjectManager* gom, SceneManager* sceneMgr)
	: m_sceneMgr(sceneMgr), m_logger(NULL), m_gameObjManager(gom), m_btWorld(NULL), m_debugDrawer(NULL), m_broadphase(NULL),
	m_collisionConfig(NULL), m_dispatcher(NULL), m_solver(NULL), m_godLoader(NULL), m_debugging(false)
{
	m_logger = Game::getSingleton().getLog();
	m_logger->logMessage("PhysicsManager::PhysicsManager()");

	m_broadphase = new btDbvtBroadphase();
	//m_broadphase = new btAxisSweep3(btVector3(-1000, -1000, -1000), btVector3(1000, 1000, 1000));
	m_collisionConfig = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfig);
	m_solver = new btSequentialImpulseConstraintSolver();

	m_btWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfig);
	m_btWorld->setGravity(btVector3(0, GRAVITY, 0));

#ifdef _DEBUG
	m_debugDrawer = new BtOgre::DebugDrawer(m_sceneMgr->getRootSceneNode(), m_btWorld);
	m_btWorld->setDebugDrawer(m_debugDrawer);
#endif

	m_btWorld->setInternalTickCallback(internalTick, static_cast<void*> (this), false);

	m_godLoader = &GODLoader::getSingleton();

	m_logger->logMessage("PhysicsManager initialized");
}

PhysicsManager::~PhysicsManager()
{
	for (int i = 0; i < m_btWorld->getNumCollisionObjects(); i++)
	{
		btCollisionObject* obj = m_btWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		m_btWorld->removeRigidBody(body);
		SAFE_DELETE(body);
	}

	SAFE_DELETE(m_debugDrawer);
	SAFE_DELETE(m_btWorld);

	SAFE_DELETE(m_solver);
	SAFE_DELETE(m_dispatcher);
	SAFE_DELETE(m_collisionConfig);
	SAFE_DELETE(m_broadphase);

	m_logger->logMessage("Physics world destroyed");
}

void PhysicsManager::update(Real delta)
{
	m_btWorld->stepSimulation(delta, 8); // TODO: simulation step
#ifdef _DEBUG
	m_debugDrawer->setDebugMode(m_debugging);
	m_btWorld->debugDrawWorld();
	m_debugDrawer->step();
#endif
}

// Physics simulation step : processed before simulation done and after update(delta) call
void PhysicsManager::checkCollisions(Real timeStep)
{
	std::map<std::pair<PhysicsObject*, PhysicsObject*>, CollisionInfo> currentCollisions;

	const int numManifolds = m_btWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = m_btWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* objA = const_cast<btCollisionObject*> (contactManifold->getBody0());
		btCollisionObject* objB = const_cast<btCollisionObject*> (contactManifold->getBody1());

		// Add currently colliding objects to list
		const int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& cp = contactManifold->getContactPoint(j);
			if (cp.getDistance() < 0.f)
			{
				PhysicsObject* pof = static_cast<PhysicsObject*> (objA->getUserPointer());
				PhysicsObject* pos = static_cast<PhysicsObject*> (objB->getUserPointer());
				std::pair<PhysicsObject*, PhysicsObject*> collisionPair(pof, pos);
				CollisionInfo colInfo(cp);
				currentCollisions.insert(std::make_pair(collisionPair, colInfo));
			}
		}
	}

	if (!currentCollisions.empty())
	{
		for (const auto& col : currentCollisions)
		{
			if (m_collidingObjects.find(col.first) == m_collidingObjects.end())
			{
				m_collidingObjects.insert(col);
			}
		}
	}

	if (!m_collidingObjects.empty())
	{
		for (auto& col : m_collidingObjects)
		{
			if (currentCollisions.find(col.first) == currentCollisions.end())
			{
				col.second.type = CollisionType::ENDED;
			}
		}
	}

	auto it = m_collidingObjects.begin();
	for (; it != m_collidingObjects.end();)
	{
		PhysicsObject* pof = it->first.first;
		PhysicsObject* pos = it->first.second;
		CollisionInfo& status = it->second;
		const int type = status.type;
		CollisionEventData* data = new CollisionEventData(status.contactPoint);
		data->collider = pof;
		data->collidingWith = pos;

		if (type == CollisionType::ENDED)
		{
			EventManager::getSingleton().notify(type, data);
			it = m_collidingObjects.erase(it++);
		}
		else {
			if (type == CollisionType::COLLIDING)
			{
				EventManager::getSingleton().notify(type, data);
			}

			else if (type == CollisionType::STARTED)
			{
				EventManager::getSingleton().notify(type, data);
				status.type = CollisionType::COLLIDING;
			}

			SAFE_DELETE(data);
			++it;
		}
	}
}

void PhysicsManager::destroy(PhysicsObject* po)
{
	btRigidBody* body = po->getRigidBody();
	m_btWorld->removeRigidBody(body);
	SAFE_DELETE(body);
	m_logger->logMessage(po->getID() + " removed from physics world");
}

void PhysicsManager::destroyVehicle(Vehicle* vehicle)
{
	m_btWorld->removeVehicle(vehicle->getVehicle());
	m_btWorld->removeRigidBody(vehicle->getRigidBody());
	m_logger->logMessage("Vehicle " + vehicle->getID() + " removed from physics world");
}

void PhysicsManager::setUpPhysicsObject(PhysicsObject* obj)
{
	switch (obj->getType())
	{
	case GameObjectType::TYPE_COMBAT_VEHICLE:
		setUpVehicle(static_cast<Vehicle*> (obj));
		break;
	default:
		setUpMainProperties(obj);
		break;
	}
}

void PhysicsManager::setUpVehicle(Vehicle* vehicleObj)
{
	auto& params = m_godLoader->getGameObjectDefinitionByTag(vehicleObj->getTag())->getParameters();
	const Real mass = StringConverter::parseReal(params["mass"]);
	const bool dynamic = mass == 0 ? false : true;
	const bool animated = StringConverter::parseBool(params["animated"]);
	const String collisionShape = params["collision-shape"];
	const Real suspensionRestLength = StringConverter::parseReal(params["suspensionRestLength"]);
	vehicleObj->setDynamic(dynamic);

	vehicleObj->setSteeringClamp(StringConverter::parseReal(params["steeringClamp"]));
	vehicleObj->setSteeringIncrement(StringConverter::parseReal(params["steeringIncrementIdeal"]));
	vehicleObj->setSteeringReduced(StringConverter::parseReal(params["steeringIncrementReduced"]));
	vehicleObj->setSteeringReductionSpeed(StringConverter::parseReal(params["steeringReductionSpeed"]));

	vehicleObj->setMaxEngineForce(StringConverter::parseReal(params["maxEngineForce"]));
	vehicleObj->setEngineForce(StringConverter::parseReal(params["engineForce"]));
	vehicleObj->setSpeedGain(StringConverter::parseReal(params["speedGainIdeal"]));

	vehicleObj->setBrakeForce(StringConverter::parseReal(params["brakeForce"]));
	vehicleObj->setBrakeGain(StringConverter::parseReal(params["brakeGainIdeal"]));
	vehicleObj->setBrakeReductionSpeed(StringConverter::parseReal(params["brakeReductionSpeed"]));
	vehicleObj->setBrakeReduced(StringConverter::parseReal(params["brakeGainReduced"]));

	btCollisionShape* chassisShape = createCollisionShape(vehicleObj->getEntity(), collisionShape, animated);
	RigidBodyState* state = new RigidBodyState(vehicleObj->getNode());
	btVector3 inertia;
	btCompoundShape* compound = new btCompoundShape();
	btTransform localTrans;
	localTrans.setIdentity();
	//const Ogre::Vector3& center = vehicleObj->getEntity()->getBoundingBox().getCenter();
	//localTrans.setOrigin(BtOgre::Convert::toBullet(center));
	//localTrans.setOrigin(btVector3(0, .5, 0));
	compound->addChildShape(localTrans, chassisShape);
	compound->calculateLocalInertia(mass, inertia);
	btRigidBody* chassis = new btRigidBody(mass, state, compound, inertia);
	chassis->setUserPointer(vehicleObj);
	m_btWorld->addRigidBody(chassis);
	chassis->setActivationState(DISABLE_DEACTIVATION);

	btVehicleRaycaster* vehicleRaycaster = new IsolatedVehicleRaycaster(m_btWorld, chassis);
	//btVehicleRaycaster* vehicleRaycaster = new btDefaultVehicleRaycaster(m_btWorld);
	btRaycastVehicle::btVehicleTuning tuning;
	btRaycastVehicle* vehicle = new btRaycastVehicle(tuning, chassis, vehicleRaycaster);
	vehicle->setCoordinateSystem(0, 1, 2);

	btVector3 wheelDirectionCS0(0, -1, 0);
	btVector3 wheelAxleCS(-1, 0, 0);

	const StringVector wheelIDs = StringUtil::split(params["wheels"], " ");
	for (const auto& id : wheelIDs)
	{
		GameObject* go = m_gameObjManager->getGameObjectByID(id);
		if (go == NULL)
		{
			go = m_gameObjManager->createManually("wheel", id);
		}

		auto defs = m_godLoader->getGameObjectDefinitionByTag(go->getID())->getParameters();
		bool isFront = StringConverter::parseBool(defs["isFront"]);
		const btVector3 cp = Convert::toBullet(StringConverter::parseVector3(defs["connectionPoint"]));
		const Real radius = StringConverter::parseReal(defs["radius"]); // radius = 0.48
		vehicle->addWheel(cp, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, radius, tuning, isFront);
		vehicleObj->getWheels().push_back(go);
	}

	m_btWorld->addVehicle(vehicle);

	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		btWheelInfo& wheel = vehicle->getWheelInfo(i);
		wheel.m_suspensionStiffness = StringConverter::parseReal(params["suspensionStiffness"]);
		wheel.m_wheelsDampingRelaxation = StringConverter::parseReal(params["suspensionDamping"]);
		wheel.m_wheelsDampingCompression = StringConverter::parseReal(params["suspensionCompression"]);
		wheel.m_frictionSlip = StringConverter::parseReal(params["wheelFriction"]);
		wheel.m_rollInfluence = StringConverter::parseReal(params["rollInfluence"]);
		wheel.m_engineForce = StringConverter::parseReal(params["engineForce"]);
		wheel.m_skidInfo = StringConverter::parseReal(params["skidInfo"]);
	}

	vehicleObj->setRigidBody(chassis);
	vehicleObj->setMotionState(state);
	vehicleObj->setCollisionShape(compound);
	vehicleObj->setVehicle(vehicle);
	vehicleObj->setVehicleRaycaster(vehicleRaycaster);

	vehicle->resetSuspension();

	vehicleObj->syncVehicle();

	m_logger->logMessage("Vehicle " + vehicleObj->getID() + " added");
}

void PhysicsManager::setUpMainProperties(PhysicsObject* object)
{
	m_logger->logMessage("Setting up physics " + object->getID());

	auto& params = m_godLoader->getGameObjectDefinitionByTag(object->getTag())->getParameters();
	const Real mass = StringConverter::parseReal(params["mass"]);
	const bool dynamic = mass == 0 ? false : true;
	const bool animated = StringConverter::parseBool(params["animated"]);

	object->setDynamic(dynamic);

	const String collisionShape = params["collision-shape"];
	btVector3 inertia(0, 0, 0);
	btRigidBody* body = NULL;
	btCollisionShape* colShape = NULL;
	RigidBodyState* motionState = NULL;

	colShape = createCollisionShape(object->getEntity(), collisionShape, animated);
	motionState = new RigidBodyState(object->getNode());

	if (dynamic)
	{
		colShape->calculateLocalInertia(mass, inertia);
	}

	body = new btRigidBody(mass, motionState, colShape, inertia);
	body->setUserPointer(object);

	const String crv = params["collisionResponse"];
	if (crv != "")
	{
		bool isResponsive = StringConverter::parseBool(crv);
		if (!isResponsive)
		{
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		}
	}

	object->setMotionState(motionState);
	object->setCollisionShape(colShape);
	object->setRigidBody(body);
	m_btWorld->addRigidBody(body);//, collisionFlag, colGroup);
}

btCollisionShape* PhysicsManager::createCollisionShape(Entity* entity, const Ogre::String& collisionShape, bool animated)
{
	VertexIndexToShape* converter = NULL;
	if (animated)
		converter = new AnimatedMeshToShapeConverter(entity);
	else
		converter = new StaticMeshToShapeConverter(entity);

	btCollisionShape* colShape = NULL;
	if (collisionShape == "BOX")
		colShape = converter->createBox();
	else if (collisionShape == "SPHERE")
		colShape = converter->createSphere();
	else if (collisionShape == "CAPSULE")
		colShape = converter->createCapsule();
	else if (collisionShape == "MESH")
		colShape = converter->createTrimesh();
	else if (collisionShape == "CYLINDER")
		colShape = converter->createCylinder();
	else if (collisionShape == "CONVEX")
		colShape = converter->createConvex();

	SAFE_DELETE(converter);

	return colShape;
}
