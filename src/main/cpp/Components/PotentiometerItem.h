/****************************** Header ******************************\
Class Name: PotentiometerItem inherits InputComponent
File Name:	PotentiometerItem.h
Summary: Abstraction for the WPIlib AnalogPotentiometer that extends to include
some helper and control methods.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Dylan Watson
Email: dylantrwatson@gmail.com
\********************************************************************/

#ifndef SRC_COMPONENTS_POTENTIOMETERITEM_H_
#define SRC_COMPONENTS_POTENTIOMETERITEM_H_

#include "InputComponent.h"

namespace Components{
class PotentiometerItem : public InputComponent{
private:
	int channel;
	int initPosition; //this is set on init. Ensures accurate measurement of distance traveled. As long as elevator is all the way down on startup, which it should
	AnalogPotentiometer *apt;
	//The max value the pot can reach
	double ValAngle = 0;
	//The range the pot has to be from an angle to return true when calling within angle
	double Threshold = 0;
	double Distance(double Val, double OtherVal)
	{
		return abs((Val) - (OtherVal));
	};
	bool Inrange(double Target, double Value, double Thres)
    {
	    if(Distance(Value, Target) <= Thres)
	   	{
			return true;
		}
        return false;
    };

public:
	PotentiometerItem();
	PotentiometerItem(int _channel, string _name, bool Real);
	double GetAngle();
	bool WithinAngle(double angle);
	void SetThreshold(double val) { Threshold = val; };
	void SetTotalVal(double Val) { ValAngle = Val; };
	virtual string GetName() override;
	virtual double Get() override;
	virtual void DeleteComponent() override;
	void UpdateComponent() override;
	virtual ~PotentiometerItem();
};
}

#endif /* SRC_COMPONENTS_POTENTIOMETERITEM_H_ */
