/****************************** Header ******************************\
Class Name:	ControlItem
File Name:	ControlItem.cpp
Summary:	Abstraction for managing all driver and operator controls
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Dylan Watson, Ian Poll
Email:	dylantrwatson@gmail.com, irobot9803@gmail.com
\*********************************************************************/

#include <iostream>

#include "ControlItem.h"
#include "GoalButtonControl.h"
#include "GoalAxisControl.h"
#include "POVGoalControl.h"
#include "ComboControl.h"
#include "SwerveControl.h"
#include "../Goals/GoalSelector.h"
#include "../Goals/FRC_Goals.h"

using namespace std;
using namespace Controls;

#pragma region Event Handlers

auto onControllerValueChanged = [&](EventArgs* e) {
	try{
		if (e->Type == 0)
		{
			auto args = (TEventArgs<double, ControlItem*>*)e;
			//Log::General(" EVENT "+args->GetSender()->name);

			args->GetSender()->SetToComponents(args->GetValue(), (int)args->GetMode());
			SmartDashboard::PutNumber(args->GetSender()->name, args->GetValue());
		}
		else if (e->Type == 1)
		{
			auto SwerveArgs = (IEventArgs<double, double, double, ControlItem*>*)e;
			//Log::General(" EVENT "+SwerveArgs->GetSender()->name);

			SwerveControl *DT = (SwerveControl*)(SwerveArgs->GetSender());
			DT->GetManager()->Set(SwerveArgs->GetVValue(), SwerveArgs->GetHValue(), SwerveArgs->GetSValue());

			SmartDashboard::PutNumber(SwerveArgs->GetSender()->name, SwerveArgs->GetVValue());
		}
		else if (e->Type == 2)
		{
			auto args = (LEventArgs<vector<double>, vector<string>, ControlItem*>*)e;
			//Log::General(" EVENT "+args->GetSender()->name);
			MultitaskGoal* Selected = nullptr;

			if(args->GetSender()->Holder == ControlItem::JoystickHolder::DriverController)
			{
				Selected = args->GetSender()->m_activeCollection->GetDriverGoal();
			}
			else if(args->GetSender()->Holder == ControlItem::JoystickHolder::OperatorController)
			{
				Selected = args->GetSender()->m_activeCollection->GetOperatorGoal();
			}
			else
			{
				Selected = args->GetSender()->m_activeCollection->GetRobotGoal();
			}

			//If the sender is a GoalAxisControl
			if (dynamic_cast<GoalAxisControl*>(args->GetSender()))
			{
				Goal* ControlGoal = Selected->GetGoal(abs(((GoalAxisControl*)(args->GetSender()))->IdKey));
				if(ControlGoal == nullptr)
				{
					for(int i = 0; i < ((GoalAxisControl*)(args->GetSender()))->RemoveKeys.size(); i++)
					{
						Selected->RemoveGoal(abs(((GoalAxisControl*)(args->GetSender()))->RemoveKeys.at(i)));
					}
					TeleOpGoal goal = ((GoalAxisControl*)(args->GetSender()))->m_goal;
					SmartDashboard::PutString("GoalAxisControl Status", "Control Found");
					Goal* goalToAdd = SelectTeleOpGoal(args->GetSender()->m_activeCollection, goal);
					//Goal* OverGoal = new Goal_ControllerOverride(args->GetSender()->m_activeCollection, 1);
					//OverGoal->IdentityKey = ((GoalButtonControl*)(args->GetSender()))->IdKey;
					goalToAdd->IdentityKey = abs(((GoalAxisControl*)(args->GetSender()))->IdKey);
					goalToAdd->CopyStringFrom(args->GetStrings(), 0);
					goalToAdd->CopyFrom(args->GetValues(), ((GoalAxisControl*)(args->GetSender()))->StartIndex);
					Selected->AddGoal(goalToAdd);
					goalToAdd->Activate();
					//OverGoal->Activate();
				}
				else
				{
					ControlGoal->CopyStringFrom(args->GetStrings(), 0);
					ControlGoal->CopyFrom(args->GetValues(), ((GoalAxisControl*)(args->GetSender()))->StartIndex);
					if(ControlGoal->GetStatus() == Goal::Goal_Status::eCompleted && ((GoalAxisControl*)(args->GetSender()))->RepeatWhenFinished)
						ControlGoal->Activate();
				}
				SmartDashboard::PutString("GoalAxisControl Status", "GoalActivated");
				return;
			}
			else if (dynamic_cast<GoalButtonControl*>(args->GetSender()))
			{
				Goal* ControlGoal = Selected->GetGoal(abs(((GoalButtonControl*)(args->GetSender()))->IdKey));
				if(ControlGoal == nullptr)
				{
					for(int i = 0; i < ((GoalButtonControl*)(args->GetSender()))->RemoveKeys.size(); i++)
					{
						Selected->RemoveGoal(abs(((GoalButtonControl*)(args->GetSender()))->RemoveKeys.at(i)));
					}
					TeleOpGoal goal = ((GoalButtonControl*)(args->GetSender()))->m_goal;
					SmartDashboard::PutString("GoalButtonControl Status", "Control Found");
					Goal* goalToAdd = SelectTeleOpGoal(args->GetSender()->m_activeCollection, goal, args->GetValues().at(0));
					//Goal* OverGoal = new Goal_ControllerOverride(args->GetSender()->m_activeCollection, 1);
					//OverGoal->IdentityKey = ((GoalButtonControl*)(args->GetSender()))->IdKey;
					goalToAdd->IdentityKey = abs(((GoalButtonControl*)(args->GetSender()))->IdKey);
					goalToAdd->CopyStringFrom(args->GetStrings(), 0);
					goalToAdd->CopyFrom(args->GetValues(), ((GoalButtonControl*)(args->GetSender()))->StartIndex);
					Selected->AddGoal(goalToAdd);
					goalToAdd->Activate();
					//OverGoal->Activate();
					SmartDashboard::PutString("GoalButtonControl Status", "GoalActivated");
				}
				else
				{
					ControlGoal->CopyStringFrom(args->GetStrings(), 0);
					ControlGoal->CopyFrom(args->GetValues(), ((GoalButtonControl*)(args->GetSender()))->StartIndex);
					if(ControlGoal->GetStatus() == Goal::Goal_Status::eCompleted && ((GoalButtonControl*)(args->GetSender()))->RepeatWhenFinished)
						ControlGoal->Activate();
				}
				return;
			}
			else if (dynamic_cast<POVGoalControl*>(args->GetSender()))
			{
				Goal* ControlGoal = Selected->GetGoal(abs(((POVGoalControl*)(args->GetSender()))->IdKey));
				if(ControlGoal == nullptr)
				{
					for(int i = 0; i < ((POVGoalControl*)(args->GetSender()))->RemoveKeys.size(); i++)
					{
						Selected->RemoveGoal(abs(((POVGoalControl*)(args->GetSender()))->RemoveKeys.at(i)));
					}
					TeleOpGoal goal = ((POVGoalControl*)(args->GetSender()))->m_goal;
					SmartDashboard::PutString("POVGoalControl Status", "Control Found");
					Goal* goalToAdd = SelectTeleOpGoal(args->GetSender()->m_activeCollection, goal);
					//Goal* OverGoal = new Goal_ControllerOverride(args->GetSender()->m_activeCollection, 1);
					//OverGoal->IdentityKey = ((GoalButtonControl*)(args->GetSender()))->IdKey;
					goalToAdd->IdentityKey = abs(((POVGoalControl*)(args->GetSender()))->IdKey);
					goalToAdd->CopyStringFrom(args->GetStrings(), 0);
					goalToAdd->CopyFrom(args->GetValues(), ((POVGoalControl*)(args->GetSender()))->StartIndex);
					Selected->AddGoal(goalToAdd);
					goalToAdd->Activate();
					//OverGoal->Activate();
				}
				else
				{
					ControlGoal->CopyStringFrom(args->GetStrings(), 0);
					ControlGoal->CopyFrom(args->GetValues(), ((POVGoalControl*)(args->GetSender()))->StartIndex);
					if(ControlGoal->GetStatus() == Goal::Goal_Status::eCompleted && ((POVGoalControl*)(args->GetSender()))->RepeatWhenFinished)
						ControlGoal->Activate();
				}
				SmartDashboard::PutString("POVGoalControl Status", "GoalActivated");
				return;
			}
			else if (dynamic_cast<ComboControl*>(args->GetSender()))
			{
				Goal* ControlGoal = Selected->GetGoal(abs(((ComboControl*)(args->GetSender()))->IdKey));
				if(ControlGoal == nullptr)
				{
					for(int i = 0; i < ((ComboControl*)(args->GetSender()))->RemoveKeys.size(); i++)
					{
						Selected->RemoveGoal(abs(((ComboControl*)(args->GetSender()))->RemoveKeys.at(i)));
					}
					TeleOpGoal goal = ((ComboControl*)(args->GetSender()))->m_goal;
					SmartDashboard::PutString("ComboControl Status", "Control Found");
					Goal* goalToAdd = SelectTeleOpGoal(args->GetSender()->m_activeCollection, goal);
					//Goal* OverGoal = new Goal_ControllerOverride(args->GetSender()->m_activeCollection, 1);
					//OverGoal->IdentityKey = ((GoalButtonControl*)(args->GetSender()))->IdKey;
					goalToAdd->IdentityKey = abs(((ComboControl*)(args->GetSender()))->IdKey);
					goalToAdd->CopyStringFrom(args->GetStrings(), 0);
					goalToAdd->CopyFrom(args->GetValues(), ((ComboControl*)(args->GetSender()))->StartIndex);
					Selected->AddGoal(goalToAdd);
					goalToAdd->Activate();
					//OverGoal->Activate();
				}
				else
				{
					ControlGoal->CopyStringFrom(args->GetStrings(), 0);
					ControlGoal->CopyFrom(args->GetValues(), ((ComboControl*)(args->GetSender()))->StartIndex);
					if(ControlGoal->GetStatus() == Goal::Goal_Status::eCompleted && ((ComboControl*)(args->GetSender()))->RepeatWhenFinished)
						ControlGoal->Activate();
				}
				SmartDashboard::PutString("ComboControl Status", "GoalActivated");
				return;
			}
			args->GetSender()->SetToComponents(args->GetValues().at(0));
			SmartDashboard::PutNumber(args->GetSender()->name, args->GetValues().at(0));
		}
		else
		{
			Log::Error("FAILED EVENT");
		}
	}
	catch(exception &e)
	{
		Log::Error("Known Exception Thrown in onControllerValueChanged in a Control! This can cause fatal Runtime Errors! Check your logs and XML.");
		SmartDashboard::PutString("OnValueChangedStatus", "Error");
		//TODO: Make this the append instead
		Log::Error(e.what());
	}
	catch(...){
		Log::Error("UnknownException Thrown in onControllerValueChanged in a Control! This can cause fatal Runtime Errors! Check your XML and yell at the programmers!");
		SmartDashboard::PutString("OnValueChangedStatus", "Error");
	}
};

