/****************************** Header ******************************\
Class Name: SwerveModule
File Name:	SwerveModule.cpp
Summary: Module that holds the swivel motor and wheel motor
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ian Poll
Email: irobot9803@gmail.com
\********************************************************************/
#define _USE_MATH_DEFINES
#include <iostream>

#include "SwerveModule.h"
#include <cmath>

using namespace std;
using namespace frc;
using namespace Components;

//Constructor used in config
SwerveModule::SwerveModule(string name, Motor *SwivelMtr, Motor *WheelMtr, EncoderItem* SwivelEnc, EncoderItem* WheelEnc, double TicksPerRev, double WheelTicks) : OutputComponent(name)
{

    GetType = SwerveModule::InputType::EncoderType;
    Swivel = SwivelMtr;
    Wheel = WheelMtr;

    SwivelEncoder = SwivelEnc;
    WheelEncoder = WheelEnc;

    EncRevTicks = TicksPerRev;
    WheelEncRevTicks = WheelTicks;
    SwerveModule::ResetEncs();
}

//Constructor used in config
SwerveModule::SwerveModule(string name, Motor *SwivelMtr, Motor *WheelMtr, double TicksPerRev, double WheelTicks) : OutputComponent(name)
{
    Swivel = SwivelMtr;
    Wheel = WheelMtr;
    ((SparkMaxItem*)Wheel)->SetEncoderRev(WheelTicks);

    EncRevTicks = TicksPerRev;
    WheelEncRevTicks = WheelTicks;

    GetType = SwerveModule::InputType::MotorType;

    SwerveModule::ResetEncs();
}

/*
The delete component is just a way to clean up space (it was used for quickload so we dont get dups of objects)
*/
void SwerveModule::DeleteComponent()
{
    delete this;
}

//Sets the power percent of the wheel motor of the swerve module
void SwerveModule::Set(double val)
{
    Wheel->SetPercent(val * Dir);
}

//Sets the power percent of the swivel motor of the swerve module
void SwerveModule::SetSwivel(double SwivelVal)
{
    Swivel->SetPercent(SwivelVal);
}

//Sets both the wheel and swivel motor power percent
void SwerveModule::Set(double val, double SwivelVal)
{
    SwerveModule::Set(val);
    SwerveModule::SetSwivel(SwivelVal);
}

//Gets the current wheel motor power percent
double SwerveModule::Get()
{
    return Wheel->Get();
}

//Gets the current swivel motor power percent
double SwerveModule::GetSwivel()
{
    return Swivel->Get();
}

//Gets the current encoder value of the wheel motor
double SwerveModule::GetEnc()
{
    if (GetType == SwerveModule::InputType::EncoderType)
        return WheelEncoder->Get();
    else
        return (Wheel)->GetEncoder()->Get();
}

//Gets the current encoder value of the swivel motor
double SwerveModule::GetSwivelEnc()
{
    if (GetType == SwerveModule::InputType::EncoderType)
        return SwivelEncoder->Get();
    else
        return (Swivel)->GetEncoder()->Get();
}

//Resets the encoder value of the swivel motor
void SwerveModule::ResetSwivelEnc()
{
    if (GetType == SwerveModule::InputType::EncoderType)
        SwivelEncoder->Reset();
    else
        (Swivel)->GetEncoder()->Reset();
}

//Resets the encoder value of the wheel motor
void SwerveModule::ResetWheelEnc()
{
    if (GetType == SwerveModule::InputType::EncoderType)
        WheelEncoder->Reset();
    else
        (Wheel)->GetEncoder()->Reset();
}

//Resets the encoder value of the wheel motor and the swivel motor
void SwerveModule::ResetEncs()
{
    SwerveModule::ResetSwivelEnc();
    SwerveModule::ResetWheelEnc();
}

