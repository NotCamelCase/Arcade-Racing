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