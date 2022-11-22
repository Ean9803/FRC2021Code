/****************************** Header ******************************\
Class Name: EncoderItem inherits InputComponent
File Name:	EncoderItem.cpp
Summary: Abstraction for the WPIlib Encoder that extends to include
some helper and control methods.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Dylan Watson, Ian Poll
Email: dylantrwatson@gmail.com, irobot9803@gmail.com
\********************************************************************/

#include "EncoderItem.h"
using namespace Components;

EncoderItem::EncoderItem() {}

//Constructor used in config
EncoderItem::EncoderItem(string _name, int _aChannel, int _bChannel, bool _reversed, bool Real)
	: InputComponent(_name){
	aChannel = _aChannel;
	bChannel = _bChannel;
	reversed = _reversed;
	encoder = new Encoder(aChannel, bChannel, reversed);
	FromTable(Real);
	Offset = OutputTable->GetNumber(name, 0);
	{
		Log::General("Using Table values");
		OutputTable->PutNumber(name, 0);
		OutputTable->PutBoolean(name + "-Reset", true);
	}
	Type = InputType::Independent;
}

//Constructor used in config with linked component
EncoderItem::EncoderItem(string _name, NativeComponent *Connected) : InputComponent(_name)
{
	Type = InputType::Data_Driven;
	LinkedComponent = Connected;
}

//Resets the value of the encoder
void EncoderItem::Reset()
{
	if(Type == InputType::Independent)
	{
		encoder->Reset();
		OutputTable->PutBoolean(name + "-Reset", true);
	}
	else
	{
		if(LinkedComponent != nullptr)
			LinkedComponent->ResetData();
		else
			Log::Error("Encoder " + name + " tracking nullptr!");
	}
}

//Gets the current value of the encoder
double EncoderItem::Get()
{
	if(Type == InputType::Independent)
	{
		double input = (UseTable ? OutputTable->GetNumber(name, 0) : (double)encoder->Get());
		return input - Offset;
	}
	else
	{
		if(LinkedComponent != nullptr)
			return LinkedComponent->GetData();
		else
		{
			Log::Error("Encoder " + name + " tracking nullptr!");
			return 0;
		}
	}
}

//Returns the name of the encoder stated in the config
string EncoderItem::GetName()
{
	return name;
}

/*
The delete component is just a way to clean up space (it was used for quickload so we dont get dups of objects)
*/
void EncoderItem::DeleteComponent()
{
	delete encoder;
	delete this;
}

//Returns the angle of the encoder (-360 to 360 when ticks per rev is set correctly)
double EncoderItem::GetAngle()
{
	return (fmod(Get(), TicksPer) / TicksPer) * 360;
}

//Returns if the current angle is near the given angle in a certain range or threshold
bool EncoderItem::WithinAngle(double Angle)
{
	return Inrange(Angle, GetAngle(), Threshold);
}

//Returns the distance the encoder has moved from the last reset
double EncoderItem::GetDistance(double WheelRadius)
{
	double Percent = Get() / TicksPer;
	return Percent * (WheelRadius * 2 * 3.1415);
}

//Updates the NT if needed
void EncoderItem::UpdateComponent()
{
	if (!UseTable && Type == InputType::Independent)
	{
		OutputTable->PutNumber(name, EncoderItem::Get());
	}
	if(PrintOut)
	{
		Log::General("````````````````````````````" + name + " value: " + to_string(Get()));
	}
}

EncoderItem::~EncoderItem() {}