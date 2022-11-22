/****************************** Header ******************************\
Class Name:	ComboControl inherits ControlItem
File Name:	ComboControl.cpp
Summary:	Interface for an Multi control to one.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Ian Poll
Email:	irobot9803@gmail.com
\*********************************************************************/

#include <iostream>

#include "ComboControl.h"

using namespace Controls;


ComboControl::ComboControl(Joystick *_joy, string _name, ActiveCollection* ac, bool primary, bool FireOnce, bool Goal, TeleOpGoal goal, vector<string> Strings, int _StartIndex, int KeyID, vector<int> RemoveKeys, bool repeat, int Mode, vector<ControlItem*> Controls, vector<NativeComponent*> RobotInput, vector<ControlItem*> Input, vector<double> Values, vector<double> RobotValues, vector<double> LessOrGreater, vector<bool> ABSV, vector<bool> Negate, vector<bool> AO, vector<bool> RAO, vector<double> InLessOrGreater, vector<bool> InABSV, vector<bool> InNegate, vector<bool> NotInRobot, vector<bool> NotInControls, vector<int> ValInGreaterLess, vector<int> ValRobotInGreaterLess, bool UseOut, double Out)
	: ControlItem(_joy, _name, false, 1, ac)
{
    this->Controls = Controls;
    this->RobotInput = RobotInput;
    this->primary = primary;
    
    this->Values = Values;
    this->RobotValues = RobotValues;

    this->NotInControls = NotInControls;
    this->NotInRobot = NotInRobot;

    this->Negate = Negate;
    this->AO = AO;
    this->RAO = RAO;
    this->LessOrGreater = LessOrGreater;
    this->ABSV = ABSV;

    this->InNegate = InNegate;
    this->InLessOrGreater = InLessOrGreater;
    this->InABSV = InABSV;

    this->FireOnce = FireOnce;
    this->Input = Input;
    this->Mode = Mode;
    this->OutUse = Out;
    this->UseOut = UseOut;

    this->ValInGreaterLess = ValInGreaterLess;
    this->ValRobotInGreaterLess = ValRobotInGreaterLess;

    m_StringVals = Strings;
    useGoal = Goal;
    StartIndex = _StartIndex;
    IdKey = KeyID;
	this->RemoveKeys = RemoveKeys;
    m_goal = goal;
    RepeatWhenFinished = repeat;
}

void ComboControl::DeleteComponent()
{
	delete this;
}

double ComboControl::Update(double _dTime)
{
    bool Fire = true;
    for(int i = 0; i < (int)Controls.size(); i++)
    {
        if(i < (int)Values.size())
        {
            bool poll = ComboControl::ProcessBoolean(ComboControl::ProcessVal(Controls[i]->GetInput(), i, false), Values[i], i, ValInGreaterLess, NotInControls);
            if(AO[i])
            {
                Fire = poll || Fire;
            }
            else
            {
                Fire = poll && Fire;
            }
            
        }
        else
        {
            Fire = false;
            Log::Error("!!!!Value size is off from control size!!!!");
        }
    }
    bool robotfire = true;
    for(int i = 0; i < (int)RobotInput.size(); i++)
    {
        if(i < (int)RobotValues.size())
        {
            bool poll = ComboControl::ProcessRobotBoolean(RobotInput[i], RobotValues[i], i);
            if(RAO[i])
            {
                Fire = poll || Fire;
            }
            else
            {
                Fire = poll && Fire;
            }
        }
        else
        {
            robotfire = false;
            Log::Error("!!!!Robot Value size is off from control size!!!!");
        }
    }

    if (RobotInput.size() > 0 && Controls.size() > 0)
    {
        Fire = (primary ? (Fire && robotfire) : (Fire || robotfire));
    }
    else if (RobotInput.size() > 0 && Controls.size() == 0)
    {
        Fire = robotfire;
    }
    else if (RobotInput.size() == 0 && Controls.size() == 0)
    {
        Fire = false;
        Log::Error("Ur Mom is false");
    }
    
    m_AxisVals.clear();
    double Average = 0;
    if(!UseOut)
    {
    for(int i = 0; i < (int)Input.size(); i++)
    {
        double Val = ComboControl::ProcessVal(Input[i]->GetInput(), i, true);
        m_AxisVals.push_back(Val);
        Average += Val;
    }
    Average /= ((int)Input.size() == 0 ? 1 : (int)Input.size());
    }
    else
    {
        m_AxisVals.push_back(OutUse);
        Average = OutUse;
    }

    if(PrintOut)
	{
		Log::General("````````````````````````````" + name + " value: " + to_string(Average) + " | Values met: " + (Fire ? "Yes" : "No"));
	}

    if(Fire)
    {
        if(FireOnce && FireControl)
        {
            FireControl = false;
            if(useGoal)
            {
                ValueChanged(new LEventArgs<vector<double>, vector<string>, ComboControl*>(m_AxisVals, m_StringVals, this));
                return Average;
            }
            else
            {
                ValueChanged(new TEventArgs<double, ComboControl*>(Average, this, (TEventArgs<double, ComboControl*>::ControlPowerMode)Mode));
                return Average;
            }
        }
        else
        {
            if(useGoal)
            {
                ValueChanged(new LEventArgs<vector<double>, vector<string>, ComboControl*>(m_AxisVals, m_StringVals, this));
                return Average;
            }
            else
            {
                ValueChanged(new TEventArgs<double, ComboControl*>(Average, this, (TEventArgs<double, ComboControl*>::ControlPowerMode)Mode));
                return Average;
            }
        }
    }
    else
    {
        FireControl = true;
    }
    return Average;
}

