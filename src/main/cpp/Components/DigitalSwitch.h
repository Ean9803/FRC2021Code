/****************************** Header ******************************\
Class Name: DigitalSwitch inherits OutputComponent
File Name:	DigitalSwitch.h
Summary: Digital Component to help with logic for combo controls
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ian Poll
Email: irobot9803@gmail.com
\********************************************************************/

#ifndef SRC_COMPONENTS_DIGITALSWITCH_H_
#define SRC_COMPONENTS_DIGITALSWITCH_H_

#include "OutputComponent.h"

using namespace std;

namespace Components
{
class DigitalSwitch : public OutputComponent
{
private:
	//The time that needs to pass before the return value is changed when the given value is >0
	double TimeExtended = 0;
	//The time that needs to pass before the return value is changed when the given value is <0
	double TimeRetracted = 0;
	//The current time that has passed when the given value is different from tha current value
	double TimeIn = 0;
	//When the abs value of the given value is >1 the timer is skipped
	bool Insta = false;
	//The current value to return
    double _CurrentState = 0;
	//The given value
    double State = 0;
	//The default value
    double Def = 0;

public:
    DigitalSwitch();
	DigitalSwitch(string _name, double _Default);
	virtual void DefaultSet() override;
	virtual void Set(DoubleSolenoid::Value value) override;
	void SetOn();
	void SetOff();
    void SetDead();
	void SetTimeExtend(double Time) {TimeExtended = Time;};
	void SetTimeRetract(double Time) {TimeRetracted = Time;};
	virtual void Set(double value) override;
	virtual void DeleteComponent() override;
	virtual void UpdateComponent() override;
	virtual double Get() override;
	virtual ~DigitalSwitch();
};
}

#endif /* SRC_COMPONENTS_DIGITALSWITCH_H_ */
