#pragma once

#include <vector>
#include <map>

class IEventListener;

/*
* Singleton class to notify registered EventListeners off an event
* whose type is hardcoded in @EventType enum
*/
class EventManager
{
public:
	~EventManager();

	void registerListener(int eventType, IEventListener* listener);
	void unregisterListener(int eventType, IEventListener* listener);

	/* Notifies all registered event listeners off Event **/
	void notify(int eventType, void* data = NULL);

	static EventManager& getSingleton();

private:
	EventManager();

	std::map<int, std::vector<IEventListener*>> m_eventListeners;
	std::map<int, std::vector<IEventListener*>> m_awaitingRemovals;
	std::map<int, std::vector<IEventListener*>> m_awaitingAdditions;
	
	bool m_pendingEvents;
};