#include "World\SpawnPoint.h"

using namespace Ogre;

SpawnPoint::SpawnPoint(const String& spawnID, const Vector3& pos, const Quaternion& rot, const Vector3& scale)
	: m_spawnID(spawnID), m_pos(pos), m_rot(rot), m_scale(scale)
{
}

SpawnPoint::~SpawnPoint()
{
}