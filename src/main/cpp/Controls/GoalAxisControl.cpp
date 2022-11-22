/****************************** Header ******************************\
Class Name:	GoalAxisControl inherits ControlItem
File Name:	GoalAxisControl.cpp
Summary:	Interface for a multiple axis control.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Ian Poll
Email:	irobot9803@gmail.com
\*********************************************************************/

#include "GoalAxisControl.h"
#include <cmath>

using namespace Controls;

GoalAxisControl::GoalAxisControl() {}

GoalAxisControl::GoalAxisControl(Joystick *_joy, string _name, vector<int> Axis, ActiveCollection* ac, TeleOpGoal _goal, vector<string> Strings, int _StartIndex, int KeyID, vector<int> _RemoveKeys, bool _RepeatWhenFinished, double DeadZone, double _powerMultiplier) : ControlItem(_joy, _name, false, _powerMultiplier, ac)
{
	m_Axis = Axis;
	m_goal = _goal;
    m_StringVals = Strings;
    DeadZ = DeadZone;
    StartIndex = _StartIndex;
    IdKey = KeyID;
	RemoveKeys = _RemoveKeys;
    Mult = _powerMultiplier;
    RepeatWhenFinished = _RepeatWhenFinished;
}

double GoalAxisControl::Update(double _dTime)
{
    m_current = 0;
    m_AxisVals.clear();

    for(int i = 0; i < m_Axis.size(); i++)
    {
        m_current += joy->GetRawAxis(m_Axis[i]);
        if(abs(joy->GetRawAxis(m_Axis[i])) > DeadZ)
        {
            m_AxisVals.push_back((m_Axis[i] >= 0 ? 1 : -1) * GetAxis(vector<int> {m_Axis[i]}).at(0) * Mult);
        }
        else
        {
            m_AxisVals.push_back(0);
        }
    }
    m_current /= m_Axis.size();

    if(m_current != m_previous)
    {
        m_previous = m_current;
        ValueChanged(new LEventArgs<vector<double>, vector<string>, GoalAxisControl*>(m_AxisVals, m_StringVals, this));
    }
    if(PrintOut)
	{
		Log::General("````````````````````````````" + name + " value: " + to_string(m_current));
	}
    
    return m_current;
}

GoalAxisControl::~GoalAxisControl() {}