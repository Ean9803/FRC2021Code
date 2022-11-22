/****************************** Header ******************************\
Class Name:	ServoControl inherits ControlItem
File Name:	ServoControl.cpp
Summary:	Interface for an servo control.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Ian Poll
Email:	irobot9803@gmail.com
\*********************************************************************/


#include <cmath>
#include <iostream>

#include "ServoControl.h"

using namespace std;
using namespace Controls;
using namespace Components;

ServoControl::ServoControl(Joystick *_joy, string _name, ActiveCollection* ac, ServoControlPart useType, bool cycle, vector<double> Angles, vector<double> Buttons, double Axis, vector<double> PosDir, double Mult, bool Inc, int Mode)
	: ControlItem(_joy, _name, Axis < 0, Mult, ac)
{
    this->useType = useType;
    this->cycle = cycle;
    this->Angles = Angles;
    this->buttons = Buttons;
    this->Axis = Axis;
    this->Mult = Mult;
    this->Inc = Inc;
    this->Mode = Mode;
    Dir = (PosDir);
}

void ServoControl::DeleteComponent()
{
	delete this;
}

double ServoControl::Update(double _dTime)
{
    if(useType == ServoControlPart::button || useType == ServoControlPart::DPad)
    {
        if(!Pressed)
        {
            if(!cycle && !Inc)
            {
                for (int i = 0; i < (useType == ServoControlPart::button ? (int)buttons.size() : (int)Dir.size()); i++)
                {
                    if(useType == ServoControlPart::button ? GetButton(vector<int> {(int)buttons[i]}).at(0) : GetD_Pad().at(0) == Dir[i])
                    {
                        Pressed = true;
                        CurrentAngle = Angles[fmod(i, Angles.size())];
                    }
                }
            }
            else if(!cycle && Inc)
            {
                for (int i = 0; i < (useType == ServoControlPart::button ? (int)buttons.size() : (int)Dir.size()); i++)
                {
                    if(useType == ServoControlPart::button ? GetButton(vector<int> {(int)buttons[i]}).at(0) : GetD_Pad().at(0) == Dir[i])
                    {
                        Pressed = true;
                        if(fmod(i, 2) == 0)
                        {
                            CurrentAngle += Mult;
                        }
                        else
                        {
                            CurrentAngle -= Mult;
                        }
                    }
                }
            }
            else if(cycle && !Inc)
            {
                for (int i = 0; i < (useType == ServoControlPart::button ? (int)buttons.size() : (int)Dir.size()); i++)
                {
                    if(useType == ServoControlPart::button ? GetButton(vector<int> {(int)buttons[i]}).at(0) : GetD_Pad().at(0) == Dir[i])
                    {
                        Pressed = true;
                        if(fmod(i, 2) == 0)
                        {
                            index++;
                        }
                        else
                        {
                            index--;
                        }
                    }
                }
                if(index < 0)
                {
                    index = Angles.size() - 1;
                }
                else if(index >= Angles.size())
                {
                    index = 0;
                }
                CurrentAngle = Angles[index];
            }
        }
        else
        {
            Pressed = false;
            for (int i = 0; i < (useType == ServoControlPart::button ? (int)buttons.size() : (int)Dir.size()); i++)
            {
                if(useType == ServoControlPart::button ? GetButton(vector<int> {(int)buttons[i]}).at(0) : GetD_Pad().at(0) == Dir[i])
                {
                    Pressed = true;
                }
            }
        }
    }
    else
    {
        double val = GetAxis(vector<int> {(int)Axis}).at(0);
        if(abs(val) > 0.01)
        {
            if(Inc)
                CurrentAngle += Mult * val;
            else
                CurrentAngle = val;//(val + 1) / 2;
        }
    }
    if(CurrentAngle != LastAngle)
    {
        LastAngle = CurrentAngle;
        ValueChanged(new TEventArgs<double, ServoControl*>(CurrentAngle, this, (TEventArgs<double, ServoControl*>::ControlPowerMode)Mode));
    }
    if(PrintOut)
	{
		Log::General("````````````````````````````" + name + " value: " + to_string(CurrentAngle));
	}
    return CurrentAngle;
}


ServoControl::~ServoControl() { }

