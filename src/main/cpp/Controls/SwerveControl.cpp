/****************************** Header ******************************\
Class Name:	SwerveControl
File Name:	SwerveControl.cpp
Summary:	Interface for swerve control.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Ian Poll
Email:	irobot9803@gmail.com
\*********************************************************************/

#define _USE_MATH_DEFINES

#include "SwerveControl.h"
#include <cmath>

using namespace std;
using namespace Controls;
using namespace Components;

SwerveControl::SwerveControl(Joystick *_joy, DriveCalculation _Cal, string _name, int _axisV, int _axisH, int _axisS, double _deadZone, bool _reversed, double _powerMultiplier, ActiveCollection* ac, SwerveManager *Manager) : ControlItem(_joy, _name, _reversed, _powerMultiplier, ac)
{
    SwerveDrive = Manager;
    Cal = _Cal;
    HAxis = abs(_axisH);
    VAxis = abs(_axisV);
    SAxis = abs(_axisS);

    ReverseH = _axisH < 0;
    ReverseV = _axisV < 0;
    ReverseS = _axisS < 0;

    Mult = _powerMultiplier;
    DeadZone = _deadZone;
    Reversed = _reversed;

    m_Collection = ac;
}

double SwerveControl::Update(double _dTime)
{
    SwerveDrive->UpdateSystem(_dTime);
    //SwerveDrive->PrintLocation();
    vector<double> In = GetAxis(vector<int> {HAxis, VAxis, SAxis});
    double rawH = -CalculateDeadZone(In.at(0), DeadZone) * (ReverseH ? -Mult : Mult);
    double rawV = CalculateDeadZone(In.at(1), DeadZone) * (ReverseV ? -Mult : Mult);
    double rawS = -CalculateDeadZone(In.at(2), DeadZone) * (ReverseS ? -Mult : Mult);
    rawV *= -1;

    if(Reversed)
    {
        rawV *= -1;
    }

    if (Cal == SwerveControl::DriveCalculation::Field_Oriented)
    {
        double gyro = m_Collection->GetNavX()->GetNavXAngle(NavX::AxisAngle::Y_Axis) * M_PI / 180;
        m_Collection->GetNavX()->OutPrint();
        double temp = rawH * cos(gyro) + rawV * sin(gyro);
        rawV = -rawH * sin(gyro) + rawV * cos(gyro);
        rawH = temp;
        rawV *= -1;
        rawH *= -1;
    }
    else if (Cal == SwerveControl::DriveCalculation::Warthog)
    {
        rawS = rawH;
        rawH = 0;
    }
    else if (Cal == SwerveControl::DriveCalculation::Warthog_Field_Oriented) //useless..only drive in a stright line relative to start angle and rotate on the line
    {
        double gyro = m_Collection->GetNavX()->GetConstAngle() * M_PI / 180;

        double temp = rawH * cos(gyro) + rawV * sin(gyro);
        rawV = -rawH * sin(gyro) + rawV * cos(gyro);
        rawH = temp;
        
        rawS = rawH;
        rawH = 0;
    }
    double Average = abs((abs(rawH) + abs(rawV) + abs(rawS)) / 3);
    if(Average > 0)
    {
        StopM = true;
        ValueChanged(new IEventArgs<double, double, double, SwerveControl*>(rawV, rawH, rawS, this));
    }
    else if(StopM)
    {
        ValueChanged(new IEventArgs<double, double, double, SwerveControl*>(0, 0, 0, this));
        StopM = false;
    }

    double RawAv =  (((rawH) + (rawV) + (rawS)) / 3);
    if(PrintOut)
	{
		Log::General("````````````````````````````" + name + " value: " + to_string(RawAv));
	}

    return RawAv;
}

void SwerveControl::DeleteComponent()
{
    delete this;
}

SwerveControl::~SwerveControl() {}