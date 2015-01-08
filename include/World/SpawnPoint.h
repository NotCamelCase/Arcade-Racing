#pragma once

#include <OgreString.h>
#include <OgreVector3.h>
#include <OgreQuaternion.h>

class World;

class SpawnPoint
{
public:
	friend class World;

	Ogre::String getSpawnID() const { return m_spawnID; }

	Ogre::Quaternion& getRotation() { return m_rot; }
	Ogre::Vector3& getPosition() { return m_pos; }
	Ogre::Vector3& getScale() { return m_scale; }

private:
	SpawnPoint(const Ogre::String& spawnID, const Ogre::Vector3& pos,
		const Ogre::Quaternion& rot, const Ogre::Vector3& scale);
	~SpawnPoint();

	const Ogre::String m_spawnID;

	Ogre::Quaternion m_rot;
	Ogre::Vector3 m_pos;
	Ogre::Vector3 m_scale;
};