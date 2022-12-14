/****************************** Header ******************************\
Class Name: SwerveManager
File Name:	SwerveManager.cpp
Summary: Manager that holds the Swerve modules
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ian Poll
Email: irobot9803@gmail.com
\********************************************************************/

#define _USE_MATH_DEFINES

#include "SwerveManager.h"
#include <cmath>

using namespace std;
using namespace frc;
using namespace Components;

//Constructor used in config
SwerveManager::SwerveManager(string name, bool Wait, SwerveModule *FrontLeft, SwerveModule *FrontRight, SwerveModule *BackLeft, SwerveModule *BackRight, double _Length, double _Width, double WheelSize, double RobotScale) : OutputComponent(name)
{
    Modules.push_back(FrontLeft);
    Modules.push_back(BackLeft);
    Modules.push_back(FrontRight);
    Modules.push_back(BackRight);
    SwerveManager::SetL(_Length);
    SwerveManager::SetW(_Width);
    Pos = new double_Vector2();
    WaitSwivel = Wait;
    SwerveManager::TableSetUp();
    SwerveManager::SetWheelDiameter(WheelSize);
    SwerveManager::SetRobotScale(RobotScale);
}

//Constructor used in config
SwerveManager::SwerveManager(string name, bool Wait, SwerveModule *FrontLeft, SwerveModule *FrontRight, SwerveModule *BackLeft, SwerveModule *BackRight, NavX *Nav, double _Length, double _Width, double WheelSize, double RobotScale) : OutputComponent(name)
{
    Modules.push_back(FrontLeft);
    Modules.push_back(BackLeft);
    Modules.push_back(FrontRight);
    Modules.push_back(BackRight);
    SwerveManager::SetL(_Length);
    SwerveManager::SetW(_Width);
    Pos = new double_Vector2();
    RobotNav = Nav;
    WaitSwivel = Wait;
    SwerveManager::TableSetUp();
    SwerveManager::SetWheelDiameter(WheelSize);
    SwerveManager::SetRobotScale(RobotScale);
}

//Constructor used in config
SwerveManager::SwerveManager(string name, bool Wait, vector<SwerveModule*> Swerve_Modules, NavX *Nav, double _Length, double _Width, double WheelSize, double RobotScale) : OutputComponent(name)
{
    Modules = Swerve_Modules;
    Pos = new double_Vector2();
    SwerveManager::SetL(_Length);
    SwerveManager::SetW(_Width);
    RobotNav = Nav;
    WaitSwivel = Wait;
    SwerveManager::TableSetUp();
    SwerveManager::SetWheelDiameter(WheelSize);
    SwerveManager::SetRobotScale(RobotScale);
}

//This sets up the network table stuff so the simulation in the folder ApplicationZIPs can modifiy it (The sim cant put new values in the NT so this code does it)
void SwerveManager::TableSetUp()
{
    for(int i = 0; i < Modules.size(); i++)
    {
        OutputTable->PutNumber(Modules.at(i)->GetWheelMtr()->name + "-Thres", 0.1);    
    }
    OutputTable->PutNumber("BotX", 0);
    OutputTable->PutNumber("BotY", 0);
    OutputTable->PutNumber("BotZ", 0);
    SimPos = new double_Vector2();
}

/*
The robot scale is used to help make getting telemetry working faster by increasing or decreasing the effect the movement has on the robots estimated position
if you are on cruch because the robot was 80% completed a week and a half before comp and the change of the input for the wheel size value doesnt give you a
correct position or the robot thinks its going 50 feet per second you can change the scale
*/
void SwerveManager::SetRobotScale(double Scale)
{
    OutputTable->PutNumber("2A-Scale", Scale);
}

/*
The delete component is just a way to clean up space (it was used for quickload so we dont get dups of objects)
*/
void SwerveManager::DeleteComponent()
{
    if(SimPos != nullptr)
        delete SimPos;
    delete Pos;
    delete this;
}

//This will set all the modules wheels in the swerve manager to this output percent value
void SwerveManager::Set(double val)
{
    for(int i = 0; i < Modules.size(); i++)
    {
        Modules.at(i)->Set(val);
    }
}

//This will set all the modules swivels in the swerve manager to this output percent value
void SwerveManager::SetSwivel(double val)
{
    for(int i = 0; i < Modules.size(); i++)
    {
        Modules.at(i)->SetSwivel(val);
    }
}

//This updates all the modules in the swerve manager to track speed and angle for telemetry
void SwerveManager::UpdateModules()
{
    for(int i = 0; i < Modules.size(); i++)
    {
        Modules.at(i)->UpdateWheelRate();
    }
}

