/****************************** Header ******************************\
Class Name: SparkMax inherits OutputComponent
File Name:	SparkMaxItem.cpp
Summary: Abstraction for the SparkMax
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ian Poll, Shruti Venkatramanan, Guadalupe Rodriguez, Emily Martinez
Email: irobot983@gmail.com
\********************************************************************/

#include <iostream>

#include "SparkMaxItem.h"

using namespace std;
using namespace frc;
using namespace Components;

SparkMaxItem::SparkMaxItem() {}
//Constructor used by config
SparkMaxItem::SparkMaxItem(int _channel, string _name, bool _reversed, bool Real) : Motor(_name, _reversed){
	channel = _channel;
	reversed = _reversed;
	Max = new CANSparkMax(channel, rev::CANSparkMax::MotorType::kBrushless);
	Max->SetIdleMode(rev::CANSparkMax::IdleMode::kBrake);
	Name = _name;
	Offset = OutputTable->GetNumber(name + "-Encoder", 0);
	FromTable(Real);
	{
		Log::General("Using Table values");
		OutputTable->PutNumber(name + "-Encoder", 0);
		OutputTable->PutBoolean(name + "-Reset", true);
	}
}

//Returns the current spark max output percent
double SparkMaxItem::Get(){
    return Max->Get();
}

//Returns the value of the encoder on the spark max or the linked encoder if one is set in config
double SparkMaxItem:: GetEncoderValue(){
	return (UseTable ? OutputTable->GetNumber(name + "-Encoder", 0) : (Max->GetEncoder(rev::CANEncoder::EncoderType::kHallSensor, EncTicks).GetPosition() - Offset));
}

//Resets the encoder on the spark max
void SparkMaxItem::Reset(){
	Offset = Max->GetEncoder(rev::CANEncoder::EncoderType::kHallSensor, EncTicks).GetPosition();
	if (UseTable)
		OutputTable->PutBoolean(name + "-Reset", true);
}

//Get the name of the component
string SparkMaxItem::GetName(){
	return Name;
}

//Returns the direction of the motor when set to a postive value
int SparkMaxItem::GetPolarity(){
	return (reversed? -1 : 1);
}

//Sets the motor power
void SparkMaxItem::Set(double val){
	val = CalculateVal(val);
	// Log::General(Name+" : "  + to_string(val));
	if((val<0 || val>0) && !inUse)
	{
		inUse = true;
		if(reversed) Max->Set(-val);
		else Max->Set(val);
		inUse = false;
	}
	else if(!inUse)
	{
		inUse = true;
		Max->StopMotor();
		inUse = false;
	}
}

//Stops the motor
void SparkMaxItem::Stop(){
	if(!inUse)
	{
		inUse = true;
		SetPercent(0);
		inUse = false;
	}
}

/*
The delete component is just a way to clean up space (it was used for quickload so we dont get dups of objects)
*/
void SparkMaxItem::DeleteComponent()
{
	CleanUpProfiles();
	delete Max;
	delete this;
}

//Method that is called by the ActiveCollection
void SparkMaxItem::UpdateComponent()
{
	if (!UseTable)
	{
		OutputTable->PutNumber(name + "-Encoder", SparkMaxItem::GetEncoderValue());
	}
	if(PrintOut)
	{
		Log::General("`````````````````````Motor: " + name + "    Power: " + to_string(SparkMaxItem::Get()) + "   Position: " + to_string(SparkMaxItem::GetEncoderValue()) + " Angle: " + to_string(GetAngle()));
	}
}

//Resets the encoder
void SparkMaxItem::ResetEncoderValue()
{
	Max->GetEncoder(rev::CANEncoder::EncoderType::kHallSensor, 24).SetPosition(0);
}

void SparkMaxItem::DefaultSet(){
	Log::Error("WHY DID YOU CALL THE DEFAULT SET FOR A MOTOR?!? Yell at your programmers! Retard (or don't, its really up to you)");
}

void SparkMaxItem::Set(DoubleSolenoid::Value value){
	Log::Error("WHY DID YOU CALL THE DEFAULT SET FOR A MOTOR?!? Yell at your programmers! Retard (or don't, its really up to you)");
}

SparkMaxItem::~SparkMaxItem() {}
