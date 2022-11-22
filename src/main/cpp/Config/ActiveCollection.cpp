/****************************** Header ******************************\
Class Name: ActiveCollection
File Name:	ActiveCollection.cpp
Summary: Stores all Components on the robot controlled by the software.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ryan Cooper, Dylan Watson, Chris Weeks, Ian Poll
Email: cooper.ryan@centaurisoftware.co, dylantrwatson@gmail.com, chrisrweeks@aol.com, irobot9803@gmail.com
\********************************************************************/

#include <iostream>
#include <typeinfo>
#include <typeindex>

#include "ActiveCollection.h"
#include "../Controls/ControlItem.h"
#include "../Goals/Goal.h"

using namespace std;
using namespace Configuration;

/**
 * Default Constructor
**/
ActiveCollection::ActiveCollection()
{
	superior_Goal = new MultitaskGoal(this, true);

	Driver_Goal = new MultitaskGoal(this, true);
	Operator_Goal = new MultitaskGoal(this, true);
	Robot_Goal = new MultitaskGoal(this, true);
	
	superior_Goal->AddGoal(Driver_Goal);
	superior_Goal->AddGoal(Operator_Goal);
	superior_Goal->AddGoal(Robot_Goal);

	overdrive = false;
}

//Resets and adds to superior goal
void ActiveCollection::ResetSuperior_Goal()
{
	superior_Goal->~MultitaskGoal();

	superior_Goal = new MultitaskGoal(this, true);

	Driver_Goal = new MultitaskGoal(this, true);
	Operator_Goal = new MultitaskGoal(this, true);
	Robot_Goal = new MultitaskGoal(this, true);
	
	superior_Goal->AddGoal(Driver_Goal);
	superior_Goal->AddGoal(Operator_Goal);
	superior_Goal->AddGoal(Robot_Goal);

	superior_Goal->Activate();
}

void ActiveCollection::ProcessSuperior_Goal(double dTime)
{
	superior_Goal->Process(dTime);
}

/**
 * Method to return a NativeComponent of a certain type and name
**/

NativeComponent* ActiveCollection::Get(string name)
{
	NativeComponent *ret = nullptr;
	try{
		for(int i=0; i<(int)activeCollection.size();i++)
			if((*activeCollection[i]).name == (string)name)
				ret=activeCollection[i];
		if(!ret) throw "AHHH";
	}
	catch(...){
		Log::Error("Cannot find component " + name + ", it does not exist in the active collection!");
	}
	return ret;
}

/**
 * Method to return an OutputComponent of a certain type and name
**/

OutputComponent* ActiveCollection::GetOutput(string name)
{
	OutputComponent *ret = nullptr;
	try{
		for(int i=0; i<(int)activeCollection.size();i++)
			if((*activeCollection[i]).name == (string)name)
				ret=(OutputComponent*)activeCollection[i];
		if(!ret) throw "AHHH";
	}
	catch(...){
		Log::Error("Cannot find output component " + name + ", it does not exist in the active collection!");
	}
	return ret;
}

/**
 * Method to return a Motor of a certain name
**/

Motor* ActiveCollection::GetMotor(string name)
{
	Motor *ret = nullptr;
	try{
		for(int i=0; i<(int)activeCollection.size();i++){
			if((*activeCollection[i]).name == (string)name){
				ret=(Motor*)activeCollection[i];
			}
		}
		if (!ret) throw "AHHH";
	}
	catch(...){
		Log::Error("Cannot find Motor " + name + ", it does not exist in the active collection!");
	}
	return ret;
}

/**
 * Method to return a VictorSP of a certain name
**/
VictorSPItem* ActiveCollection::GetVictor(string name)
{
	VictorSPItem *ret = nullptr;
	try{
		for(int i=0; i<(int)activeCollection.size();i++){
			if((*activeCollection[i]).name == (string)name){
				ret=(VictorSPItem*)activeCollection[i];
			}
		}
		if (!ret) throw "AHHH";
	}
	catch(...){
		Log::Error("Cannot find victor " + name + ", it does not exist in the active collection!");
	}
	return ret;
}

