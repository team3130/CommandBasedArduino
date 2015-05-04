/*
 * MooseLights.cpp
 *
 *  Created on: May 3, 2015
 *      Author: Mikhail Kyraha
 */

#include "Arduino.h"
#include "Command.h"
#include "Subsystem.h"

class LEDSubsystem : public Subsystem {
public:
	LEDSubsystem(int numberOfLeds, const char *name);
	~LEDSubsystem();
	void interlace(int size, CRGB *pattern);
	void setColor(CRGB color);
private:
	int m_NumberOfLeds;
	CRGB *leds;
};



