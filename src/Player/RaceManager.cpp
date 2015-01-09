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

#include "Player\RaceManager.h"

#include <OgreLog.h>
#include <OgreStringConverter.h>

#include "Player\Player.h"
#include "Player\PlayerManager.h"

#include "NBID.h"
#include "Game.h"
#include "GODLoader.h"

TrackData::TrackData(const Ogre::String& obj)
	: lapCount(0), objName(obj), niceName("")
{
	// Fill in any to-be-used necessary data once for current track
	auto& defs = GODLoader::getSingleton().getGameObjectDefinitionByTag(obj)->getParameters();
	lapCount = Ogre::StringConverter::parseInt(defs["lap_number"]);
	niceName = defs["nice_name"];
}

TrackData::~TrackData()
{
}

RaceManager::RaceManager()
	: m_trackData(NULL)
{
}

RaceManager::~RaceManager()
{
	for (auto& stats : m_stats)
	{
		SAFE_DELETE(stats.second);
	}
	m_stats.clear();

	SAFE_DELETE(m_trackData);

	Game::getSingleton().getLog()->logMessage("RaceManager destroyed");
}

RaceManager& RaceManager::getSingleton()
{
	static RaceManager* onlyInstance = new RaceManager();

	return *onlyInstance;
}

PlayerStats* RaceManager::createPlayerStats(Player* owner)
{
	PlayerStats* newStats = new PlayerStats();
	m_stats[owner->getID()] = newStats;

	return newStats;
}

void RaceManager::destroyPlayerStats(Player* owner)
{
	auto& it = m_stats.find(owner->getID());
	if (it != m_stats.end())
	{
		PlayerStats* stats = NULL;
		stats = it->second;
		SAFE_DELETE(stats);
		m_stats.erase(owner->getID());
	}
}

unsigned long RaceManager::computePrevLapsSum(Player* owner)
{
	PlayerStats* stats = owner->getPlayerStats();
	assert(stats != NULL && "Player's stats corrupt");
	unsigned long sum = 0;
	for (int i = 0; i < stats->currentLap - 1; i++)
	{
		sum += stats->lapTimes[i];
	}

	return sum;
}

unsigned long RaceManager::calcRaceTiming(Player* key)
{
	unsigned long sum = 0;
	PlayerStats* stats = key->getPlayerStats();
	assert(stats != NULL && "Player's stats corrupt");
	std::cout << "Min: " << stats->min << "\tSec: " << stats->sec << "\tSplit Sec: " << stats->splitSec << std::endl;
	sum = (stats->min * 60 + (stats->splitSec * 0.01) + stats->sec) * 1000;

	return sum;
}

void RaceManager::createTrackData(const Ogre::String& trackname)
{
	SAFE_DELETE(m_trackData); // Destroy previous track data

	m_trackData = new TrackData(trackname);
}