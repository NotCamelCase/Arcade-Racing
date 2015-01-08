#include "Event\EventManager.h"

#include "Event\IEventListener.h"
#include "Utils.h"

EventManager::EventManager()
	: m_pendingEvents(false)
{
}

EventManager::~EventManager()
{
	m_awaitingAdditions.clear();
	m_awaitingRemovals.clear();
	m_eventListeners.clear();

	for (auto& listenerList : m_awaitingAdditions)
	{
		if (!listenerList.second.empty())
			listenerList.second.clear();
	}
	m_awaitingAdditions.clear();

	for (auto& listenerList : m_awaitingRemovals)
	{
		if (!listenerList.second.empty())
			listenerList.second.clear();
	}
	m_awaitingRemovals.clear();

	for (auto& listenerList : m_eventListeners)
	{
		if (!listenerList.second.empty())
			listenerList.second.clear();
	}
	m_eventListeners.clear();

	printf("EventManager destroyed\n");
}

EventManager& EventManager::getSingleton()
{
	static EventManager* onlyInstanece = new EventManager();

	return *onlyInstanece;
}

void EventManager::registerListener(int eventType, IEventListener* listener)
{
	if (!m_pendingEvents)
	{
		std::vector<IEventListener*>& listenerList = m_eventListeners[eventType];
		listenerList.push_back(listener);
	}
	else
	{
		std::vector<IEventListener*>& listenerList = m_awaitingAdditions[eventType];
		listenerList.push_back(listener);
	}
}

void EventManager::unregisterListener(int eventType, IEventListener* listener)
{
	if (!m_pendingEvents)
	{
		std::vector<IEventListener*>& listenerList = m_eventListeners[eventType];
		listenerList.erase(std::remove(listenerList.begin(), listenerList.end(), listener));
	}
	else
	{
		std::vector<IEventListener*>& listenerList = m_awaitingRemovals[eventType];
		listenerList.push_back(listener);
	}
}

void EventManager::notify(int eventType, void* data)
{
	for (auto& listenerAddition : m_awaitingAdditions)
	{
		std::vector<IEventListener*>& listenerList = m_eventListeners[listenerAddition.first];
		listenerList.insert(listenerList.begin(), listenerAddition.second.begin(), listenerAddition.second.end());
	}
	m_awaitingAdditions.clear();

	for (auto& listenerRemoval : m_awaitingRemovals)
	{
		std::vector<IEventListener*>& listenerList = m_eventListeners[listenerRemoval.first];
		for (auto& listener : listenerRemoval.second)
		{
			listenerList.erase(std::remove(listenerList.begin(), listenerList.end(), listener));
		}
		listenerRemoval.second.clear();
	}
	m_awaitingRemovals.clear();

	m_pendingEvents = true;

	const std::vector<IEventListener*>& listenerList = m_eventListeners[eventType];
	for (const auto& listener : listenerList)
	{
		listener->onEvent(eventType, data);
	}

	m_pendingEvents = false;
}