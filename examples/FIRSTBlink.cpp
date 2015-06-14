/*
 * FIRSTBlink.cpp
 *
 *  Created on: Jun 14, 2015
 *      Author: Mikhail Kyraha
 */


#include <Arduino.h>

#include "FIRSTCommand.h"
#include "FIRSTScheduler.h"
#include "FIRSTSubsystem.h"
#include "FIRSTTimer.h"


class LEDSubsystem : public FIRSTSubsystem {
public:
  LEDSubsystem(int port, const char *name);
  ~LEDSubsystem();
  void setColor(int color) { /*digitalWrite(m_port, color);*/ m_on = (color > 0);};
  void flipColor() {setColor(m_on ? LOW : HIGH);};
private:
  int m_port;
  bool m_on;
};

LEDSubsystem::LEDSubsystem(int port, const char *name)
  : FIRSTSubsystem(name)
  , m_port(port)
  , m_on(false)
{
  //pinMode(m_port, OUTPUT);
}
LEDSubsystem::~LEDSubsystem(){}



class Blink : public FIRSTCommand {
private:
  FIRSTTimer m_timer;
  LEDSubsystem *m_led;
public:
	Blink(LEDSubsystem *LED);
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();
};

Blink::Blink(LEDSubsystem *led)
{
  m_led = led;
  Requires(led);
}

void Blink::Initialize()
{
  m_timer.Reset();
  m_timer.Start();
  m_led->setColor(HIGH);
}

void Blink::Execute()
{
  if(m_timer.Get() > 1.6) {
    m_timer.Reset();
    m_timer.Start();
    m_led->flipColor();
  }
}

bool Blink::IsFinished() { return false;}
void Blink::End() {}
void Blink::Interrupted() { End(); }


LEDSubsystem *testLED;
Blink *Blinker;

void setup() {
  // put your setup code here, to run once:
  testLED = new LEDSubsystem(12, "Test LED");
  Blinker = new Blink(testLED);
  testLED->SetDefaultCommand(Blinker);
}

void loop() {
  // put your main code here, to run repeatedly:
  FIRSTScheduler::GetInstance()->Run();
  delay(20);
}
