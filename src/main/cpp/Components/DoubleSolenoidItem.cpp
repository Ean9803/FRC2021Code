/****************************** Header ******************************\
Class Name: DoubleSolenoidItem inherits OutputComponent
File Name:	DoubleSolenoidItem.cpp
Summary: Abstraction for the WPIlib DoubleSolenoid that extends to include
some helper and control methods.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Dylan Watson
Email: dylantrwatson@gmail.com
\********************************************************************/

#include <cmath>
#include <iostream>

#include "DoubleSolenoidItem.h"
#include "../Util/Constants.h"

using namespace std;
using namespace frc;
using namespace Components;
using namespace Util;

DoubleSolenoidItem::DoubleSolenoidItem() {}
//Constructor used in config
DoubleSolenoidItem::DoubleSolenoidItem(string _name, int _forwardChannel, int _reverseChannel, DoubleSolenoid::Value _default, bool _reversed, bool Real)
	: OutputComponent(_name){
	forwardChannel = _forwardChannel;
	reverseChannel = _reverseChannel;
	this->_default = _default;

	reversed = _reversed;
	solenoid = new DoubleSolenoid(forwardChannel, reverseChannel);
	_LastState = _default;
	//TODO: THIS MAY BE BROKEN
	//DefaultSet();
	FromTable(Real);
	if (UseTable)
	{
		Log::General("Using Table values");
		OutputTable->PutBoolean(name, false);
	}
}

/*
The delete component is just a way to clean up space (it was used for quickload so we dont get dups of objects)
*/
void DoubleSolenoidItem::DeleteComponent()
{
	delete solenoid;
	delete this;
}

//Sets the solenoid to the value set in config
void DoubleSolenoidItem::DefaultSet(){
	Set(_default);
}

//Sets the solenoid based off the value
void DoubleSolenoidItem::Set(DoubleSolenoid::Value value){
	if(!inUse){
		inUse = true;
		switch(value){
			case DoubleSolenoid::Value::kForward:
				SetForward();
				break;
			case DoubleSolenoid::Value::kReverse:
				SetReverse();
				break;
			case DoubleSolenoid::Value::kOff:
				SetOff();
				break;
		}
		inUse = false;
	}
}

//returns the current state of the solenoid it may not be the same in real life due to the two hoses in the pistion being swapped
double DoubleSolenoidItem::Get()
{
	double ret = 0.0;
	DoubleSolenoid::Value curState = _LastState;
	switch(curState)
	{
		case DoubleSolenoid::Value::kOff : return 0.0;
		case DoubleSolenoid::Value::kForward : return 1.0;
		case DoubleSolenoid::Value::kReverse : return -1.0;
	}	
	return ret;
}

//Returns the current solenoid state
DoubleSolenoid::Value DoubleSolenoidItem::GetState(){
	return solenoid->Get();
}

//Returns the default solenoid state
DoubleSolenoid::Value DoubleSolenoidItem::GetDefaultValue(){
	return _default;
}

//Sets the solenoid value based off a number value
void DoubleSolenoidItem::Set(double value){
	if(!inUse){
		inUse = true;
		if(reversed)
			value *= -1;
		if(value > 0)
			SetForward();
		else if(value < 0)
			SetReverse();
		else
		{
			SetOff();
		}
		
		if (!(value >= -1 && value <= 1))
		{
			Insta = true;
			if(PrintOut)
				Log::Warning("The valid arguments for DoubleSolenoid " + name + " is Reverse, Off, and Forward (-1, 0, 1), Doing Insta");
		}
		inUse = false;
	}
}

//Loop called when updating that has a timer to set the return value to the given value after a given amout of time has passed that is stated in the config
void DoubleSolenoidItem::UpdateComponent()
{
	if(PrintOut)
	{
		Log::General("````````````````````````````" + name + " value: " + to_string(Get()) + " Time passed: " + to_string(TimeExtended));
	}
	
	if(_CurrentState != _LastState)
	{
		TimeIn += D_Time;
		if((_CurrentState == DoubleSolenoid::Value::kReverse ? TimeIn >= TimeRetracted : TimeIn >= TimeExtended) || Insta)
		{
			_LastState = _CurrentState;
			TimeIn = 0;
			Insta = false;
		}
	}
}

//Sets value of solenoid based off boolean
void DoubleSolenoidItem::Set(bool value){
	Set((double)value);
}

void DoubleSolenoidItem::SetForward(){
	_CurrentState = DoubleSolenoid::Value::kForward; 
	solenoid->Set(DoubleSolenoid::Value::kForward);
	OutputTable->PutBoolean(name, true);
}

void DoubleSolenoidItem::SetReverse(){
	_CurrentState = DoubleSolenoid::Value::kReverse; 
	solenoid->Set(DoubleSolenoid::Value::kReverse);
	OutputTable->PutBoolean(name, false);
}

void DoubleSolenoidItem::SetOff(){
	_CurrentState = DoubleSolenoid::Value::kOff; 
	solenoid->Set(DoubleSolenoid::Value::kOff);
}

DoubleSolenoidItem::~DoubleSolenoidItem() {}

