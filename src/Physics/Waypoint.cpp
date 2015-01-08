#include "Physics\Waypoint.h"

#include <OgreStringConverter.h>

#include "Utils.h"
#include "Player\RaceManager.h"

Waypoint::Waypoint(int index)
	: m_locationIndex(index), m_passCount(0), m_passed(false)
{
}

Waypoint::~Waypoint()
{
}

void Waypoint::onPass()
{
	const int maxLapCount = RaceManager::getSingleton().getTrackData()->lapCount;
	m_passCount++;

	m_passed = (m_passCount >= maxLapCount);
}