/**
 * Method to return a TalonSRX of a certain name
**/
TalonSRXItem* ActiveCollection::GetTalon(string name)
{
	TalonSRXItem *ret = nullptr;
	try{
		for(int i=0; i<(int)activeCollection.size();i++){
			if((*activeCollection[i]).name == (string)name){
				ret=(TalonSRXItem*)activeCollection[i];
			}
		}
		if (!ret) throw "AHHH";
	}
	catch(...){
		Log::Error("Cannot find talon " + name + ", it does not exist in the active collection!");
	}
	return ret;
}

/**
 * Method to return all Solenoids
 **/
void ActiveCollection::DoubleSolenoidDefault()
{
	try
	{
		for(int i=0; i<(int)activeCollection.size();i++)
		{
			if(dynamic_cast<DoubleSolenoidItem*>(activeCollection[i]))
				((DoubleSolenoidItem*)activeCollection[i])->DefaultSet();
		}
	}
	catch(...){
		Log::Error("Cannot find DoubleSolenoids, it does not exist in the active collection!");
	}
}

/**
 * Method to return an Encoder of a certain name
**/
EncoderItem* ActiveCollection::GetEncoder(string name)
{
	EncoderItem *ret = nullptr;
	try{
		for(int i=0; i<(int)activeCollection.size();i++){
			if((*activeCollection[i]).name == (string)name){
				ret=(EncoderItem*)activeCollection[i];
			}
		}
		if (!ret) throw "AHHH";
	}
	catch(...){
		Log::Error("Cannot find encoder " + name + ", it does not exist in the active collection!");
	}
	return ret;
}

/**
 * Method to return a Servo of a certain name
**/
ServoItem* ActiveCollection::GetServo(string name)
{
	ServoItem *ret = nullptr;
	try{
		for(int i=0; i<(int)activeCollection.size();i++){
			if((*activeCollection[i]).name == (string)name){
				ret=(ServoItem*)activeCollection[i];
			}
		}
		if (!ret) throw "AHHH";
	}
	catch(...){
		Log::Error("Cannot find servo " + name + ", it does not exist in the active collection!");
	}
	return ret;
}

/**
 * Method to return the NavX
**/
NavX* ActiveCollection::GetNavX()
{
	NavX *ret = nullptr;
	try{
		for(int i=0; i<(int)activeCollection.size();i++){
			if((*activeCollection[i]).name == "NavX"){
				ret=(NavX*)activeCollection[i];
			}
		}
		if (!ret) throw "AHHH";
	}
	catch(...){
		Log::Error("Cannot find the NavX, check the config.");
	}
	return ret;
}

/**
 * Method to return the current size of the ActiveCollection list
**/
int ActiveCollection::GetSize(){
	return (int)activeCollection.size();
}

/**
 * Adds a component to the ActiveCollection
**/
void ActiveCollection::Add(NativeComponent *component){
	try{
		activeCollection.push_back(component);
	}
	catch(...){
		Log::Error("Cannot add component " + (*component).name);
	}
}

//Returns list of components
vector<NativeComponent*> ActiveCollection::GetRawComponent(){
	return activeCollection;
}

void ActiveCollection::AddEvent(Event *event){
	EventMap.push_back(event);
}

//Adds control item to driver list
void ActiveCollection::AddDriverControl(ControlItem* Control)
{
	m_driveControlCollection.push_back(Control);
}

//Adds control item to operator list
void ActiveCollection::AddOperatorControl(ControlItem* Control)
{
	m_operateControlCollection.push_back(Control);
}

/**
 * Methods to return a Control of a certain name
**/
ControlItem* ActiveCollection::GetDriverControl(string name)
{
	ControlItem *ret = nullptr;
	try{
		for(int i=0; i<(int)m_driveControlCollection.size();i++){
			if((*m_driveControlCollection[i]).name == (string)name){
				ret=(ControlItem*)m_driveControlCollection[i];
			}
		}
		if (!ret) throw "AHHH";
	}
	catch(...){
		Log::Error("Cannot find driver control " + name + ", it does not exist in the active collection!");
	}
	return ret;
}

