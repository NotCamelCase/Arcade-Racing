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