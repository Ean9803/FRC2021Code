/****************************** Header ******************************\
Class Name: REVColorSensorV3 inherits InputComponent
File Name:	REVColorSensorV3.cpp
Summary: Abstraction for the WPIlib Encoder that extends to include
some helper and control methods.
Project:     BroncBotzFRC2020
Copyright (c) BroncBotz.
All rights reserved.
    ----------
   |///   /// |
   |    |     |
   | ________ |
    -----------

Author(s): Shruti Venkatramanan, Emily Martinez, Guadalupe Rodriguez.
Email: Shruti.venkat05@gmail.com
\********************************************************************/

#include "REVColorSensorV3.h"


using namespace rev;
using namespace std;
using namespace frc;
using namespace Components;

//Constructor used by config
REVColorSensorV3::REVColorSensorV3(string _name, bool Real) : InputComponent(_name)
{
	Color = new ColorSensorV3(frc::I2C::Port::kOnboard);
	Name = _name;
  kBlueTarget = frc::Color(0.143, 0.427, 0.429);
  kGreenTarget = frc::Color(0.197, 0.561, 0.240);
  kRedTarget = frc::Color(0.561, 0.232, 0.114);
  kYellowTarget = frc::Color(0.361, 0.524, 0.113);
  kBlack = frc::Color(0, 0, 0);
  m_colorMatcher.AddColorMatch(kBlueTarget);
  m_colorMatcher.AddColorMatch(kGreenTarget);
  m_colorMatcher.AddColorMatch(kRedTarget);
  m_colorMatcher.AddColorMatch(kYellowTarget);
  isReal = Real;
  OutputTable->PutNumber(name + "-Color", 0);
  OutputTable->PutNumber(name + "-Distance", 0);
}

//Returns a color object that best matched the color the sensor is looking at
frc::Color REVColorSensorV3::GetColor(){
  if(isReal)
  {
	  frc::Color input = (Color->GetColor()); 
	  return input;
  }
  else
  {
    double ColorVal = REVColorSensorV3::Get();
    if(ColorVal == 1)
    {
      return kBlueTarget;
    }
    else if(ColorVal == 2)
    {
      return kRedTarget;
    }
    else if(ColorVal == 3)
    {
      return kGreenTarget;
    }
    else if(ColorVal == 4)
    {
      return kYellowTarget;
    }
    else
    {
      return kBlack;
    }
  }
}

//Returns the distance the object is from the color sensor by using how bright the light reflecting off the surface is
uint32_t REVColorSensorV3::GetProximity(){
  if(isReal)
  {
    uint32_t input = ((uint32_t)Color->GetProximity());
    return input;
  }
  else
  {
    return ((1 / (OutputTable->GetNumber(name + "-Distance", 0) + 1)) * 2047) - 1; 
  }
}

//Returns the name of the component
string REVColorSensorV3::GetName(){
	return name;
}

//Returns a number that corresponds to a color that best matches the color the sensor is seeing
double REVColorSensorV3::Get(){

  if(isReal)
  {
  string colorString = "";
    double Conf = 0.0;
    frc::Color matchedColor = m_colorMatcher.MatchClosestColor(GetColor(), Conf);
    
  if (matchedColor == kBlueTarget) {
      return 1;
    }
     else if (matchedColor == kRedTarget) {
      return 2;
    } 
    else if (matchedColor == kGreenTarget) {
      return 3;
    }
     else if (matchedColor == kYellowTarget) {
      return 4;
    }
     else {
      return 0;
    }
  }
  else
  {
    return OutputTable->GetNumber(name + "-Color", 0);
  }
}

/*
The delete component is just a way to clean up space (it was used for quickload so we dont get dups of objects)
*/
void REVColorSensorV3::DeleteComponent()
{
  delete Color;
  delete this;
}

//Method that is called by the ActiveCollection
void REVColorSensorV3::UpdateComponent()
{
  if (!UseTable)
	{
		OutputTable->PutNumber(name + "-Color", REVColorSensorV3::Get());
	}
  frc::Color detectedColor = Color->GetColor();
  if(PrintOut)
	{
		Log::General("````````````````````````````" + name + " value: " + to_string(Get()) + " current color = R: " + to_string(detectedColor.red) + ", G: " + to_string(detectedColor.green) + ", B: " + to_string(detectedColor.blue) + " \nDistance: " + to_string(Color->GetProximity()));
	}
}

REVColorSensorV3::~REVColorSensorV3() {}