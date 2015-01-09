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

#include "InputMap.h"

std::map<unsigned int, int> InputMap::s_inputMap;
unsigned int InputMap::s_keyFlags;

InputMap::InputMap()
{
}

InputMap::~InputMap()
{
}

void InputMap::mapInput(unsigned int type, int code)
{
	auto it = s_inputMap.find(type);
	if (it == s_inputMap.end())
	{
		// Insert new input code for type
		s_inputMap.insert(std::make_pair(type, code));
	}
	else {
		// Update the input code for type
		it->second = code;
	}
}

unsigned int InputMap::isKeyActive(InputType input, unsigned int keyFlag)
{
	if (keyFlag)
		return keyFlag & input;
	else
		return (InputMap::s_keyFlags & input);
}

void InputMap::reflectKeyPress(const OIS::KeyEvent& e)
{
	for (const auto& input : InputMap::s_inputMap)
	{
		if (e.key == input.second)
		{
			InputMap::s_keyFlags |= input.first;
		}
	}
}

void InputMap::reflectKeyRelease(const OIS::KeyEvent& e)
{
	for (const auto& input : InputMap::s_inputMap)
	{
		if (e.key == input.second)
		{
			InputMap::s_keyFlags &= ~input.first;
		}
	}
}