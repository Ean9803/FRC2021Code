/****************************** Header ******************************\
Class Name:	ButtonControl inherits ControlItem
File Name:	ButtonControl.h
Summary:	Interface for a single button control.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Dylan Watson, Ian Poll
Email:	dylantrwatson@gmail.com, irobot9803@gmail.com
\*********************************************************************/

#ifndef SRC_CONTROLS_BUTTONCONTROL_H_
#define SRC_CONTROLS_BUTTONCONTROL_H_

#include "ControlItem.h"
#include "../Components/VictorSPItem.h"
#include "../Components/DoubleSolenoidItem.h"

namespace Controls{
class ButtonControl : public ControlItem {
private:
	int button;
	bool actOnRelease;
	double previous;
	bool isSolenoid;
	int Mode;
	bool isOverdrive;
	double TimePassed = 0;
	double TimeSet = 0;
	double Mult;
	bool reversed;

	int Sign(double Val)
	{
		if (Val >= 0)
			return 1;
		return -1;
	}

public:
	ButtonControl();
	ButtonControl(Joystick *_joy, string _name, int _button, bool _actOnRelease, bool _reversed, double _powerMultiplier, bool _isSolenoid, ActiveCollection* ac, int Mode, double HoldTime, bool _isOverdrive = false);
	virtual ~ButtonControl();
	virtual void DeleteComponent() override;
	virtual double Update(double _dTime) override;
};
}

#endif /* SRC_CONTROLS_BUTTONCONTROL_H_ */
