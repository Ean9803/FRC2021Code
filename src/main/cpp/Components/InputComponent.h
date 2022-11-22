/****************************** Header ******************************\
Class Name: InputComponent inherits NativeComponent
File Name:	InputComponent.h
Summary: Abstraction for all programmable robot input components.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Dylan Watson
Email: dylantrwatson@gmail.com
\********************************************************************/

#ifndef SRC_COMPONENTS_INPUTCOMPONENT_H_
#define SRC_COMPONENTS_INPUTCOMPONENT_H_

#include <string>

#include "NativeComponent.h"

using namespace std;

namespace Components
{
    class InputComponent : public NativeComponent
    {
        public:
			//Abstract method to get a value
	        virtual double Get() = 0;
			//Abstract method to get the name of a component
	        virtual string GetName() = 0;
			//Used in combo control to get a value, can return something else when overrided
			virtual double GetValue() override {return Get();};
	        InputComponent() { }
	        InputComponent(string name) : NativeComponent(name) { }
	        virtual ~InputComponent() { }
    };
}

#endif /* SRC_COMPONENTS_INPUTCOMPONENT_H_ */
