#pragma once

#include <map>

#include <OIS.h>

#include "Utils.h"

/* Pre-defined input values for controlling vehicle */
enum InputType
{
	ACCELERATE = BIT_ME(0),
	BRAKE = BIT_ME(1),
	HAND_BRAKE = BIT_ME(2),
	STEER_LEFT = BIT_ME(3),
	STEER_RIGHT = BIT_ME(4)
};

/* Stores InputType values and updates according to key press & release events
* check efficiently is a key is active
*/
class InputMap
{
public:
	/* Map InputType value to specified KeyCode */
	static void mapInput(unsigned int type, int code);

	/* Check current key code against key mappings for keys pressed */
	static void reflectKeyPress(const OIS::KeyEvent& e);
	
	/* Check current key code against key mappings for keys released */
	static void reflectKeyRelease(const OIS::KeyEvent& e);

	/* 1 if input is currently pressed, 0 otherwise */
	static unsigned int isKeyActive(InputType input, unsigned int keyFlag = NULL);

	static unsigned int getKeyFlags() { return s_keyFlags; }

private:
	InputMap();
	~InputMap();

	static std::map<unsigned int, int> s_inputMap;
	static unsigned int s_keyFlags;
};