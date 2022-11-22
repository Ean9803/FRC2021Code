/****************************** Header ******************************\
Class Name:	POVGoalControl inherits ControlItem
File Name:	POVGoalControl.cpp
Summary:	Interface for an axis control.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Ian Poll
Email:	irobot9803@gmail.com
\*********************************************************************/


#include <cmath>
#include <iostream>

#include "POVGoalControl.h"

using namespace std;
using namespace Controls;
using namespace Components;

POVGoalControl::POVGoalControl(Joystick *_joy, string _name, double PosDirection, double _powerMultiplier, ActiveCollection* ac, TeleOpGoal _goal, vector<string> Strings, int _StartIndex, int KeyID, vector<int> _RemoveKeys, bool _RepeatWhenFinished,  POVGoalControlMode Mode, POVGoalControlState State)
	: ControlItem(_joy, _name, (PosDirection < 0), _powerMultiplier, ac)
{
    this->Mode = Mode;
    if(Mode == POVGoalControlMode::JoystickMode) { LastPress = 0; }
    this->State = State;
    m_goal = _goal;
    m_StringVals = Strings;
    RepeatWhenFinished = _RepeatWhenFinished;
    StartIndex = _StartIndex;
    IdKey = KeyID;
	RemoveKeys = _RemoveKeys;
    Mult = _powerMultiplier;
    Dir = abs(PosDirection);
    StartIndex = _StartIndex;
    Reverse = PosDirection < 0;
    NegDir = fmod(abs(PosDirection) + 180, 360);
}

void POVGoalControl::DeleteComponent()
{
	delete this;
}

double POVGoalControl::Update(double _dTime)
{
    int Direction = (int)GetD_Pad().at(0);
    if(Mode == POVGoalControlMode::JoystickMode)
    {
        int Val = 0;
        if(Direction == Dir)
        {
            Val = 1;
        }
        else if(Direction == NegDir)
        {
            Val = -1;
        }
        if(LastPress != Val)
            ValueChanged(new LEventArgs<vector<double>, vector<string>, POVGoalControl*>(vector<double> {(Direction == Dir ? 1 : 0) * Mult}, m_StringVals, this));
    }
    else
    {
        if(LastPress != Direction && (Direction == Dir || Direction == -1))
        {
    		if(State == POVGoalControlState::both)
	    	{
            	ValueChanged(new LEventArgs<vector<double>, vector<string>, POVGoalControl*>(vector<double> {(Direction == Dir ? 1 : 0) * Mult}, m_StringVals, this));
		    }
    		else if(State == POVGoalControlState::onPress)
	    	{
		    	if(Direction == Dir)
			    {
				    ValueChanged(new LEventArgs<vector<double>, vector<string>, POVGoalControl*>(vector<double> {(Direction == Dir ? 1 : 0) * Mult}, m_StringVals, this));
    			}
	    	}
		    else
    		{
	    		if(Direction != Dir)
		    	{
			    	ValueChanged(new LEventArgs<vector<double>, vector<string>, POVGoalControl*>(vector<double> {(Direction == Dir ? 1 : 0) * Mult}, m_StringVals, this));
    			}
	    	}
		    LastPress = Direction;
        }
    }
    if(PrintOut)
	{
		Log::General("````````````````````````````" + name + " value: " + to_string((Direction == Dir ? 1 : (Direction == NegDir ? -1 : 0))));
	}
    return (Direction == Dir ? 1 : (Direction == NegDir ? -1 : 0));
}


POVGoalControl::~POVGoalControl() { }

