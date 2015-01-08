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