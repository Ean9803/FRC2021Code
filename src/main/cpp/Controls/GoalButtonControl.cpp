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

#include "GoalButtonControl.h"
#include <cmath>

using namespace Controls;

GoalButtonControl::GoalButtonControl() {}

GoalButtonControl::GoalButtonControl(Joystick *_joy, string _name, vector<int> Axis, ActiveCollection* ac, TeleOpGoal _goal, vector<string> Strings, int _StartIndex, int KeyID, vector<int> _RemoveKeys, bool _RepeatWhenFinished, ControlButtonState ActivationState, double _powerMultiplier)
	: ControlItem(_joy, _name, false, _powerMultiplier, ac) {
	m_Axis = Axis;
	m_goal = _goal;
    m_StringVals = Strings;
    StartIndex = _StartIndex;
    IdKey = KeyID;
	RemoveKeys = _RemoveKeys;
    Mult = _powerMultiplier;
    RepeatWhenFinished = _RepeatWhenFinished;
	State = ActivationState;
}

double GoalButtonControl::Update(double _dTime)
{
	m_current = 0;
    m_AxisVals.clear();

    for(int i = 0; i < (int)m_Axis.size(); i++)
    {
        m_current += (joy->GetRawButton(m_Axis[i]) ? 1 : 0);
        if(joy->GetRawButton(m_Axis[i]))
        {
            m_AxisVals.push_back((m_Axis[i] >= 0 ? 1 : -1) * (GetButton(vector<int> {m_Axis[i]}).at(0) ? 1 : 0) * Mult);
        }
        else
        {
            m_AxisVals.push_back(0);
        }
    }
    m_current /= (m_Axis.size() == 0 ? 1 : m_Axis.size());

    if(m_current != m_previous)
    {
		if(State == ControlButtonState::both)
		{
        	ValueChanged(new LEventArgs<vector<double>, vector<string>, GoalButtonControl*>(m_AxisVals, m_StringVals, this));
		}
		else if(State == ControlButtonState::onPress)
		{
			if(m_current > m_previous)
			{
				ValueChanged(new LEventArgs<vector<double>, vector<string>, GoalButtonControl*>(m_AxisVals, m_StringVals, this));
			}
		}
		else
		{
			if(m_current < m_previous)
			{
				ValueChanged(new LEventArgs<vector<double>, vector<string>, GoalButtonControl*>(m_AxisVals, m_StringVals, this));
			}
		}
		m_previous = m_current;
    }
	if(PrintOut)
	{
		Log::General("````````````````````````````" + name + " value: " + to_string(m_current));
	}
    return m_current;
}

GoalButtonControl::~GoalButtonControl() {}