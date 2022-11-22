/****************************** Header ******************************\
Class Name:	AxisControl inherits ControlItem
File Name:	AxisControl.h
Summary:	Interface for an axis control.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Dylan Watson, Ian Poll
Email:	dylantrwatson@gmail.com, irobot9803@gmail.com
\*********************************************************************/


#ifndef SRC_CONTROLS_AXISCONTROL_H_
#define SRC_CONTROLS_AXISCONTROL_H_

#include <frc/WPILib.h>
#include "ControlItem.h"
#include "../Components/PotentiometerItem.h"
#include "../Goals/FRC_Goals.h"

using namespace frc;
using namespace std;
using namespace Configuration;

namespace Controls
{
	class AxisControl : public ControlItem
	{
		private:
			int axis;
			double deadZone;
			double Mult;
			int Mode;
			bool Overdrive;
			bool isLift;
			double gane;
			bool reversed = false;
			ActiveCollection* m_activeCollection;
			int Sign(double val)
			{
				if(val >= 0)
					return 1;
				return -1;
			}

		public:
			AxisControl();
			AxisControl(Joystick *_joy, string _name, int _axis, double _deadZone, bool _reversed, double _powerMultiplier, ActiveCollection* ac, int Mode, bool _useOverdrive = false);
			virtual double Update(double _dTime) override;
			void SetLift(double _gane, ActiveCollection *activeCollection);
			virtual ~AxisControl();
	};
}

#endif /* SRC_CONTROLS_AXISCONTROL_H_ */