#pragma endregion

ControlItem::ControlItem(){}

ControlItem::ControlItem(Joystick *_joy, string _name, bool _reversed, double _powerMultiplier, ActiveCollection* activeCollection)
{
	m_activeCollection = activeCollection;
	joy = _joy;
	name = _name;
	reversed = _reversed;
	powerMultiplier = _powerMultiplier;
	ValueChanged += onControllerValueChanged;
	Holder = joy->GetPort() >= 0 && joy->GetPort() <= 1 ? (joy->GetPort() == 0 ? JoystickHolder::DriverController : JoystickHolder::OperatorController) : JoystickHolder::Other;
}

void ControlItem::AddComponent(OutputComponent *component)
{
	components.push_back(component);
}

vector<string> ControlItem::GetComponents()
{
	vector<string> componentNames;
	for(int i=0; i<(int)components.size();i++)
		componentNames.push_back((*components[i]).name);
	return componentNames;
}

void ControlItem::SetToComponents(double val, int Mode)
{
	try
	{
		for(int i=0; i<(int)components.size();i++)
		{
			if(dynamic_cast<Motor*>(components[i]))
			{
				if(Mode == 1)
				{
					((Motor*)components[i])->SetPower(val);
				}
				else if(Mode == 2)
				{
					((Motor*)components[i])->SetSpeed(val);
				}
				else if(Mode == 3)
				{
					((Motor*)components[i])->SetPosition(val);
				}
				else
				{
					((Motor*)components[i])->SetPercent(val);
				}
			}
			else
			{
				(*components[i]).Set(val);
			}
		}
	}
	catch(...)
	{
		Log::Error("Error setting value to binding for " + name + " control!");
	}
}

void ControlItem::DeleteComponent()
{
	delete this;
}

ControlItem::~ControlItem() {}

