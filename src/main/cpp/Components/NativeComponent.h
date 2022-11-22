/****************************** Header ******************************\
Class Name: NativeComponent
File Name:	OutputComponent.h
Summary: Abstraction for all programmable robot components.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Dylan Watson
Email: dylantrwatson@gmail.com
\********************************************************************/

#ifndef SRC_COMPONENTS_NATIVECOMPONENT_H_
#define SRC_COMPONENTS_NATIVECOMPONENT_H_

#include <string>
#include <frc/WPILib.h>

#include "../Global.h"

#include <networktables/NetworkTableInstance.h>

using namespace frc;
using namespace std;

namespace Components
{
	class NativeComponent
	{
		public:
			//Name of the component
			string name;
			//Deleting component and cleaning up junk for quickload
			virtual void DeleteComponent() = 0;
			//Used to set if input components get values from the network table (which the values are set in the simulation in ApplicationZIPs) or real life
			void FromTable(bool Yes) { UseTable = !Yes; }
			//Method called from the ActiveCollection to the components
			virtual void UpdateComponent() {};
			//Used by ActiveCollection to set delta time between each loop to each component
			virtual void SetD_Time(double D_Time) { this->D_Time = D_Time; }
			NativeComponent(){}
			NativeComponent(string _name){ name = _name; }
			virtual ~NativeComponent(){}
			virtual double GetData(){return 0;};
			virtual void ResetData(){};
			//Used by combo control to use values for logic
			virtual double GetValue() {return 0;};
			//Bool set when in config Print = "true" in component
			void SetOutprint(bool Print) { PrintOut = Print; };
			//Network table ref to set and get values that the simulation modifies
			std::shared_ptr<NetworkTable> OutputTable = nt::NetworkTableInstance::GetDefault().GetTable("SmartDashboard");
		protected:
			bool UseTable = false;
			double D_Time;
            bool PrintOut = false;
	};

	//Used by swerve manager to store possible location
    struct double_Vector2
    {
        double X = 0;
        double Y = 0;

		double LastX = 0;
		double LastY = 0;
    };
}

#endif /* SRC_COMPONENTS_NATIVECOMPONENT_H_ */
