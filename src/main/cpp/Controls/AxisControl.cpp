/****************************** Header ******************************\
Class Name:	AxisControl inherits ControlItem
File Name:	AxisControl.cpp
Summary:	Interface for an axis control.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Dylan Watson, Ian Poll
Email:	dylantrwatson@gmail.com, irobot9803@gmail.com
\*********************************************************************/


#include <cmath>
#include <iostream>

#include "AxisControl.h"
#include "..\Util\Constants.h"

using namespace std;
//using namespace Util;
using namespace Controls;
using namespace Components;

AxisControl::AxisControl() { }

AxisControl::AxisControl(Joystick *_joy, string _name, int _axis, double _deadZone, bool _reversed, double _powerMultiplier, ActiveCollection* ac, int Mode, bool _useOverdrive)
	: ControlItem(_joy, _name, _reversed, _powerMultiplier, ac)
{
	axis = _axis;
	deadZone = _deadZone;
	isLift = false;
	m_activeCollection = ac;
	Overdrive = _useOverdrive;
	this->Mode = Mode;
	reversed = _reversed;
	Mult = _powerMultiplier;
}

double AxisControl::Update(double _dTime)
{
	double Val = GetAxis(vector<int> {abs(axis)}).at(0) * (Mult) * (reversed ? -1 : 1) * Sign(axis);
	
	if(abs(Val) > deadZone)
	{
		if(Overdrive)
		{
			if(abs(Val) > 0.95)
				ValueChanged(new TEventArgs<double, AxisControl*>(Sign(Val) * 1, this, (TEventArgs<double, AxisControl*>::ControlPowerMode)0));
			else
				ValueChanged(new TEventArgs<double, AxisControl*>(Val, this, (TEventArgs<double, AxisControl*>::ControlPowerMode)Mode));
		}
		else
			ValueChanged(new TEventArgs<double, AxisControl*>(Val, this, (TEventArgs<double, AxisControl*>::ControlPowerMode)Mode));
	}
	else
	{
		if(isLift)
			ValueChanged(new TEventArgs<double, AxisControl*>(gane, this, (TEventArgs<double, AxisControl*>::ControlPowerMode)Mode));
		else
			ValueChanged(new TEventArgs<double, AxisControl*>(0, this, (TEventArgs<double, AxisControl*>::ControlPowerMode)Mode));
	}
	if(PrintOut)
	{
		Log::General("````````````````````````````" + name + " value: " + to_string(abs(Val) > deadZone ? Val : 0));
	}
	return abs(Val) > deadZone ? Val : 0;
}

void AxisControl::SetLift(double _gane, ActiveCollection* activeCollection){
	gane = _gane;
	isLift = true;
	m_activeCollection = activeCollection;
}

AxisControl::~AxisControl() { }

