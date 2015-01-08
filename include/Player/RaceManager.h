#pragma once

#include "Utils.h"

#define MAX_LAP_COUNT 6 // Max lap count within tracks

#define CURRENT_TRACK_COUNT 2

class NBID;
class Player;

struct TrackData
{
	Ogre::String objName; // GOD object name defined in .god
	Ogre::String niceName;

	int lapCount;

	TrackData(const Ogre::String& obj);
	~TrackData();
};

/* Used to measure player race data to be saved locally for comparison in sorting pole positions in MULTI */
struct PlayerStats
{
	int currentLap;
	int vehicleSpeed;

	unsigned int min, sec, splitSec;
	unsigned long lapTimes[MAX_LAP_COUNT]; // Microsecs spent per lap as measured by Timer

	unsigned long bestLapTimes[CURRENT_TRACK_COUNT];

	PlayerStats()
		: currentLap(0), vehicleSpeed(0), min(0), sec(0), splitSec(0)
	{
	}

	~PlayerStats()
	{
	}
};

/*
* RaceManager handles general player and race statistics
* that'll be kept online in Scoreoid
*/
class RaceManager
{
public:
	~RaceManager();

	/* Create new PlayerStats object to keep track of player race data */
	PlayerStats* createPlayerStats(Player* owner);

	/* Compute the sum of previous laps' timing */
	unsigned long computePrevLapsSum(Player* key);

	/* Calculate overall race timing in milliseconds */
	unsigned long calcRaceTiming(Player* key);

	/* Destroy early-created PlayerStats object*/
	void destroyPlayerStats(Player* owner);

	void createTrackData(const Ogre::String& trackname);
	TrackData* getTrackData() const { return m_trackData; }

	static RaceManager& getSingleton();

private:
	RaceManager();

	std::map<NBID*, PlayerStats*, Utils::ComparisonFunctor<NBID>> m_stats;
	TrackData* m_trackData;
};