/****************************** Header ******************************\
Class Name: Config
File Name:	Config.h
Summary: 	Manages and loads the configuration file.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ryan Cooper, Dylan Watson, Chris Weeks, Ian Poll
Email: cooper.ryan@centaurisoftware.co, dylantrwatson@gmail.com, chrisrweeks@aol.com, irobot9803@gmail.com
\********************************************************************/


#ifndef SRC_CONFIG_CONFIG_H_
#define SRC_CONFIG_CONFIG_H_

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableEntry.h>
#include <networktables/NetworkTableInstance.h>
#include <map>

#include "ActiveCollection.h"
#include "../Systems/Drive.h"
#include "../Controls/ButtonControl.h"
#include "../Controls/AxisControl.h"
#include "../Controls/ToggleButtonControl.h"
#include "../Controls/GoalButtonControl.h"
#include "../Controls/GoalAxisControl.h"
#include "../Controls/SwerveControl.h"
#include "../Controls/POVControl.h"
#include "../Controls/POVGoalControl.h"
#include "../Controls/ComboControl.h"
#include "../Controls/ServoControl.h"
#include "../Pugi/pugixml.h"
#include "../Goals/GoalSelector.h"
#include "../Util/Log.h"
#include "../Limelight/limelight.h"
#include "../Components/SparkMaxItem.h"
#include "../Components/REVColorSensorV3.h"
#include "../Components/DigitalSwitch.h"

using namespace System;
using namespace pugi;
using namespace Lime;
namespace Configuration{

class Config {
public:
	Config(ActiveCollection *_activeCollection, Drive *_drive, string ConfigFile = "config");
	//TODO: Make this a bool return
	void LoadValues(xml_document &doc);
	void AllocateComponents(xml_node &root);
	void AllocateDriverControls(xml_node &controls);
	void AllocateOperatorControls(xml_node &controls);
	virtual ~Config();
private:
	Joystick *m_driveJoy;
	Joystick *m_operatorJoy;
	ActiveCollection *m_activeCollection;
	Drive *m_drive;
	vector<string> getBindingStringList(string bindings);
	bool setBindingsToControl(vector<string> bindings, ControlItem *control);
	TeleOpGoal getTeleOpGoal(string goalString);
	int GetLocation(string Loc);
	int GetActivationState(string Type);

	//If it can return the value of the attribute name (double) and if not return default and output error saying value of errorMessage
	double GetDoubleAttribute(xml_node Node, string Name, double Default = 0, string errorMessage = "ERROR finding Attribute");
	
	//If it can return the value of the attribute name (int) and if not return default and output error saying value of errorMessage
	int GetIntAttribute(xml_node Node, string Name, int Default = 0, string errorMessage = "ERROR finding Attribute");
	
	//If it can return the value of the attribute name (bool) and if not return default and output error saying value of errorMessage
	bool GetBoolAttribute(xml_node Node, string Name, bool Default = false, string errorMessage = "ERROR finding Attribute");
	
	//If it can return the value of the attribute name (string) and if not return default and output error saying value of errorMessage
	string GetStringAttribute(xml_node Node, string Name, string Default = "", string errorMessage = "ERROR finding Attribute");
	
	//Checks if a node has an attribute of name and if not output error saying value of errorMessage
	bool HasAttribute(xml_node Node, string Name, string errorMessage = "ERROR finding Attribute");

	void SetMotorGroup(Motor* Subj, xml_node Node);

	//Return the second value (bool) of a group in a map with the same first value that it gets in the node
	bool GetBoolAttDic(xml_node Node, string Name, map<string, bool> Map, bool Default = false);
	
	//Return the second value (int) of a group in a map with the same first value that it gets in the node
	int GetIntAttDic(xml_node Node, string Name, map<string, int> Map, int Default = 0);
	
	//Return the second value (double) of a group in a map with the same first value that it gets in the node
	double GetDoubleAttDic(xml_node Node, string Name, map<string, double> Map, double Default = 0);
	
	//Return the second value (string) of a group in a map with the same first value that it gets in the node
	string GetStringAttDic(xml_node Node, string Name, map<string, string> Map, string Default = "N/A");

	//Checks if a string can be converted to a double
	bool isDouble(string str)
	{
		char* Out = 0;
		strtod(str.c_str(), &Out);
		if(*Out != '\0' || Out == str.c_str())
			return false;
		return true;
	}

	bool Real = false;

	map<string, bool> ReturnType = {{"Power", false}, {"Angle", true}};
	map<string, int> LimeReturnType = {{"Number", 0}, {"Distance", 1}, {"Centered", 2}};
	map<string, int> NavReturnType = {{"X_Axis", 0}, {"Y_Axis", 1}, {"Z_Axis", 2}};
};

}
#endif /* SRC_CONFIG_CONFIG_H_ */
