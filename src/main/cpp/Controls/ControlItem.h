/****************************** Header ******************************\
Class Name:	ControlItem
File Name:	ControlItem.h
Summary:	Abstraction for managing all driver and operator controls
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Dylan Watson, Ian Poll
Email:	dylantrwatson@gmail.com, irobot9803@gmail.com
\*********************************************************************/

#pragma once

#include <frc/WPILib.h>
#include <vector>

#include "../Util/Constants.h" 
#include "../Components/OutputComponent.h"
#include "../Global.h"
#include "../Config/ActiveCollection.h"
#include "../Util/SmartDashboard.h"
#include "../Components/Swerve/SwerveManager.h"

using namespace frc;
using namespace std;
using namespace Util;
using namespace Components;
using namespace Configuration;

namespace Controls
{
	class ControlItem 
	{
		protected:
			bool reversed;
			double powerMultiplier;
			vector<OutputComponent*> components;
			bool Enabled = true;
			bool PrintOut = false;
			vector<double> Inputs {};
			vector<double> CurrentIn;

			//Used to get the button values at an index
			vector<double> GetButton(vector<int> index, double Default = 0)
			{
				CurrentIn.clear();
				vector<double> Return;
				for (int i = 0; i < (int)index.size(); i++)
				{
					Return.push_back(joy->GetRawButton(index[i]) ? 1 : 0);
				}
				vector<double> DefV((int)index.size(), Default);
				return CurrentIn = (IsEnabled() ? Return : (0 < (int)Inputs.size() ? Inputs : DefV));
			}

			//Used to get the axis values at an index
			vector<double> GetAxis(vector<int> index, double Default = 0)
			{
				CurrentIn.clear();
				vector<double> Return;
				for (int i = 0; i < (int)index.size(); i++)
				{
					Return.push_back(joy->GetRawAxis(index[i]));
				}
				vector<double> DefV((int)index.size(), Default);
				return CurrentIn = (IsEnabled() ? Return : (0 < (int)Inputs.size() ? Inputs : DefV));
			}

			//Used to get the d_pad values
			vector<double> GetD_Pad(double Default = -1)
			{
				CurrentIn.clear();
				return CurrentIn = (IsEnabled() ? vector<double> {(double)joy->GetPOV(0)} : (0 < (int)Inputs.size() ? Inputs : vector<double> {Default}));
			}

		public:

			enum JoystickHolder
			{
				DriverController = 0,
				OperatorController = 1,
				Other = 2
			};

			JoystickHolder Holder;

			ActiveCollection* m_activeCollection;
			Joystick *joy;
			ControlItem();
			ControlItem(Joystick *_joy, string _name, bool _reversed, double _powerMultiplier, ActiveCollection* activeCollection);
			virtual double Update(double _dTime) = 0;
			void UpdateInput(double Input) { LastInput = Input; };
			double GetInput() { return LastInput; };
			void AddComponent(OutputComponent *component);
			vector<string> GetComponents();
			string name;
			Event ValueChanged;
			void SetToComponents(double val, int Mode = 0);
			virtual void DeleteComponent();
			//This activates the control to be used
			void Enable() {Enabled = true;}
			//This deactivates the control to not be used
			void Disable() {Enabled = false;}
			bool IsEnabled() {return Enabled;}
			void SetOutprint(bool Print) { PrintOut = Print; };
			vector<double> CurrentInput() { return CurrentIn; };
			//This disables the control and mimics the controller input which is useful for auto
			void FeedIn(double DTime, vector<double> Input) { Inputs = Input; Disable(); UpdateInput(Update(DTime));};
			virtual ~ControlItem();

		private:
			double LastInput = 0;
	};
}