/****************************** Header ******************************\
Class Name:	POVGoalControl inherits ControlItem
File Name:	POVGoalControl.h
Summary:	Interface for an POV Goal control.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Ian Poll
Email:	irobot9803@gmail.com
\*********************************************************************/


#ifndef SRC_CONTROLS_POV_GOAL_CONTROL_H_
#define SRC_CONTROLS_POV_GOAL_CONTROL_H_

#include <frc/WPILib.h>
#include "ControlItem.h"
#include "../Goals/FRC_Goals.h"
#include "../Goals/FRC_Goals.h"
#include "../Goals/GoalSelector.h"

using namespace frc;
using namespace std;
using namespace Configuration;

namespace Controls
{
	class POVGoalControl : public ControlItem
	{
		public:
            enum POVGoalControlMode
            {
                JoystickMode = 0,
                ButtonMode = 1
            };
            enum POVGoalControlState
            {
                onPress = 0,
                onRelease = 1,
                both = 2
            };
            
            TeleOpGoal m_goal;
	        int StartIndex = 0;
	
	        int IdKey = 0;
	        vector<int> RemoveKeys;
	        bool RepeatWhenFinished;

			POVGoalControl(Joystick *_joy, string _name, double PosDirection, double _powerMultiplier, ActiveCollection* ac, TeleOpGoal _goal, vector<string> Strings, int _StartIndex, int KeyID, vector<int> _RemoveKeys, bool _RepeatWhenFinished,  POVGoalControlMode Mode, POVGoalControlState State);
			virtual double Update(double _dTime) override;
            virtual void DeleteComponent() override;
			virtual ~POVGoalControl();
        private:
            double Dir;
            double NegDir;
            double Mult = 0;
            double LastPress = -1;
            bool RunStuff = false;
            bool Reverse = false;
	        vector<string> m_StringVals;
            POVGoalControlMode Mode;
            POVGoalControlState State;
	};
}

#endif /* SRC_CONTROLS_POV_GOAL_CONTROL_H_ */
