/****************************** Header ******************************\
Class Name:	ToggleButtonControl inherits ControlItem
File Name:	ToggleButtonControl.cpp
Summary:	Interface for a toggle button control.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Dylan Watson, Ian Poll
Email:	dylantrwatson@gmail.com, irobot9803@gmail.com
\*********************************************************************/

#include "ToggleButtonControl.h"

using namespace std;
using namespace Controls;

ToggleButtonControl::ToggleButtonControl() { }

ToggleButtonControl::ToggleButtonControl(Joystick *_joy, string _name, int _button, bool _IsReversed, double _powerMultiplier, double _negative, ActiveCollection* ac): ControlItem(_joy, _name, _IsReversed, _powerMultiplier, ac)
{
	button = _button;
	NegativeValue = _negative;
}

double ToggleButtonControl::Update(double _dTime)
{
	bool val = GetButton(vector<int> {button}).at(0) == 1 ? true : false;
	if (val == previousState) return toggleOn ? 1 : -1;
	if (val != previousState && val == true)
	{
		if (toggleOn)
		{
			toggleOn = false;
			ValueChanged(new TEventArgs<double, ToggleButtonControl*>(NegativeValue, this));
		}
		else
		{
			toggleOn = true;
			ValueChanged(new TEventArgs<double, ToggleButtonControl*>(val * powerMultiplier, this));
		}
	}
	previousState = val;
	if(PrintOut)
	{
		Log::General("````````````````````````````" + name + " value: " + to_string(toggleOn));
	}
	return toggleOn ? 1 : -1;
}

ToggleButtonControl::~ToggleButtonControl() { }

