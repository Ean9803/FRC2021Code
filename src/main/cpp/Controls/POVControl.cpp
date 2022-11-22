/****************************** Header ******************************\
Class Name:	POVControl inherits ControlItem
File Name:	POVControl.cpp
Summary:	Interface for an axis control.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Ian Poll
Email:	irobot9803@gmail.com
\*********************************************************************/


#include <cmath>
#include <iostream>

#include "POVControl.h"

using namespace std;
using namespace Controls;
using namespace Components;

POVControl::POVControl(Joystick *_joy, string _name, double PosDirection, double _powerMultiplier, ActiveCollection* ac, int ControlMode, POVControlMode Mode)
	: ControlItem(_joy, _name, (PosDirection < 0), _powerMultiplier, ac)
{
    this->Mode = Mode;
    Mult = _powerMultiplier;
    Dir = abs(PosDirection);
    ControlM = ControlMode;
    Reverse = PosDirection < 0;
    NegDir = fmod(abs(PosDirection) + 180, 360);
}

void POVControl::DeleteComponent()
{
	delete this;
}

double POVControl::Update(double _dTime)
{
    int Direction = (int)GetD_Pad().at(0);
    if(Mode == POVControlMode::JoystickMode)
    {
        if(Direction == Dir)
        {
            ValueChanged(new TEventArgs<double, POVControl*>(Mult * (Reverse ? -1 : 1), this, (TEventArgs<double, POVControl*>::ControlPowerMode)ControlM));
        }
        else if(Direction == NegDir)
        {
            ValueChanged(new TEventArgs<double, POVControl*>(-Mult * (Reverse ? -1 : 1), this, (TEventArgs<double, POVControl*>::ControlPowerMode)ControlM));
        }
        else
        {
            ValueChanged(new TEventArgs<double, POVControl*>(0, this, (TEventArgs<double, POVControl*>::ControlPowerMode)ControlM));
        }
    }
    else
    {
        if(Direction == Dir)
        {
            ValueChanged(new TEventArgs<double, POVControl*>(Mult * (Reverse ? -1 : 1), this, (TEventArgs<double, POVControl*>::ControlPowerMode)ControlM));
        }
        else
        {
            ValueChanged(new TEventArgs<double, POVControl*>(0, this, (TEventArgs<double, POVControl*>::ControlPowerMode)0));
        }
    }
    if(PrintOut)
	{
		Log::General("````````````````````````````" + name + " value: " + to_string(Mode != POVControlMode::JoystickMode ? (Direction == Dir ? 1 : 0) : (Direction == Dir ? 1 : (Direction == NegDir ? -1 : 0))));
        Log::General("````````````````````````````Bindings:");
        for (int i = 0; i < components.size(); i++)
        {
            Log::General("------+: " + components[i]->name);
        }
	}
    return (Mode != POVControlMode::JoystickMode ? (Direction == Dir ? 1 : 0) : (Direction == Dir ? 1 : (Direction == NegDir ? -1 : 0))) * Mult * (Reverse ? -1 : 1);
}


POVControl::~POVControl() { }

