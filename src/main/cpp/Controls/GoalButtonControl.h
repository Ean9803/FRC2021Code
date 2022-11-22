/****************************** Header ******************************\
Class Name:	GoalButtonControl inherits ControlItem
File Name:	GoalButtonControl.h
Summary:	Interface for a single button control that fires a goal.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Dylan Watson, Ian Poll
Email:	dylantrwatson@gmail.com, irobot9803@gmail.com
\*********************************************************************/

#pragma once

#include "ControlItem.h"
#include "../Components/VictorSPItem.h"
#include "../Components/DoubleSolenoidItem.h"
#include "../Goals/FRC_Goals.h"
#include "../Goals/GoalSelector.h"

namespace Controls{

class GoalButtonControl : public ControlItem {
public:
	enum ControlButtonState
	{
		onRelease = 0,
		onPress = 1,
		both = 2
	};

	GoalButtonControl();
	GoalButtonControl(Joystick *_joy, string _name, vector<int> Axis, ActiveCollection* ac, TeleOpGoal _goal, vector<string> Strings, int _StartIndex, int KeyID, vector<int> RemoveKeys, bool _RepeatWhenFinished = false, ControlButtonState ActivationState = ControlButtonState::onPress, double _powerMultiplier = 1.0);
	virtual ~GoalButtonControl();
	int getSign(double val);
	virtual double Update(double _dTime) override;

	TeleOpGoal m_goal;
	int StartIndex = 0;
	
	int IdKey = 0;
	vector<int> RemoveKeys;
	bool RepeatWhenFinished;

private:
	vector<int> m_Axis;
    vector<double> m_AxisVals;
	vector<string> m_StringVals;
	ControlButtonState State;

	double m_current = 0;
	double m_previous = 0;
	double Mult = 1;
};
}