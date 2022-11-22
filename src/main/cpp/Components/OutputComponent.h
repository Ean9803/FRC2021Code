/****************************** Header ******************************\
Class Name: OutputComponent inherits NativeComponent
File Name:	OutputComponent.h
Summary: Abstraction for all programmable robot output components.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Dylan Watson
Email: dylantrwatson@gmail.com
\********************************************************************/

#ifndef SRC_COMPONENTS_OUTPUTCOMPONENT_H_
#define SRC_COMPONENTS_OUTPUTCOMPONENT_H_

#include "NativeComponent.h"

using namespace std;

namespace Components
{
	class OutputComponent : public NativeComponent
	{
		public:
			bool inUse = false;
			//Abstract method to set component
			virtual void Set(double val) = 0;
			//Abstract method to get value
			virtual double Get() = 0;
			OutputComponent(){}
			OutputComponent(string name) : NativeComponent(name){}
			//Used by combo control to use values for logic
			virtual double GetValue() override {return Get();};
			//Abstract method to set solenoid to default value
			virtual void DefaultSet() = 0;
			//Abstract method to set solenoid to value
			virtual void Set(DoubleSolenoid::Value value) = 0;
			virtual ~OutputComponent(){}
	};
}

#endif /* SRC_COMPONENTS_OUTPUTCOMPONENT_H_ */
