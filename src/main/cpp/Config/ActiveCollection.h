/****************************** Header ******************************\
Class Name: ActiveCollection
File Name:	ActiveCollection.h
Summary: Stores all Components on the robot controlled by the software.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ryan Cooper, Dylan Watson, Chris Weeks, Ian Poll
Email: cooper.ryan@centaurisoftware.co, dylantrwatson@gmail.com, chrisrweeks@aol.com, irobot9803@gmail.com
\********************************************************************/

#pragma once

#ifndef SRC_CONFIG_ACTIVECOLLECTION_H_
#define SRC_CONFIG_ACTIVECOLLECTION_H_

#include <vector>
#include <utility>

#include "../Components/VictorSPItem.h"
#include "../Components/VictorSPXItem.h"
#include "../Components/TalonSRXItem.h"
#include "../Components/EncoderItem.h"
#include "../Components/PotentiometerItem.h"
#include "../Components/DoubleSolenoidItem.h"
#include "../Components/DigitalInputItem.h"
#include "../Components/NavX.h"
#include "../Components/SparkMaxItem.h"
#include "../Components/PDBManager.h"
#include "../Components/ServoItem.h"


namespace Controls
{
	class ControlItem;
};

class MultitaskGoal;

using namespace std;
using namespace Components;
using namespace Controls;

namespace Configuration
{
	class ConfigVariable
	{
		public:
			ConfigVariable(string Name, string StringData, double DoubleData, bool BoolData)
			{
				this->Name = Name;
				this->StringData = StringData;
				this->DoubleData = DoubleData;
				this->BoolData = BoolData;
			}

			string GetName()
			{
				return Name;
			}

			string StringValue()
			{
				return StringData;
			}

			double DoubleValue()
			{
				return DoubleData;
			}

			bool BoolValue()
			{
				return BoolData;
			}
		private:
			string Name;
			string StringData;
			double DoubleData;
			bool BoolData;
	};

	class ActiveCollection
	{
		public:
			enum ComponentType
			{
				VictorSP
			};

			ActiveCollection();
			virtual ~ActiveCollection(){}

			NativeComponent* Get(string name);
			OutputComponent* GetOutput(string name);
			Motor* GetMotor(string name);
			VictorSPItem* GetVictor(string name);
			TalonSRXItem* GetTalon(string name);
			EncoderItem* GetEncoder(string name);
			ServoItem* GetServo(string name);
			void DoubleSolenoidDefault();
			NavX* GetNavX();
			int GetSize();
			vector<NativeComponent*> GetRawComponent();
			void Add(NativeComponent *component);
			void AddEvent(Event *event);
			vector<Event*> EventMap;

			vector<string> GetAutoControls() {return AutoControls;}
			void AddAutoControl(string ControlName) {AutoControls.push_back(ControlName);}

			vector<string> GetDeadControls() {return DeadControls;}
			void AddDeadControl(string ControlName) {DeadControls.push_back(ControlName);}

			void AddDriverControl(ControlItem* control);
			void AddOperatorControl(ControlItem* control);

			void UpdateDriver(double D_Time);
			void UpdateOperator(double D_Time);

			ControlItem* GetDriverControl(string name);
			ControlItem* GetOperatorControl(string name);
			ControlItem* GetControl(string Name);
			bool HasDriverControl(string Name);
			bool HasOperatorControl(string Name);
			bool HasControl(string Name);

			void DisableAllControlsExcept(vector<string> Names);
			void EnableAllControlsExcept(vector<string> Names);

			void DeleteAll();

			void ResetSuperior_Goal();

			void ProcessSuperior_Goal(double dTime);

			void SetDriverGoal(MultitaskGoal* g) {Driver_Goal = g;}
			MultitaskGoal* GetDriverGoal() {return Driver_Goal;}

			void SetOperatorGoal(MultitaskGoal* g) {Operator_Goal = g;}
			MultitaskGoal* GetOperatorGoal() {return Operator_Goal;}