//Updates the speed of the wheel on a 1:1 ratio of the motor (thats why sacale in the swerve manager exists)
void SwerveModule::UpdateWheelRate()
{
    double Pos = 0;
    if (GetType == SwerveModule::InputType::EncoderType)
    {
        Pos = WheelEncoder->Get();
    }
    else
    {
        Pos = (Wheel)->GetEncoder()->Get();
    }

    double LastPos = Pos;
    if (LastWheelTick != LastPos)
    {
        Pos -= LastWheelTick;
        LastWheelTick = LastPos;
        LastChange = Pos;
    }
    else
    {
        Pos = LastChange;
    }
    
    Pos = abs(Pos) * Wheel->GetPowerProfile()->Sign(SwerveModule::Get());
    Pos /= WheelEncRevTicks;
    Pos *= WheelDi * M_PI;
    Pos /= D_Time;
    Pos *= abs(SwerveModule::Get()) > 0 ? 1 : 0;
    
    LastSpeed = Pos;
    OutputTable->PutNumber(name + "-Speed", LastSpeed);
}

//Resets the PID position of the swivel and wheel motors
void SwerveModule::ResetPID()
{
    Wheel->GetPositionProfile()->Reset();
    Swivel->GetPositionProfile()->Reset();
}

//Sets the delta time between each loop
void SwerveModule::SetDeltaTime(double Time)
{
    D_Time = Time;
    if(PrintOut)
        Log::General("```````````````````````````" + name + " Wheel angle: " + to_string(Wheel->GetAngle()) + " Swivel angle: " + to_string(Swivel->GetAngle()));
}

//Calls SetPosition on the given motor so the motor will go to that encoder position
void SwerveModule::ProcessMotor(Motor *Subject, double Enc, double Target, double TickRev)
{
    Subject->SetPosition((Target / 360) * TickRev, D_Time);
}

//Calculates and sets the target angle of the swivel motor returns true when reached
bool SwerveModule::SetTargetSwivel(double Target)
{
    PIDProfile* Ref = Swivel->GetPositionProfile();
    CurrentSwivelTarget = Target;

    Target = fmod(Target, 360);
    if(Target > 180)
        Target = Target - 360;
    if(Ref->Sign(LastSign) != Ref->Sign(Target))
    {
        if(Ref->CloserTo(LastSign, 0, 180) == 180 && Ref->CloserTo(Target, 0, 180) == 180)
        {
            if(LastSign > Target)
                Revs++;
            else
                Revs--;
        }
    }
    LastSign = Target;
    SmoothTarget = Target + (360 * Revs);

    SwerveModule::ProcessMotor(Swivel, SwerveModule::GetSwivelEnc(), SmoothTarget, EncRevTicks);
    return (Swivel->GetPositionProfile()->ABSValue(SmoothTarget - (SwerveModule::GetSwivelEnc() / EncRevTicks) * 360) > 45 ? Swivel->GetPositionProfile()->Inrange(SmoothTarget, (SwerveModule::GetSwivelEnc() / EncRevTicks) * 360, 5) : true);
}

//Sets the target encoder value for the wheel motor returns true when reached
bool SwerveModule::SetTargetWheel(double Target)
{
    CurrentWheelTarget = Target;
    SwerveModule::ProcessMotor(Wheel, SwerveModule::GetEnc(), Target, WheelEncRevTicks);
    return Wheel->GetPositionProfile()->Inrange(Target, (SwerveModule::GetEnc() / EncRevTicks) * 360, 1);
}

//Sets the targets for the wheel and swivel encoders returns true when reached
bool SwerveModule::SetTarget(double Wheel_Target, double Swivel_Target)
{
    return SwerveModule::SetTargetSwivel(Swivel_Target) && SwerveModule::SetTargetWheel(Wheel_Target);
}

//Sets the target speed (RPM) of the wheel motor
bool SwerveModule::SetSpeedTarget(double SPEEEED)
{
    return Wheel->SetToSpeed(SPEEEED);
}

//This is not used for anything but needs to be stated because it is inheriting an abstract class
void SwerveModule::DefaultSet()
{
    Log::Error("WHY DID YOU CALL THE DEFAULT SET FOR A MOTOR?!? Yell at your programmers!");
}

//This is not used for anything but needs to be stated because it is inheriting an abstract class
void SwerveModule::Set(DoubleSolenoid::Value value)
{
    Log::Error("WHY DID YOU CALL THE DOUBLESOLENOID SET FOR A MOTOR?!? Yell at your programmers!");
}