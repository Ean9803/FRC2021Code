/****************************** Header ******************************\
Class Name: PotentiometerItem inherits InputComponent
File Name:	PotentiometerItem.cpp
Summary: Abstraction for the WPIlib AnalogPotentiometer that extends to include
some helper and control methods.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Dylan Watson
Email: dylantrwatson@gmail.com
\********************************************************************/

#include "PotentiometerItem.h"

using namespace Components;

PotentiometerItem::PotentiometerItem() {}
//Constructor used in config
PotentiometerItem::PotentiometerItem(int _channel, string _name, bool Real)
	: InputComponent(_name){
	channel = _channel;
	apt = new AnalogPotentiometer(channel);
	initPosition = OutputTable->GetNumber(_name + "-int", apt->Get());
	FromTable(Real);
	{
		Log::General("Using Table values");
		OutputTable->PutNumber(_name, 0);
		OutputTable->PutNumber(_name + "-int", initPosition);
	}
}

//Return the name of the component
string PotentiometerItem::GetName(){
	return name;
}

//Return the value releative from the start position
double PotentiometerItem::Get(){
	return (UseTable ? OutputTable->GetNumber(name, 0) : apt->Get() - initPosition); //init position it subtracted to return the delta from startup position.
}

/*
The delete component is just a way to clean up space (it was used for quickload so we dont get dups of objects)
*/
void PotentiometerItem::DeleteComponent()
{
	delete apt;
	delete this;
}

//Method to update component
void PotentiometerItem::UpdateComponent()
{
	if (!UseTable)
	{
		OutputTable->PutNumber(name + "-Encoder", PotentiometerItem::Get());
	}
	if(PrintOut)
	{
		Log::General("````````````````````````````" + name + " value: " + to_string(Get()));
	}
}

//Returns the angle of the pot relative from the start position
double PotentiometerItem::GetAngle()
{
	return (fmod(Get(), ValAngle) / ValAngle) * 360;
}

//Returns true if the angle the pot is at is within the threshold from the given angle
bool PotentiometerItem::WithinAngle(double Angle)
{
	return Inrange(Angle, GetAngle(), Threshold);
}

PotentiometerItem::~PotentiometerItem() {}