/****************************** Header ******************************\
Class Name: EncoderItem inherits InputComponent
File Name:	EncoderItem.h
Summary: Abstraction for the WPIlib Encoder that extends to include
some helper and control methods.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Dylan Watson, Ian Poll
Email: dylantrwatson@gmail.com, irobot9803@gmail.com
\********************************************************************/
#ifndef SRC_COMPONENTS_ENCODERITEM_H_
#define SRC_COMPONENTS_ENCODERITEM_H_

#include "InputComponent.h"

using namespace std;
using namespace frc;
using namespace Util;

namespace Components{
class EncoderItem : public InputComponent{
private:
	//Determines the mode of the encoder, Independent is a normal encoder,
	//data driven is feed in values from motor controller encoders such as talon or spark max and have it act as an encoder object
	enum InputType
	{
		Independent = 0,
		Data_Driven = 1
	};

	int aChannel;
	int bChannel;
	bool reversed;
	int Offset = 0;
	Encoder *encoder = nullptr;
	//This it the motor the encoder is reading values from and/or is returned when the linkedcomponent calls GetEncoder()
	NativeComponent* LinkedComponent = nullptr;
	double TicksPer = 1;
	InputType Type;
	double Threshold = 0;

	//Helper methods
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

	virtual double Get() override;
	EncoderItem();
	EncoderItem(string _name, int _aChannel, int _bChannel, bool _reversed, bool Real);
	EncoderItem(string _name, NativeComponent *Connected);
	NativeComponent* GetLinkedComponent(){return LinkedComponent;};
	double GetAngle();
	bool WithinAngle(double Angle);
	void SetThreshold(double val) { Threshold = val; };
	void SetTicksPerRev(double Ticks) { TicksPer = Ticks; };
	double GetTicksPerRev() { return TicksPer; };
	double GetDistance(double WheelRadius);
	virtual string GetName() override;
	void Reset();
	virtual void DeleteComponent() override;
	virtual void UpdateComponent() override;
	virtual ~EncoderItem();
};
}

#endif /* SRC_COMPONENTS_ENCODERITEM_H_ */
