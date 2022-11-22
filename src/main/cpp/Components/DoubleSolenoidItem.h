/****************************** Header ******************************\
Class Name: DoubleSolenoidItem inherits OutputComponent
File Name:	DoubleSolenoidItem.h
Summary: Abstraction for the WPIlib DoubleSolenoid that extends to include
some helper and control methods.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Dylan Watson
Email: dylantrwatson@gmail.com
\********************************************************************/

#ifndef SRC_COMPONENTS_DOUBLESOLENOIDITEM_H_
#define SRC_COMPONENTS_DOUBLESOLENOIDITEM_H_

#include "OutputComponent.h"

using namespace std;

namespace Components{
class DoubleSolenoidItem : public OutputComponent{
private:
	//The port in the pcm for forward
	int forwardChannel;
	//The port in the pcm for reverse
	int reverseChannel;
	DoubleSolenoid::Value _default;
	DoubleSolenoid::Value _LastState;
	DoubleSolenoid::Value _CurrentState;
	bool reversed;
	DoubleSolenoid *solenoid;
	//The time that needs to pass before the return value is changed when the given value is >0
	double TimeExtended = 0;
	//The time that needs to pass before the return value is changed when the given value is <0
	double TimeRetracted = 0;
	//The current time that has passed when the given value is different from tha current value
	double TimeIn = 0;
	//When the abs value of the given value is >1 the timer is skipped
	bool Insta = false;

public:
	DoubleSolenoidItem();
	DoubleSolenoidItem(string _name, int _forwardChannel, int _reverseChannel, DoubleSolenoid::Value _default, bool _reversed, bool Real);
	virtual void DefaultSet() override;
	virtual void Set(DoubleSolenoid::Value value) override;
	void Set(bool value);
	void SetForward();
	void SetReverse();
	void SetOff();
	void SetTimeTaken(double Time) {TimeExtended = Time;};
	void SetTimeRetract(double Time) {TimeRetracted = Time;};
	virtual void Set(double value) override;
	virtual void DeleteComponent() override;
	virtual void UpdateComponent() override;
	virtual double Get() override;
	DoubleSolenoid::Value GetState();
	DoubleSolenoid::Value GetDefaultValue();
	virtual ~DoubleSolenoidItem();
	DoubleSolenoid *AsDoubleSolenoid() { return solenoid; }
};
}

#endif /* SRC_COMPONENTS_DOUBLESOLENOIDITEM_H_ */
