/****************************** Header ******************************\
Class Name:	ButtonControl inherits ControlItem
File Name:	ButtonControl.cpp
Summary:	Interface for a single button control.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Dylan Watson, Ian Poll
Email:	dylantrwatson@gmail.com, irobot9803@gmail.com
\*********************************************************************/

#include <iostream>

#include "ButtonControl.h"

using namespace Controls;

ButtonControl::ButtonControl() {}

ButtonControl::ButtonControl(Joystick *_joy, string _name, int _button, bool _actOnRelease, bool _reversed, double _powerMultiplier, bool _isSolenoid, ActiveCollection* ac, int Mode, double HoldTime, bool _isOverdrive)
	: ControlItem(_joy, _name, _reversed, _powerMultiplier, ac) {
	button = _button;
	actOnRelease = _actOnRelease;
	isSolenoid = _isSolenoid;
	previous = 0;
	this->Mode = Mode;
	isOverdrive = _isOverdrive;
	TimeSet = HoldTime;
	Mult = _powerMultiplier;
	reversed = _reversed;
}

void ButtonControl::DeleteComponent()
{
	delete this;
}

double ButtonControl::Update(double _dTime){
	double val = GetButton(vector<int> {abs(button)}).at(0) * (reversed ? -1 : 1) * Mult * Sign(button); //*< Value recieved from the button
	double tmp = val;

	if(abs(val) > 0)
	{
		if(isOverdrive && !isSolenoid)
		{
			TimePassed += _dTime;
			if(TimePassed > TimeSet)
			{
				ValueChanged(new TEventArgs<double, ButtonControl*>(Sign(val) * -1, this, (TEventArgs<double, ButtonControl*>::ControlPowerMode)0));		
			}
			else
			{
				ValueChanged(new TEventArgs<double, ButtonControl*>(val, this, (TEventArgs<double, ButtonControl*>::ControlPowerMode)Mode));
			}
		}
		else
		{
			ValueChanged(new TEventArgs<double, ButtonControl*>(isSolenoid ? (reversed ? -1 : 1) : (val), this, (TEventArgs<double, ButtonControl*>::ControlPowerMode)Mode));
		}
	}
	else if(actOnRelease)
	{
		TimePassed = 0;
		ValueChanged(new TEventArgs<double, ButtonControl*>((isSolenoid ? (reversed ? 1 : -1) : 0), this, (TEventArgs<double, ButtonControl*>::ControlPowerMode)0));
	}
	else
	{
		TimePassed = 0;
	}

	val = (TimePassed > TimeSet ? Sign(val) * -1 : val);
	
	if(PrintOut)
	{
		Log::General("````````````````````````````" + name + " value: " + to_string(val));
	}

	return val;	
}

ButtonControl::~ButtonControl() {}