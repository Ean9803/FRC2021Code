/****************************** Header ******************************\
Class Name:	ServoControl inherits ControlItem
File Name:	ServoControl.h
Summary:	Interface for an servo control.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Ian Poll
Email:	irobot9803@gmail.com
\*********************************************************************/


#ifndef SRC_CONTROLS_SERVOCONTROL_H_
#define SRC_CONTROLS_SERVOCONTROL_H_

#include <frc/WPILib.h>
#include "ControlItem.h"
#include "../Goals/FRC_Goals.h"

using namespace frc;
using namespace std;
using namespace Configuration;

namespace Controls
{
	class ServoControl : public ControlItem
	{
		public:
            enum ServoControlPart
            {
                button = 0,
                JoystickAxis = 1,
                DPad = 2
            };
            
			ServoControl(Joystick *_joy, string _name, ActiveCollection* ac, ServoControlPart useType, bool cycle, vector<double> Angles, vector<double> Buttons, double Axis, vector<double> PosDir, double Mult, bool Inc, int Mode);
			virtual double Update(double _dTime) override;
            virtual void DeleteComponent() override;
			virtual ~ServoControl();
        private:
            vector<double> Angles;
            vector<double> buttons;
            double Axis;
            bool Inc;
            double Mult;
            bool cycle;
            vector<double> Dir;
            ServoControlPart useType;
            double CurrentAngle = 0;
            double LastAngle = 0;
            int index = 0;
            int Mode = 0;
            bool Pressed = false;
	};
}

#endif /* SRC_CONTROLS_SERVOCONTROL_H_ */
