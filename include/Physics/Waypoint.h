#pragma once

class Waypoint
{
public:
	explicit Waypoint(int locationIndex);
	~Waypoint();

	bool operator<(Waypoint* other) { return (this->m_locationIndex < other->getLocationIndex()); }
	bool operator>(Waypoint* other) { return (this->m_locationIndex > other->getLocationIndex()); }

	void onPass();

	const int getLocationIndex() const { return m_locationIndex; }
	bool isPassed() const { return m_passed; }

private:
	const int m_locationIndex;

	int m_passCount;

	bool m_passed;
};