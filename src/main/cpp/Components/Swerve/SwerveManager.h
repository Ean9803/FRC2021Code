/****************************** Header ******************************\
Class Name: SwerveManager
File Name:	SwerveManager.h
Summary: Manager that holds the Swerve modules
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ian Poll
Email: irobot9803@gmail.com
\********************************************************************/

#ifndef SRC_SWERVE_MANAGER_H_
#define SRC_SWERVE_MANAGER_H_

#include "SwerveModule.h"
#include "../NavX.h"
#include <iostream>

using namespace std;
using namespace frc;
using namespace Util;

namespace Components
{
    class SwerveManager : public OutputComponent
    {
        public:

            SwerveManager(string name, bool Wait, SwerveModule *FrontLeft, SwerveModule *FrontRight, SwerveModule *BackLeft, SwerveModule *BackRight, double _Length, double _Width, double WheelSize = 1, double RobotScale = 1);
            SwerveManager(string name, bool Wait, SwerveModule *FrontLeft, SwerveModule *FrontRight, SwerveModule *BackLeft, SwerveModule *BackRight, NavX *Nav, double _Length, double _Width, double WheelSize = 1, double RobotScale = 1);
            SwerveManager(string name, bool Wait, vector<SwerveModule*> Swerve_Modules, NavX *Nav, double _Length, double _Width, double WheelSize = 1, double RobotScale = 1);

            void TableSetUp();

            void Set(double rawV, double rawH, double rawS);
            void SetSwivel(double val);
            bool SetSwivelTarget(double Target);
            bool SetWheelTarget(double Target);
            bool SetTarget(double Wheel_Target, double Swivel_Target);

            bool SetSwivelTargetAt(SwerveModule::Location Loc, double Target);
            bool SetWheelTargetAt(SwerveModule::Location Loc, double Target);
            void SetSwivelAt(SwerveModule::Location Loc, double Power);
            void SetWheelAt(SwerveModule::Location Loc, double Power);
            void SetWheelSpeedAt(SwerveModule::Location Loc, double Speed);

            void SetWheelDiameter(double Diameter);

            void SetL(double L) {Length = L;};
            void SetW(double W) {Width = W;};
            void SetRobotScale(double Scale);

            void SetDelta(double D_Time);
            void ResetPID();
            void ResetSwivelEnc();
            void ResetWheelEnc();

            void UpdateLoc();
            void ResetLoc();
            void UpdateModules();

            void UpdateSystem(double D_Time);
            void PrintLocation();

            virtual void DeleteComponent() override;

            SwerveModule* Get(SwerveModule::Location Loc)
            {
                for(int i = 0; i < Modules.size(); i++)
                {
                    if(Modules.at(i)->GetLocation() == Loc)
                    {
                        return Modules.at(i);
                    }
                }
                return nullptr;
            };

            vector<SwerveModule*> GetModules();

            virtual void Set(double val);
			virtual double Get();
            virtual void DefaultSet();
			virtual void Set(DoubleSolenoid::Value value);
            void SetMaxPow(double Max)
            {
                MaxValParam = Max;
            }
            double_Vector2* GetBotPos() {return Pos;};
            double_Vector2* GetSimPos() {return SimPos;};

            double GetMax(double Val1, double Val2) {return (Val1 > Val2 ? Val1 : Val2); };

            double GetEnc()
            {
                double Average = 0;
                for(int i = 0; i < Modules.size(); i++)
                {
                    Average += Modules.at(i)->GetEnc();
                }
                return Average / Modules.size();
            };

        private:
            /*
            The SwerveModule is a class that contains the swivel and wheel motor and is put in a list because (if we or the mech team wanted to)
            we can have more than one swerve module at one location with the location being front left, front right, back left and back right, a good example of
            a use that we could do for fun is make a batmobile with two pairs of front wheels that can turn and have the back wheels as swerve modules as well
            and the swerve manager will handle the dups of locations correctly OR if we wanted to have only some swerve modles at some locations (IDK why mech would
            design a swerve without all four locations but if it was for a reason we can handle it)
            */
            vector<SwerveModule*> Modules;
            
            /*
            The navx is the gyro for the robot and is used (depending on the mode the swerve manager is in which is determined by the config,
            which the gyro is mostly used during feild oriented drive and auto positioning if you want to) to compensate for the robots rotation when driving,
            so meaning if the robot is facing 45 deg from the original rotation and the driver wants to go forward, the robot would move forward relative to is
            starting rotation, so in this case 45 deg in the oposite direction relative to the robot
            */
            NavX *RobotNav = nullptr;

            /*
            The lenght and width are used to calculate speeds of the individual swerve module wheel motors, so when spining and moving the wheels whould have to spin
            slower for a robot with a small width and lenght than a robot with a larger size in order to get the same rotation speed
            */
            double Length = 0;
            double Width = 0;
            /*
            Pos is a variable the swerve manager updates when it moves so it makes it easier to do auto stuff like going to an x,y position
            The SimPos uses the network table by grabing the x and y values, this is updated from the simulation that is in the ApplicationZIPs folder
            (its for windows because I hate macs and you need to download it and just run the .exe if you want to know how to get a cad model or make your own robot that
            might be better than mech team and show it off then you can contact me my email, irobot9803@gmail.com or just spam me on slack if in the future you guys still use it)
            */
            double_Vector2 *Pos;
            double_Vector2 *SimPos = nullptr;
            double Del_Time = 0;
            //This is modified in the config but is used to put a limit on the max speed the wheels should move at so the driver wont have to worry about not full sticking it
            double MaxValParam = 1;
            /*
            This is modified in config but is used to tell if the swivel module wheels should run when the swivel motors are still reaching the target angle
            this is to prevent the wheels from breaking by being bent when the robot needs to move, it can be turned off when you feel its properly tuned and the wheels
            reach the target angle fast enough
            */
            bool WaitSwivel = false;
            //Wheel size is modified in the config but it is used to calculate distance by getting the circumference and bla bla distance
            double WheelSize = 1;
    };
}

#endif