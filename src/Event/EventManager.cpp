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