/****************************** Header ******************************\
Class Name: VictorSPItem inherits OutputComponent
File Name:	VictorSPItem.cpp
Summary: Abstraction for the WPIlib VictorSP that extends to include
some helper and control methods.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Dylan Watson
Email: dylantrwatson@gmail.com
\********************************************************************/

#include <iostream>

#include "VictorSPItem.h"

using namespace std;
using namespace Components;

//Used to set power to motor
void VictorSPItem::Set(double val)
{
	val = CalculateVal(val);
	if((val<0 || val>0) && !inUse)
	{
		inUse = true;
		if(Reversed) victor->Set(-val);
		else victor->Set(val);
		inUse = false;
	}
	else if(!inUse)
	{
		inUse = true;
		victor->Set(0);
		inUse = false;
	}
}

//Stops the motor
void VictorSPItem::Stop()
{
	if(!inUse)
	{
		inUse = true;
		SetPercent(0);
		inUse = false;
	}
}

//Returns the current percent output
double VictorSPItem::Get()
{
	if(Reversed)
		return victor->Get() * -1;
	return victor->Get();
}

//Returns the name of the component
string VictorSPItem::GetName()
{
	return name;
}

//Returns the direction of the motor
int VictorSPItem::GetPolarity()
{
	if(Reversed) return -1;
	return 1;
}

/*
The delete component is just a way to clean up space (it was used for quickload so we dont get dups of objects)
*/
void VictorSPItem::DeleteComponent()
{
	CleanUpProfiles();
	delete victor;
	delete this;
}

//Method Called by the ActiveCollection
void VictorSPItem::UpdateComponent()
{
	if(PrintOut)
	{
		Log::General("`````````````````````Motor: " + name + "    Power: " + to_string(VictorSPItem::Get()) +  "   Position: " + to_string(GetEncoder()->Get()) + " Angle: " + to_string(GetAngle()));
	}
}

void VictorSPItem::DefaultSet()
{
	Log::Error("WHY DID YOU CALL THE DEFAULT SET FOR A MOTOR?!? Yell at your programmers!");
}

void VictorSPItem::Set(DoubleSolenoid::Value value)
{
	Log::Error("WHY DID YOU CALL THE DEFAULT SET FOR A MOTOR?!? Yell at your programmers!");
}
