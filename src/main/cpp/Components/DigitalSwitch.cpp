/****************************** Header ******************************\
Class Name: DigitalSwitch inherits OutputComponent
File Name:	DigitalSwitch.h
Summary: Digital Component to help with logic for combo controls
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ian Poll
Email: irobot9803@gmail.com
\********************************************************************/

#include "DigitalSwitch.h"

using namespace std;
using namespace frc;
using namespace Components;
using namespace Util;

DigitalSwitch::DigitalSwitch(){}

//Constructor used in config
DigitalSwitch::DigitalSwitch(string _name, double _Default) : OutputComponent(_name)
{
    State = Def = _CurrentState = _Default;
	if (UseTable)
	{
		Log::General("Using Table values");
		OutputTable->PutNumber(name, _Default);
	}
}

/*
The delete component is just a way to clean up space (it was used for quickload so we dont get dups of objects)
*/
void DigitalSwitch::DeleteComponent()
{
	delete this;
}

//Sets the value of the object to the default value set in the config
void DigitalSwitch::DefaultSet()
{
    _CurrentState = Def;
}

//Sets the value of the digital switch based off solenoid value
void DigitalSwitch::Set(DoubleSolenoid::Value value){
	if(!inUse){
		inUse = true;
		switch(value){
			case DoubleSolenoid::Value::kForward:
				SetOn();
				break;
			case DoubleSolenoid::Value::kReverse:
				SetOff();
				break;
			case DoubleSolenoid::Value::kOff:
				SetDead();
				break;
		}
		inUse = false;
	}
}

//Returns the current value of the switch (-1, 0, 1)
double DigitalSwitch::Get()
{
	return State;
}

//Sets the value of the switch
void DigitalSwitch::Set(double value)
{
    if(value > 0)
		SetOn();
	else if(value < 0)
		SetOff();
    else
	{
		SetDead();
	}
		
	if (!(value >= -1 && value <= 1))
	{
		Insta = true;
	}
}

//Loop called when updating that has a timer to set the return value to the given value after a given amout of time has passed that is stated in the config
void DigitalSwitch::UpdateComponent()
{
	if(PrintOut)
	{
		Log::General("````````````````````````````" + name + " value: " + to_string(Get()) + " Time passed: " + to_string(TimeIn));
	}
	
	if(_CurrentState != State)
	{
		TimeIn += D_Time;
		if((_CurrentState > 0 ? TimeIn >= TimeExtended : TimeIn >= TimeRetracted) || Insta)
		{
			State = _CurrentState;
			TimeIn = 0;
			Insta = false;
		}
	}
}

void DigitalSwitch::SetOn()
{
	_CurrentState = 1;
	OutputTable->PutNumber(name, 1);
}

void DigitalSwitch::SetOff()
{
	_CurrentState = -1;
	OutputTable->PutNumber(name, -1);
}

void DigitalSwitch::SetDead()
{
	_CurrentState = 0;
	OutputTable->PutNumber(name, 0);
}

DigitalSwitch::~DigitalSwitch() {}

