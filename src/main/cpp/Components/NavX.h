/****************************** Header ******************************\
Class Name: NavX inherits AHRS, NativeComponent
File Name:	NavX.h
Summary: Provides methods to use and get values from sensors on the NavX board.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Dylan Watson
Email: dylantrwatson@gmail.com
\********************************************************************/

#ifndef SRC_COMPONENTS_NAVX_H_
#define SRC_COMPONENTS_NAVX_H_

#include <AHRS.h>

#include "NativeComponent.h"

namespace Components{
class NavX : public AHRS, public NativeComponent{
public:
	enum AxisAngle
	{
		Y_Axis = 0,
		X_Axis = 1,
		Z_Axis = 2
	};

	NavX() : AHRS(SerialPort::Port::kMXP, AHRS::SerialDataType::kProcessedData, 50), NativeComponent("NavX"){GetLast();}
	NavX(bool Fake) : AHRS(SerialPort::Port::kMXP, AHRS::SerialDataType::kProcessedData, 50), NativeComponent("NavX"){if(Fake){FakeRun = true; SetUP(); GetLast();}}
	NavX(SPI::Port spiPortId, uint8_t updateRateHz) : AHRS(spiPortId, updateRateHz), NativeComponent("NavX"){GetLast();}
	NavX(SPI::Port spiPortId, int spiBitRate, uint8_t updateRateHz) : AHRS(spiPortId, spiBitRate, updateRateHz), NativeComponent("NavX"){GetLast();}
	NavX(I2C::Port i2CPortId, uint8_t updateRateHz): AHRS(i2CPortId, updateRateHz), NativeComponent("NavX"){GetLast();}
	NavX(SerialPort::Port serialPortId, SerialDataType dataType, uint8_t updateRateHz) : AHRS(serialPortId, dataType, updateRateHz), NativeComponent("NavX"){GetLast();}
	NavX* GetRawComponent(){return this;}
	virtual ~NavX(){}
	double GetNavXAngle(AxisAngle Axis = AxisAngle::Y_Axis){
		if (FakeRun)
		{
			switch (Axis)
			{
				case AxisAngle::Y_Axis:
					return OutputTable->GetNumber("NavX-Y", 0);
				break;

				case AxisAngle::X_Axis:
					return OutputTable->GetNumber("NavX-X", 0);
				break;

				case AxisAngle::Z_Axis:
					return OutputTable->GetNumber("NavX-Z", 0);
				break;
			
				default:
					break;
			}
			return OutputTable->GetNumber("NavX-Y", 0);
		}
		else
		{
			double angle = 0;
			switch (Axis)
			{
				case AxisAngle::Y_Axis:
					angle = GetYaw();
				break;

				case AxisAngle::X_Axis:
					angle = GetPitch();
				break;

				case AxisAngle::Z_Axis:
					angle = GetRoll();
				break;
			
				default:
					break;
			}
			return angle;
		}
	}

	double GetConstAngle(AxisAngle Axis = AxisAngle::Y_Axis)
	{
		return GetNavXAngle(Axis) - (Axis == AxisAngle::Y_Axis ? RefAngleY : (Axis == AxisAngle::X_Axis ? RefAngleX : RefAngleZ));
	}

	void ResetNav()
	{
		RefAngleY = GetConstAngle(AxisAngle::Y_Axis);
		RefAngleX = GetConstAngle(AxisAngle::X_Axis);
		RefAngleZ = GetConstAngle(AxisAngle::Z_Axis);
		Reset();
		ResetFakeNav();
	}
	
	void ResetFakeNav()
	{
		OutputTable->PutBoolean("NavX-Reset", true);
	}

	virtual void UpdateComponent() override
	{
		OutputTable->PutNumber("NavX-Y", GetYaw());
		OutputTable->PutNumber("NavX-X", GetPitch());
		OutputTable->PutNumber("NavX-Z", GetRoll());
	}

	/*
	The delete component is just a way to clean up space (it was used for quickload so we dont get dups of objects)
	*/
	virtual void DeleteComponent() {delete this;};

	void OutPrint()
	{
		Log::General("NavX:<Angle>==>  Y Axis: " + to_string(GetNavXAngle(AxisAngle::Y_Axis)) + "   X Axis: " + to_string(GetNavXAngle(AxisAngle::X_Axis)) + "   Z Axis: " + to_string(GetNavXAngle(AxisAngle::Z_Axis)));
	}

	void SetReturn(AxisAngle Angle, bool Const = false)
	{
		ReturnAngle = Angle;
		ConstantA = Const;
	}

	virtual double GetValue() override
	{
		if(ConstantA)
			return GetConstAngle(ReturnAngle);
		else
			return GetNavXAngle(ReturnAngle);
	}

private:
	bool FakeRun = false;
	double RefAngleY = 0;
	double RefAngleX = 0;
	double RefAngleZ = 0;
	AxisAngle ReturnAngle;
	bool ConstantA = false;
	void SetUP()
	{
		OutputTable->PutNumber("NavX-Y", 0);
		OutputTable->PutNumber("NavX-X", 0);
		OutputTable->PutNumber("NavX-Z", 0);
		OutputTable->PutBoolean("NavX-Reset", true);
	}
	void GetLast()
	{
		RefAngleY = OutputTable->GetNumber("NavX-Y", 0);
		RefAngleX = OutputTable->GetNumber("NavX-X", 0);
		RefAngleZ = OutputTable->GetNumber("NavX-Z", 0);
	}
};
	

}
	

#endif /* SRC_COMPONENTS_NAVX_H_ */