ControlItem* ActiveCollection::GetOperatorControl(string name)
{
	ControlItem *ret = nullptr;
	try{
		for(int i=0; i<(int)m_operateControlCollection.size();i++){
			if((*m_operateControlCollection[i]).name == (string)name){
				ret=(ControlItem*)m_operateControlCollection[i];
			}
		}
		if (!ret) throw "AHHH";
	}
	catch(...){
		Log::Error("Cannot find operator control " + name + ", it does not exist in the active collection!");
	}
	return ret;
}

ControlItem* ActiveCollection::GetControl(string Name)
{
	ControlItem* Driver = ActiveCollection::GetDriverControl(Name);
	ControlItem* Operator = ActiveCollection::GetOperatorControl(Name);
	if(Driver != nullptr)
		return Driver;
	if(Operator != nullptr)
		return Operator;
	return nullptr;
}

/**
 * Methods to check a Control of a certain name exists
**/

bool ActiveCollection::HasDriverControl(string Name)
{
	return ActiveCollection::GetDriverControl(Name) != nullptr;
}

bool ActiveCollection::HasOperatorControl(string Name)
{
	return ActiveCollection::GetOperatorControl(Name) != nullptr;
}

bool ActiveCollection::HasControl(string Name)
{
	return ActiveCollection::GetControl(Name) != nullptr;
}

//Deletes everything it can for quickload
void ActiveCollection::DeleteAll()
{
	for(int i = 0; i < activeCollection.size(); i++)
	{
		activeCollection[i]->DeleteComponent();
	}
	activeCollection.clear();
	for(int i = 0; i < Profiles.size(); i++)
	{
		delete Profiles[i];
	}
	Profiles.clear();

	for (int i = 0; i < (int)m_driveControlCollection.size(); i++)
		m_driveControlCollection[i]->DeleteComponent();
	
	for (int i = 0; i < (int)m_operateControlCollection.size(); i++)
		m_operateControlCollection[i]->DeleteComponent();

	m_driveControlCollection.clear();
	m_operateControlCollection.clear();
}

//Updates and sets delta time for all components
void ActiveCollection::UpdateComponents(double D_Time)
{
	for(int i = 0; i < activeCollection.size(); i++)
	{
		activeCollection[i]->SetD_Time(D_Time);
		activeCollection[i]->UpdateComponent();
	}
}

//Method to update driver controls
void ActiveCollection::UpdateDriver(double D_Time)
{
	for (int i = 0; i < (int)m_driveControlCollection.size(); i++)
			if((*m_driveControlCollection[i]).IsEnabled())
			{
				//Log::General("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Updating Control: " + (*m_driveControlCollection[i]).name);
				(*m_driveControlCollection[i]).UpdateInput((*m_driveControlCollection[i]).Update(D_Time));
			}
			/*else
				Log::General("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX_Control " + (*m_driveControlCollection[i]).name + " is DISABLED");*/
}

//Method to update operator controls
void ActiveCollection::UpdateOperator(double D_Time)
{
	for (int i = 0; i < (int)m_operateControlCollection.size(); i++)
			if((*m_operateControlCollection[i]).IsEnabled())
			{
				//Log::General("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Updating Control: " + (*m_operateControlCollection[i]).name);
				(*m_operateControlCollection[i]).UpdateInput((*m_operateControlCollection[i]).Update(D_Time));
			}
			/*else
				Log::General("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX_Control " + (*m_operateControlCollection[i]).name + " is DISABLED");*/
}

//Disables all controls except for control names in the list given
void ActiveCollection::DisableAllControlsExcept(vector<string> Names)
{
	for (int i = 0; i < (int)m_operateControlCollection.size(); i++)
	{
		bool dis = true;
		for (int j = 0; j < (int)Names.size(); j++)
		{
			if(m_operateControlCollection[i]->name.compare(Names[j]))
			{
				dis = false;
			}
		}
		if(dis)
			m_operateControlCollection[i]->Disable();
	}
	for (int i = 0; i < (int)m_driveControlCollection.size(); i++)
	{
		bool dis = true;
		for (int j = 0; j < (int)Names.size(); j++)
		{
			if(m_driveControlCollection[i]->name.compare(Names[j]))
			{
				dis = false;
			}
		}
		if(dis)
			m_driveControlCollection[i]->Disable();
	}
}