			void SetRobotGoal(MultitaskGoal* g) {Robot_Goal = g;}
			MultitaskGoal* GetRobotGoal() {return Robot_Goal;}

			void SetLoopTime(double Time){WaitTime = Time;}
			double GetWaitTime() {return WaitTime;}

			int CreateAndAddProfile(string Name, double P, double I, double D, double MaxChange = 0.1, double Bias = -1, double Min = -1, double Max = 1, double InnerMin = 0, double InnerMax = 0, double Thres = 0.01, int index = -1, bool Print = false);
			int AddProfile(ProfileData* Data, int index = -1);
			ProfileData* GetProfile(int i);
			ProfileData* GetProfile(string Name);

			void SetOverdrive(bool o) {overdrive = o;}
			bool GetOverdrive() {return overdrive;}

			void SetAuto(string Auto) {AutoSele = Auto;}
			void SetAutoOverride(bool Override) {AutoOverride = Override;}
			string GetAuto() {return AutoSele;}
			bool ConfigOverride() {return AutoOverride;}
			void SetAutoScale(double Scale) {AutoScale = Scale;}
			double GetAutoScale() {return AutoScale;}

			void SetIsSwerveDrive(bool Yes) {SwerveDrive = Yes;}
			bool IsSwerveDrive() {return SwerveDrive;}

			void SetPDP(double TimeOut, double CurrentThres, double Lower, bool run) {PDP = new PDBManager(TimeOut, CurrentThres, Lower, run);}
			PDBManager* GetPDBManager() {return PDP;}

			void UpdateComponents(double D_Time);

			void AddConfigData(string Name, string StringData, double DoubleData, bool BoolData);
			ConfigVariable* GetConfigVariable(string Name);
			bool HasConfigVariable(string Name);
			
			//Adds a new PID object to the list with new name
			void AddPIDProfile(ProfileData* Data, string Name)
			{
				PIDProfile* Add = new PIDProfile(Data);
				Add->SetName(Name);
				PIDs.push_back(Add);
			};

			//Returns PID object of name
			PIDProfile* GetPIDProfile(string Name)
			{
				PIDProfile* Ret = nullptr;
				for(int i = 0; i < (int)PIDs.size(); i++)
				{
					if(PIDs.at(i)->GetName().compare(Name) == 0)
					{
						Ret = PIDs.at(i);
					}
				}
				if(Ret == nullptr)
				{
					Log::Error("Cannot get PID: " + Name);
					Log::Error("Avalable PIDs:");
					for (int i = 0; i < (int)PIDs.size(); i++)
					{
						Log::Error(PIDs[i]->GetName());
					}
				}
				return Ret;
			};

			//Checks if a PID object of name exists
			bool HasPIDProfile(string Name)
			{
				return GetPIDProfile(Name) != nullptr;
			}

	private:
		//List that contains all output and input components
		vector<NativeComponent*> activeCollection;

		//Goal that contains the three main subgoals
		MultitaskGoal* superior_Goal = nullptr;

		//Main subgoals
		MultitaskGoal* Driver_Goal = nullptr;
		MultitaskGoal* Operator_Goal = nullptr;
		MultitaskGoal* Robot_Goal = nullptr;

		//List that contains profile data, not PID objects
		vector<ProfileData*> Profiles;
		ProfileData* DefaultData = new ProfileData();
		
		//List that contains PID objects
		vector<PIDProfile*> PIDs;

		bool overdrive;
		bool AutoOverride = false;
		string AutoSele = "????";
		double AutoScale = 1;
		bool SwerveDrive = false;

		PDBManager* PDP = nullptr;

		double WaitTime = 0.01;

		vector<ControlItem*> m_driveControlCollection;
		vector<ControlItem*> m_operateControlCollection;

		//List of control names that are enabled durring auto (good for combo controls)
		vector<string> AutoControls;

		//List of control names that are disabled durring teleop
		vector<string> DeadControls;

		//List of data stated in the config
		vector<ConfigVariable*> Variables;
	};
}

#endif /* SRC_CONFIG_ACTIVECOLLECTION_H_ */