//This gets all the data that the swerve modules made when calling the UpdateModules()
void SwerveManager::UpdateLoc()
{
    if (RobotNav != nullptr)
    {
        SwerveManager::SetWheelDiameter(OutputTable->GetNumber("1A-Wheel_Size", 1));

        double FL_B = sin(SwerveManager::Get(SwerveModule::Location::Front_Left)->GetSwivelTarget() * M_PI / 180) * SwerveManager::Get(SwerveModule::Location::Front_Left)->GetWheelRate();
        double FR_B = sin(SwerveManager::Get(SwerveModule::Location::Front_Right)->GetSwivelTarget() * M_PI / 180) * SwerveManager::Get(SwerveModule::Location::Front_Right)->GetWheelRate();
        double BL_A = sin(SwerveManager::Get(SwerveModule::Location::Back_Left)->GetSwivelTarget() * M_PI / 180) * SwerveManager::Get(SwerveModule::Location::Back_Left)->GetWheelRate();
        double BR_A = sin(SwerveManager::Get(SwerveModule::Location::Back_Right)->GetSwivelTarget() * M_PI / 180) * SwerveManager::Get(SwerveModule::Location::Back_Right)->GetWheelRate();
    
        double FR_C = cos(SwerveManager::Get(SwerveModule::Location::Front_Right)->GetSwivelTarget() * M_PI / 180) * SwerveManager::Get(SwerveModule::Location::Front_Right)->GetWheelRate();
        double BR_C = cos(SwerveManager::Get(SwerveModule::Location::Front_Right)->GetSwivelTarget() * M_PI / 180) * SwerveManager::Get(SwerveModule::Location::Back_Right)->GetWheelRate();
        double FL_D = cos(SwerveManager::Get(SwerveModule::Location::Front_Left)->GetSwivelTarget() * M_PI / 180) * SwerveManager::Get(SwerveModule::Location::Front_Left)->GetWheelRate();
        double BL_D = cos(SwerveManager::Get(SwerveModule::Location::Back_Left)->GetSwivelTarget() * M_PI / 180) * SwerveManager::Get(SwerveModule::Location::Back_Left)->GetWheelRate();
    
        double A = (BR_A + BL_A) / 2;
        double B = (FL_B + FR_B) / 2;

        double C = (FR_C + BR_C) / 2;
        double D = (FL_D + BL_D) / 2;

        double R1 = (B - A) / Length;
        double R2 = (C - D) / Width;
        double R = (R1 + R2) / 2;

        double F1 = R * (Length / 2) + A;
        double F2 = -R * (Length / 2) + B;
        double F = (F1 + F2) / 2;

        double L1 = R * (Width / 2) + C;
        double L2 = -R * (Width / 2) + D;
        double L = (L1 + L2) / 2;

        double gyro = -RobotNav->GetConstAngle() * M_PI / 180;

        double temp = F * cos(gyro) + L * sin(gyro);
        L = -F * sin(gyro) + L * cos(gyro);
        F = temp;

        /*Pos->Y += L * Del_Time;
        Pos->X -= F * Del_Time;*/

        /*double ChangeX = 0;
        double ChangeY = 0;
        if(abs(L) != 0 && abs(F) != 0)
        {
            ChangeX = Pos->LastX - F;
            ChangeY = Pos->LastY - L;
            Pos->LastX = F;
            Pos->LastY = L;
        }*/
        Pos->Y += L * OutputTable->GetNumber("2A-Scale", 1);
        Pos->X -= F * OutputTable->GetNumber("2A-Scale", 1);
    }
    OutputTable->PutNumber("Loc-X", Pos->X);
    OutputTable->PutNumber("Loc-Y", Pos->Y);
    SimPos->X = OutputTable->GetNumber("BotX", 0);
    SimPos->Y = OutputTable->GetNumber("BotZ", 0);
}

//This just resets the robot location in code not in real life
void SwerveManager::ResetLoc()
{
    Pos->X = 0;
    Pos->Y = 0;
}

