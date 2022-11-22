/****************************** Header ******************************\
Class Name: DigitalInputItem inherits InputComponent
File Name:	DigitalInputItem.cpp
Summary: Abstraction for the WPIlib DigitalInput that extends to include
some helper and control methods.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Dylan Watson
Email: dylantrwatson@gmail.com
\********************************************************************/

#include "DigitalInputItem.h"

using namespace Components;

//Constructor used in config
DigitalInputItem::DigitalInputItem(int _channel, string name, bool Real) : InputComponent(name)
{
	channel = _channel;
	din = new DigitalInput(channel);
	FromTable(Real);
	if (UseTable)
	{
		Log::General("Using Table values");
		OutputTable->PutBoolean(name, false);
	}
}

/*
The delete component is just a way to clean up space (it was used for quickload so we dont get dups of objects)
*/
void DigitalInputItem::DeleteComponent()
{
	delete din;
	delete this;
}

//Returns the current value of the switch
double DigitalInputItem::Get()
{
	if (!UseTable)
		return din->Get();
	else
		return 	OutputTable->GetBoolean(name, false) ? 0 : 1;
}

//Returns the current value of the switch as a bool
bool DigitalInputItem::GetBool()
{
	return din->Get() == 0;
}

//Returns the name given in the config
string DigitalInputItem::GetName()
{
	return name;
}

//Method called in each loop
void DigitalInputItem::UpdateComponent()
{
	if (!UseTable)
	{
		OutputTable->PutBoolean(name, DigitalInputItem::GetBool());
	}
	if(PrintOut)
	{
		Log::General("````````````````````````````" + name + " value: " + to_string(Get()));
	}
}

DigitalInputItem::~DigitalInputItem() 
{
}

