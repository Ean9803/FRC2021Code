/****************************** Header ******************************\
Class Name: limelight
File Name:	limelight.h
Summary: Limelight header file and to get limelight values
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ian Poll
Email: irobot9803@gmail.com
\********************************************************************/
#pragma once

#ifndef SRC_limelight_H_
#define SRC_limelight_H_

#ifdef _Win32
#include "../Util//SmartDashboard.h"
#else
#include "frc/smartdashboard/Smartdashboard.h"
#include "networktables/NetworkTable.h"
#include "networktables/NetworkTableInstance.h"
#include "../Global.h"
#endif
#include "cmath"
#include "../Components/NativeComponent.h"
#include "iostream"

using namespace std;
using namespace Components;
namespace Lime{
class limelight : public NativeComponent //Inheritance or something 
{
    public:
        limelight() : NativeComponent("LimeLight")
        {
            SetLED(1);
            SetCamMode(false);
            SetPipeline();
            Log::General("Lime Constructed");
        } 
        limelight(bool Fake) : NativeComponent("LimeLight")
        {
            IsFake = Fake;
            SetLED(1);
            SetCamMode(false);
            SetPipeline();
            Log::General("Lime Constructed");
        } 
        double HorizontalOffset()
        {
            if(ProcessTarget && HasLEDon)
            {
                if (!IsFake)
                    return table->GetNumber("tx", 0.0);
                else
                    return (OutputTable->GetNumber("LimeLight-tx", 0.0));
            }
            else
            {
                Log::Error("Turn on the led and turn on vision processing in order to get target");
                return 0;
            }
        }
        double VerticalOffset()
        {
            if(ProcessTarget && HasLEDon)
            {
                if (!IsFake)
                    return (table->GetNumber("ty", 0.0));
                else
                    return (OutputTable->GetNumber("LimeLight-ty", 0.0));
            }
            else
            {
                Log::Error("Turn on the led and turn on vision processing in order to get target");
                return 0;
            }
        }
        bool TargetCentered()
        {
            if(SeesTarget())
            {
                if(abs(HorizontalOffset()) <= 0.1 && abs(VerticalOffset()) <= 0.1)
                    return true;
            }
            return false;
        }
        double TargetDistance(double TargetHeight)
        {
            if(ProcessTarget && HasLEDon)
            {
                if (!IsFake)
                {
                    double robotheight = TargetHeight - LimeLightHeight;
                    double radianAngle = (tan(((VerticalOffset()) * 3.1415) / 180));
                    Log::General("angle: " + to_string(radianAngle) + "  :: Limelight input: " + to_string(VerticalOffset()));
                    return (robotheight / (radianAngle)) + 5;
                }
                else
                {
                    return OutputTable->GetNumber("LimeLight-Distance", 0);
                }
            }
            else
            {
                if(ProcessTarget)
                {
                    Log::Error("Turn on the cam vision processing in order to get target");
                }
                else if(HasLEDon)
                {
                    Log::Error("Turn on the led for vision processing in order to get target");
                }
                else
                {
                    Log::Error("Turn on the led and turn on vision processing in order to get target");
                }
                return 0;
            }
        }
        bool SeesTarget()
        {
            if(ProcessTarget && HasLEDon)
            {
                int Present = 0;
                if(!IsFake)
                    Present = table->GetNumber("tv", 0.0);
                else
                    Present = OutputTable->GetNumber("LimeLight-tv", 0);
                if(Present > 0)
                {
                    return true;
                }
                return false;
            }
            else
            {
                if(ProcessTarget)
                {
                    Log::Error("Turn on the cam vision processing in order to get target");
                }
                else if(HasLEDon)
                {
                    Log::Error("Turn on the led for vision processing in order to get target");
                }
                else
                {
                    Log::Error("Turn on the led and turn on vision processing in order to get target");
                }
                return false;
            }
        }

        double TargetNumber()
        {
            if(SeesTarget())
            {
                double Present = 0;
                if(!IsFake)
                    Present = table->GetNumber("tv", 0.0);
                else
                    Present = OutputTable->GetNumber("LimeLight-tv", 0);
                return Present;
            }
            else
            {
                return 0;
            }
        }

        void SetReturn(int Val)
        {
            ReturnVal = Val;
        }

        void SetTargetHeight(double Val)
        {
            TargetHeightVal = Val;
        }

        void SetLimeLightHeight(double Val)
        {
            LimeLightHeight = Val;
        }

        virtual double GetValue() override
        {
            if(ReturnVal == 0)
                return TargetNumber();
            else if(ReturnVal == 1)
                return TargetDistance(TargetHeightVal);
            else
                return TargetCentered() ? 1 : 0;
        }

        void SetPipeline(int Pipe = 0)
        {
            if(Pipe >= 0 && Pipe < 10)
                table->PutNumber("pipeline", Pipe);
            else
                Log::Error("Pipelines only go from 0 - 9 not: " + to_string(Pipe));
        }
        void SetCamMode(bool VisionProcess = true)
        {
            if(VisionProcess)
            {
                ProcessTarget = true;
                table->PutNumber("camMode", 0);
            }
            else
            {
                ProcessTarget = false;
                table->PutNumber("camMode", 1);
            }    
        }
        void Snapshots(bool takeShot)
        {
            if(takeShot)
            {
                table->PutNumber("snapshot", 1);
            }
            else
            {
                table->PutNumber("snapshot", 0);
            }
        }
        void SetLED(int State = 0)
        {
            if(State == 0)
            {
                HasLEDon = true;
                table->PutNumber("ledMode", 3);
                OutputTable->PutNumber("LimeLight-ledMode", 3);
            }
            else if(State == 1)
            {
                HasLEDon = false;
                table->PutNumber("ledMode", 0);
                OutputTable->PutNumber("LimeLight-ledMode", 0);
            }
            else
            {
                HasLEDon = false;
                table->PutNumber("ledMode", 2);
                OutputTable->PutNumber("LimeLight-ledMode", 2);
                Log::Error("THE ROBOT IS KILLING PEOPLE'S EYES, KEEP IT UP AND SOMEONE MIGHT NOTICE!");
            }
        }
        virtual void UpdateComponent() override
        {
            if(PrintOut)
            {
                string SeeResp = "N/A";
                if(SeesTarget())
                    SeeResp = "Yes";
                else
                    SeeResp = "No";
                string  Sees = "Sees: " + SeeResp;
                string HOff = " Horizantal Offset: " + to_string(HorizontalOffset());
                string VOff = " Vertical Offset: " + to_string(VerticalOffset());
                string Distance = " Target Distance: " + to_string(TargetDistance(TargetHeightVal));
                string Height = " Target height: " + to_string(TargetHeightVal);
                string CenResp = "N/A";
                if(TargetCentered())
                    CenResp = "Yes";
                else
                    CenResp = "No";
                string Centered = " Centered: " + CenResp;
                Log::General(Sees + HOff + VOff + Distance + Height + Centered);
            }
        }

        void DeleteComponent() {delete this;};
    private:
        bool HasLEDon = true;
        bool ProcessTarget = true;
        bool IsFake = false;
        int ReturnVal = 0;
        double TargetHeightVal = 0;
        double LimeLightHeight = 0;
        std::shared_ptr<NetworkTable> table = nt::NetworkTableInstance::GetDefault().GetTable("limelight");
 };
}
#endif /* SRC_limelight_H_ */