double ComboControl::ProcessVal(double Val, int index, bool Input)
{
    double v = Val;
    if(Input)
    {
        if(index < (int)InLessOrGreater.size())
        {
            if(InLessOrGreater[index] > 0)
            {
                if(v < 0)
                    v = 0;
            }
            else if(InLessOrGreater[index] < 0)
            {
                if(v > 0)
                    v = 0;
            }
        }
        if(index < (int)InABSV.size())
        {
            if(InABSV[index])
            {
                v = abs(v);
            }
        }
        if(index < (int)InNegate.size())
        {
            if(InNegate[index])
            {
                v *= -1;
            }
        }
    }
    else
    {
        if(index < (int)LessOrGreater.size())
        {
            if(LessOrGreater[index] > 0)
            {
                if(v < 0)
                    v = 0;
            }
            else if(LessOrGreater[index] < 0)
            {
                if(v > 0)
                    v = 0;
            }
        }
        if(index < (int)ABSV.size())
        {
            if(ABSV[index])
            {
                v = abs(v);
            }
        }
        if(index < (int)Negate.size())
        {
            if(Negate[index])
            {
                v *= -1;
            }
        }
    }
    return v;
}

bool ComboControl::ProcessBoolean(double ValueIn, double ValueCompare, int index, vector<int> GorLess, vector<bool> Inverse)
{
    bool ReturnBool = false;
    if(GorLess[index] == 0)
    {
        ReturnBool = ValueIn == ValueCompare;
    }
    else if(GorLess[index] > 0)
    {
        ReturnBool = ValueIn > ValueCompare;
    }
    else if(GorLess[index] < 0)
    {
        ReturnBool = ValueIn < ValueCompare;
    }
        
    if(Inverse[index])
        ReturnBool = !ReturnBool;
    return ReturnBool;
}

bool ComboControl::ProcessRobotBoolean(NativeComponent* ValueIn, double ValueCompare, int index)
{
    bool Fire = false;
    if(dynamic_cast<Motor*>(ValueIn))
    {
        Fire = ComboControl::ProcessMotorItem((Motor*)ValueIn, ValueCompare, index);
    }
    else if(dynamic_cast<EncoderItem*>(ValueIn))
    {
        Fire = ComboControl::ProcessEncItem((EncoderItem*)ValueIn, ValueCompare, index);
    }
    else if(dynamic_cast<PotentiometerItem*>(ValueIn))
    {
        Fire = ComboControl::ProcessPotItem((PotentiometerItem*)ValueIn, ValueCompare, index);
    }
    else
    {
        Fire = ComboControl::ProcessNativeItem(ValueIn, ValueCompare, index);
    }
    return Fire;
}

bool ComboControl::ProcessMotorItem(Motor *Ptr, double Value, int index)
{
    if(Ptr->GetReturn() == 0)
    {
        if(ComboControl::ProcessBoolean(Ptr->Get(), Value, index, ValRobotInGreaterLess, NotInRobot))
        {
            return true;
        }   
    }
    else
    {
        if(Ptr->WithinAngle(Value) && ValRobotInGreaterLess[index] == 0)
        {
            return !NotInRobot[index];
        }
        else
        {
            if(ComboControl::ProcessBoolean(Ptr->GetAngle(), Value, index, ValRobotInGreaterLess, NotInRobot))
            {
                return true;
            }  
        }
    }
    return false;
}

bool ComboControl::ProcessEncItem(EncoderItem *Ptr, double Value, int index)
{
    if(Ptr->WithinAngle(Value) && ValRobotInGreaterLess[index] == 0)
    {
        return !NotInRobot[index];
    }
    else
    {
        if(ComboControl::ProcessBoolean(Ptr->GetAngle(), Value, index, ValRobotInGreaterLess, NotInRobot))
        {
            return true;
        }  
    }
    return false;
}

bool ComboControl::ProcessPotItem(PotentiometerItem *Ptr, double Value, int index)
{
    if(Ptr->WithinAngle(Value) && ValRobotInGreaterLess[index] == 0)
    {
        return !NotInRobot[index];
    }
    else
    {
        if(ComboControl::ProcessBoolean(Ptr->GetAngle(), Value, index, ValRobotInGreaterLess, NotInRobot))
        {
            return true;
        }  
    }
    return false;
}

bool ComboControl::ProcessNativeItem(NativeComponent *Ptr, double Value, int index)
{
    if(ComboControl::ProcessBoolean(Ptr->GetValue(), Value, index, ValRobotInGreaterLess, NotInRobot))
    {
        return true;
    }
    return false;
}

ComboControl::~ComboControl() {}