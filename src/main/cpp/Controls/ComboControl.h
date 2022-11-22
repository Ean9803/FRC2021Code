/****************************** Header ******************************\
Class Name:	ComboControl inherits ControlItem
File Name:	ComboControl.h
Summary:	Interface for an Multi control to one.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Ian Poll
Email:	irobot9803@gmail.com
\*********************************************************************/

#ifndef SRC_CONTROLS_COMBOCONTROL_H_
#define SRC_CONTROLS_COMBOCONTROL_H_


#include "ControlItem.h"
#include "../Goals/FRC_Goals.h"
#include "../Goals/FRC_Goals.h"
#include "../Goals/GoalSelector.h"

using namespace std;

namespace Controls{
class ComboControl : public ControlItem {
private:
	vector<ControlItem*> Controls;
    vector<NativeComponent*> RobotInput;
    vector<ControlItem*> Input;
    vector<double> Values;
    vector<double> RobotValues;
    vector<bool> AO;
    vector<bool> Negate;
    vector<bool> ABSV;
    vector<double> LessOrGreater;
    vector<bool> InNegate;
    vector<bool> InABSV;
    vector<double> InLessOrGreater;
    vector<bool> NotInRobot;
    vector<bool> NotInControls;
    vector<int> ValInGreaterLess;
    vector<int> ValRobotInGreaterLess;
    vector<bool> RAO;
    double OutUse = 0;
    bool UseOut = false;
    bool FireOnce;
    bool primary;
    bool FireControl = true;
    bool useGoal;
    vector<string> m_StringVals;
    vector<double> m_AxisVals;
    int Mode;
    double ProcessVal(double Val, int index, bool Input);
    bool ProcessBoolean(double ValueIn, double ValueCompare, int index, vector<int> GorLess, vector<bool> Inverse);
    bool ProcessRobotBoolean(NativeComponent* ValueIn, double ValueCompare, int index);
    
    bool ProcessMotorItem(Motor *Ptr, double Value, int index);
    bool ProcessEncItem(EncoderItem *Ptr, double Value, int index);
    bool ProcessPotItem(PotentiometerItem *Ptr, double Value, int index);
    bool ProcessNativeItem(NativeComponent *Ptr, double Value, int index);

public:
	ComboControl(Joystick *_joy, string _name, ActiveCollection* ac, bool primary, bool FireOnce, bool Goal, TeleOpGoal goal, vector<string> Strings, int _StartIndex, int KeyID, vector<int> RemoveKeys, bool repeat, int Mode, vector<ControlItem*> Controls, vector<NativeComponent*> RobotInput, vector<ControlItem*> Input, vector<double> Values, vector<double> RobotValues, vector<double> LessOrGreater, vector<bool> ABSV, vector<bool> Negate, vector<bool> AO, vector<bool> RAO, vector<double> InLessOrGreater, vector<bool> InABSV, vector<bool> InNegate, vector<bool> NotInRobot, vector<bool> NotInControls, vector<int> ValInGreaterLess, vector<int> ValRobotInGreaterLess, bool UseOut, double Out);
	virtual ~ComboControl();
	virtual void DeleteComponent() override;
	virtual double Update(double _dTime) override;

    TeleOpGoal m_goal;
	int StartIndex = 0;
	
	int IdKey = 0;
	vector<int> RemoveKeys;
	bool RepeatWhenFinished;
};
}

#endif /* SRC_CONTROLS_COMBOCONTROL_H_ */
