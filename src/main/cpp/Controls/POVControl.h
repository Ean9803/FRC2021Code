/****************************** Header ******************************\
Class Name:	POVControl inherits ControlItem
File Name:	POVControl.h
Summary:	Interface for an POV control.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Ian Poll
Email:	irobot9803@gmail.com
\*********************************************************************/


#ifndef SRC_CONTROLS_POVCONTROL_H_
#define SRC_CONTROLS_POVCONTROL_H_

#include <frc/WPILib.h>
#include "ControlItem.h"
#include "../Goals/FRC_Goals.h"

using namespace frc;
using namespace std;
using namespace Configuration;

namespace Controls
{
	class POVControl : public ControlItem
	{
		public:
            enum POVControlMode
            {
                JoystickMode = 0,
                ButtonMode = 1
            };
			POVControl(Joystick *_joy, string _name, double PosDirection, double _powerMultiplier, ActiveCollection* ac, int ControlMode, POVControlMode Mode);
			virtual double Update(double _dTime) override;
            virtual void DeleteComponent() override;
			virtual ~POVControl();
        private:
            double Dir;
            double NegDir;
            double Mult = 0;
            double LastPress = -1;
            bool RunStuff = false;
            bool Reverse = false;
            int ControlM = 0;
            POVControlMode Mode;
	};
}

#endif /* SRC_CONTROLS_POVCONTROL_H_ */