//Enables all controls except for control names in the list given
void ActiveCollection::EnableAllControlsExcept(vector<string> Names)
{
	for (int i = 0; i < (int)m_operateControlCollection.size(); i++)
	{
		bool dis = true;
		for (int j = 0; j < (int)Names.size(); j++)
		{
			if(m_operateControlCollection[i]->name.compare(Names[j]))
			{
				dis = false;
			}
		}
		if(dis)
			m_operateControlCollection[i]->Enable();
	}
	for (int i = 0; i < (int)m_driveControlCollection.size(); i++)
	{
		bool dis = true;
		for (int j = 0; j < (int)Names.size(); j++)
		{
			if(m_driveControlCollection[i]->name.compare(Names[j]))
			{
				dis = false;
			}
		}
		if(dis)
			m_driveControlCollection[i]->Enable();
	}
}

//Creates and adds a PID object and profile data to use later if needed
int ActiveCollection::CreateAndAddProfile(string Name, double P, double I, double D, double MaxChange, double Bias, double Min, double Max, double InnerMin, double InnerMax, double Thres, int index, bool Print)
{
	ProfileData* Data = new ProfileData(P, I, D, MaxChange, (Bias == 0 ? P * 100 : Bias), InnerMin, InnerMax, Min, Max, Thres, Name, Print);
	Log::General("Added Profile: " + Name);
	ActiveCollection::AddPIDProfile(Data, Name);
	return ActiveCollection::AddProfile(Data, index);
}

//Adds a profile data to use later if needed
int ActiveCollection::AddProfile(ProfileData* Data, int index)
{
	if(index < 0)
	{
		Profiles.push_back(Data);
		return Profiles.size() - 1;
	}
	else
	{
		if(index >= Profiles.size())
		{
			int sizeDist = (index - Profiles.size()) + 1;
			for(int i = 0; i < sizeDist; i++)
			{
				ProfileData* NewData = new ProfileData();
				Profiles.push_back(NewData);
			}
		}
		delete Profiles[index];
		Profiles[index] = Data;
		return index;
	}
}

//Gets data at an index that was specified in the config or the index at the end of the list at the time it was made
ProfileData* ActiveCollection::GetProfile(int i)
{
	if(i >= Profiles.size())
	{
		Log::Error("Cannot find Profile with index: " + to_string(i) + "----Returning Default");
		return DefaultData;
	}
	else
	{
		return Profiles[i];
	}
}

//Get data with a specific name
ProfileData* ActiveCollection::GetProfile(string Name)
{
	for(int i = 0; i < Profiles.size(); i++)
	{
		if(Profiles[i]->Name.compare(Name) == 0)
		{
			return Profiles[i];
		}
	}
	Log::Error("Cannot find Profile with Name: " + Name + "----Returning Default");
	return DefaultData;
}

//Adds a new config data variable to use in code
void ActiveCollection::AddConfigData(string Name, string StringData, double DoubleData, bool BoolData)
{
	Variables.push_back(new ConfigVariable(Name, StringData, DoubleData, BoolData));
}

//Gets config data from name stated in config
ConfigVariable* ActiveCollection::GetConfigVariable(string Name)
{
	ConfigVariable* Ret = nullptr;
	for (int i = 0; i < (int)Variables.size(); i++)
	{
		if(Variables[i]->GetName().compare(Name) == 0)
		{
			Ret = Variables[i];
		}
	}
	if(Ret == nullptr)
	{
		Log::Error("Config Variable " + Name + " doesn't exist, Avalable variables are:");
		for (int i = 0; i < (int)Variables.size(); i++)
		{
			Log::Error(Variables[i]->GetName());
		}
	}
	return Ret;
}

//Checks if a config data with name exists
bool ActiveCollection::HasConfigVariable(string Name)
{
	return GetConfigVariable(Name) != nullptr;
}