//This is how to set values to the swerve manager with rawV being the vertical axis, rawH being the horizontal axis, and rawS being the spin axis
void SwerveManager::Set(double rawV, double rawH, double rawS)
{
    double Average = abs((abs(rawV) + abs(rawH) + abs(rawS)) / 3);
    if(Average > 0)
    {
    double piVal = M_PI;

    double r = sqrt((Length * Length) + (Width * Width));

    double a = rawH - rawS * (Length / r);
    double b = rawH + rawS * (Length / r);
    double c = rawV - rawS * (Width / r);
    double d = rawV + rawS * (Width / r);

    double backRightSpeed = sqrt ((a * a) + (d * d));
    double backLeftSpeed = sqrt ((a * a) + (c * c));
    double frontRightSpeed = sqrt ((b * b) + (d * d));
    double frontLeftSpeed = sqrt ((b * b) + (c * c));

    double backRightAngle = (atan2 (a, d) * 180) / piVal;
    double backLeftAngle = (atan2 (a, c) * 180)  / piVal;
    double frontRightAngle = (atan2 (b, d) * 180)/ piVal;
    double frontLeftAngle = (atan2 (b, c) * 180) / piVal;
/*
    double MaxVal = frontLeftSpeed;
    MaxVal = SwerveManager::GetMax(MaxVal, frontRightSpeed);
    MaxVal = SwerveManager::GetMax(MaxVal, backLeftSpeed);
    MaxVal = SwerveManager::GetMax(MaxVal, backRightSpeed);

    if (MaxVal > MaxValParam)
    {
        backRightSpeed /= MaxVal;
        backLeftSpeed /= MaxVal;
        frontRightSpeed /= MaxVal;
        frontLeftSpeed /= MaxVal;
    }
*/
    backRightSpeed *= MaxValParam;
    backLeftSpeed *= MaxValParam;
    frontRightSpeed *= MaxValParam;
    frontLeftSpeed *= MaxValParam;

    bool SFL = SwerveManager::SetSwivelTargetAt(SwerveModule::Location::Front_Left, -frontLeftAngle);
    bool SFR = SwerveManager::SetSwivelTargetAt(SwerveModule::Location::Front_Right, -frontRightAngle);
    bool SBL = SwerveManager::SetSwivelTargetAt(SwerveModule::Location::Back_Left, -backLeftAngle);
    bool SBR = SwerveManager::SetSwivelTargetAt(SwerveModule::Location::Back_Right, -backRightAngle);

    if ((
         SFL &&
         SFR &&
         SBL &&
         SBR
         ) || !WaitSwivel)
    {
        SwerveManager::SetWheelSpeedAt(SwerveModule::Location::Front_Left, frontLeftSpeed);
        SwerveManager::SetWheelSpeedAt(SwerveModule::Location::Front_Right, -frontRightSpeed);
        SwerveManager::SetWheelSpeedAt(SwerveModule::Location::Back_Left, backLeftSpeed);
        SwerveManager::SetWheelSpeedAt(SwerveModule::Location::Back_Right, -backRightSpeed);

        /*
        SwerveManager::Get(SwerveModule::Location::Front_Left)->GetWheelMtr()->SetSpeed(frontLeftSpeed, Del_Time);
        SwerveManager::Get(SwerveModule::Location::Front_Right)->GetWheelMtr()->SetSpeed(-frontRightSpeed, Del_Time);
        SwerveManager::Get(SwerveModule::Location::Back_Left)->GetWheelMtr()->SetSpeed(backLeftSpeed, Del_Time);
        SwerveManager::Get(SwerveModule::Location::Back_Right)->GetWheelMtr()->SetSpeed(-backRightSpeed, Del_Time);
        
        SwerveManager::SetWheelAt(SwerveModule::Location::Front_Left, frontLeftSpeed);
        SwerveManager::SetWheelAt(SwerveModule::Location::Front_Right, frontRightSpeed);
        SwerveManager::SetWheelAt(SwerveModule::Location::Back_Left, backLeftSpeed);
        SwerveManager::SetWheelAt(SwerveModule::Location::Back_Right, backRightSpeed);
        */
    }
    else
    {
        SwerveManager::Set(0);
        Log::General("Waiting for swivel motors to get their shit together");
    }
    }
    else
    {
        SwerveManager::Set(0);
        SwerveManager::SetSwivel(0);
    }
}

//This updates all this things that need to be updated in the swerve manager and helps keep things seperate and not all the update code cramped in one space
void SwerveManager::UpdateSystem(double D_Time)
{
    SwerveManager::SetDelta(D_Time);
    SwerveManager::UpdateModules();
    SwerveManager::UpdateLoc();
    if(PrintOut)
        PrintLocation();
}

//Debug stuff
void SwerveManager::PrintLocation()
{
    Log::General("````````````````````````Swerve Manager: " + name + " Location:>> XPos:< " + to_string(Pos->X) + "  YPos:< " + to_string(Pos->Y));
}

/*
This gets a location (front left, back left, .ect) and sets the encoder linked in the config to that motor to go to the target which is the angle
using the PID given in the config or the default PID returns true when reached
*/
bool SwerveManager::SetSwivelTargetAt(SwerveModule::Location Loc, double Target)
{
    bool Finished = true;
    for(int i = 0; i < Modules.size(); i++)
    {
        if (Modules.at(i)->GetLocation() == Loc)
            Finished = Finished && Modules.at(i)->SetTargetSwivel(Target);
    }
    return Finished;
}

