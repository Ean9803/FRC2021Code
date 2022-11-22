/****************************** Header ******************************\
Class Name: VictorSPXItem inherits OutputComponent
File Name:	VictorSPXItem.cpp
Summary: Abstraction for the VictorSPX
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ian Poll
Email: irobot983@gmail.com
\********************************************************************/

#include <iostream>

#include "VictorSPXItem.h"

using namespace std;
using namespace frc;
using namespace Components;

VictorSPXItem::VictorSPXItem() {}
//Constructor used by config
VictorSPXItem::VictorSPXItem(int _channel, string _name, bool _reversed)
	: Motor(_name, _reversed){
	channel = _channel;
	reversed = _reversed;
	victor = new VictorSPX(channel);
	victor->ConfigSelectedFeedbackSensor(FeedbackDevice::QuadEncoder, 0, 10);
}

//Returns the current percent output
double VictorSPXItem::Get(){
    if (reversed)
        return victor->GetMotorOutputPercent() * -1;
    return victor->GetMotorOutputPercent();
}

//Sets the motor power percent
void VictorSPXItem::Set(double val){
	val = CalculateVal(val);
	if((val<0 || val>0) && !inUse)
	{
		inUse = true;
		if(reversed) victor->Set(ControlMode::PercentOutput, -val);
		else victor->Set(ControlMode::PercentOutput, val);
		inUse = false;
	}
	else if(!inUse){
		inUse = true;
		victor->Set(ControlMode::PercentOutput, 0);
		inUse = false;
	}
}

void VictorSPXItem::DefaultSet(){
	Log::Error("WHY DID YOU CALL THE DEFAULT SET FOR A MOTOR?!? Yell at your programmers!");
}

void VictorSPXItem::Set(DoubleSolenoid::Value value){
	Log::Error("WHY DID YOU CALL THE DEFAULT SET FOR A MOTOR?!? Yell at your programmers!");
}

//Stops the Motor
void VictorSPXItem::Stop()
{
	SetPercent(0);
}

/*
The delete component is just a way to clean up space (it was used for quickload so we dont get dups of objects)
*/
void VictorSPXItem::DeleteComponent()
{
	CleanUpProfiles();
	delete victor;
	delete this;
}

//Method called by the ActiveCollection
void VictorSPXItem::UpdateComponent()
{
	if(PrintOut)
	{
		Log::General("`````````````````````Motor: " + name + "    Power: " + to_string(VictorSPXItem::Get()) +  "   Position: " + to_string(GetEncoder()->Get()) + " Angle: " + to_string(GetAngle()));
	}
}

VictorSPXItem::~VictorSPXItem() {}