/*
This gets a location (front left, back left, .ect) and sets the encoder linked in the config to that motor to go to the target which is the distance
REMEMBER the encoder on the motor is not reset so you need to reset the encoders or the motor might go in the oposite direction to get to the distance
from zero which is ether when the robot first started or restarted if you didnt reset the encoders returns when reached
*/
bool SwerveManager::SetWheelTargetAt(SwerveModule::Location Loc, double Target)
{
    bool Finished = true;
    for(int i = 0; i < Modules.size(); i++)
    {
        if (Modules.at(i)->GetLocation() == Loc)
            Finished = Finished && Modules.at(i)->SetTargetWheel(Target);
    }
    return Finished;
}

//This sets the power percent of the swivel motor at the given location
void SwerveManager::SetSwivelAt(SwerveModule::Location Loc, double Power)
{
    for(int i = 0; i < Modules.size(); i++)
    {
        if (Modules.at(i)->GetLocation() == Loc)
            Modules.at(i)->SetSwivel(Power);
    }
}

//This sets the speed (RPM) of the wheel at a given location which is different from setting percent
void SwerveManager::SetWheelSpeedAt(SwerveModule::Location Loc, double Power)
{
    for(int i = 0; i < Modules.size(); i++)
    {
        if (Modules.at(i)->GetLocation() == Loc)
            Modules.at(i)->SetSpeedTarget(Power);
    }
}

//This sets wheel diameter for telemetry
void SwerveManager::SetWheelDiameter(double Diameter)
{
    OutputTable->PutNumber("1A-Wheel_Size", Diameter);
    for(int i = 0; i < Modules.size(); i++)
    {
        Modules.at(i)->SetWheelSize(Diameter);
    }
}

//This sets all the swivel motors in all the swerve modules in the swerve manager to a specific angle returns true when reached
bool SwerveManager::SetSwivelTarget(double Target)
{
    bool Finished = true;
    for(int i = 0; i < Modules.size(); i++)
    {
        Finished = Finished && Modules.at(i)->SetTargetSwivel(Target);
    }
    return Finished;
}

//This sets all the wheel motors in all the swerve modules in the swerve manager to a specific distance returns true when reached
bool SwerveManager::SetWheelTarget(double Target)
{
    bool Finished = true;
    for(int i = 0; i < Modules.size(); i++)
    {
        Finished = Finished && Modules.at(i)->SetTargetWheel(Target);
    }
    return Finished;
}

//This sets both the wheel distance and wheel angle in all the swerve modules and waits to move until all the swivel motors are at there target angle returns true when reached
bool SwerveManager::SetTarget(double Wheel_Target, double Swivel_Target)
{
    bool Can = SwerveManager::SetSwivelTarget(Swivel_Target);
    return  Can ? SwerveManager::SetWheelTarget(Wheel_Target) : false;
}

//This sets the delta time between each loop
void SwerveManager::SetDelta(double D_Time)
{
    Del_Time = D_Time;
    for(int i = 0; i < Modules.size(); i++)
    {
        Modules.at(i)->SetDeltaTime(D_Time);
    }
}

//This resets the PID position for both swivel and wheel motors in all the swerve modules in the swerve manager
void SwerveManager::ResetPID()
{
    for(int i = 0; i < Modules.size(); i++)
    {
        Modules.at(i)->ResetPID();
    }
}

//This resets the encoder linked to the swivel motor
void SwerveManager::ResetSwivelEnc()
{
    for(int i = 0; i < Modules.size(); i++)
    {
        Modules.at(i)->ResetSwivelEnc();
    }
}

//THis resets the encoder linked to the wheel motor
void SwerveManager::ResetWheelEnc()
{
    for(int i = 0; i < Modules.size(); i++)
    {
        Modules.at(i)->ResetWheelEnc();
    }
}

//This returns the average power of the wheel motors
double SwerveManager::Get()
{
    double Sum = 0;
    for(int i = 0; i < Modules.size(); i++)
    {
        Sum += Modules.at(i)->Get();
    }
    return Sum / Modules.size();
}

//This returns the list of the swerve modules in the swerve manager
vector<SwerveModule*> SwerveManager::GetModules()
{
    return Modules;
}

//This is not used for anything but needs to be stated because it is inheriting an abstract class
void SwerveManager::DefaultSet()
{
    Log::Error("WHY DID YOU CALL THE DEFAULT SET FOR A MOTOR?!? Yell at your programmers!");
}

//This is not used for anything but needs to be stated because it is inheriting an abstract class
void SwerveManager::Set(DoubleSolenoid::Value value)
{
    Log::Error("WHY DID YOU CALL THE DOUBLESOLENOID SET FOR A MOTOR?!? Yell at your programmers!");
}