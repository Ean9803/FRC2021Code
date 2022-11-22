/****************************** Header ******************************\
Class Name: motor
File Name:	motor.h
Summary: Abstract class to motor
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ian Poll
Email: irobot9803@gmail.com
\********************************************************************/
#ifndef SRC_Motor_motor_H_
#define SRC_Motor_motor_H_
#include "ctre/Phoenix.h"
#include "OutputComponent.h"
#include "../Util/PIDProfile.h"
#include "EncoderItem.h"

using namespace std;
using namespace frc;
using namespace Util;

namespace Components
{
    class Motor : public OutputComponent
    {
        private:
            //PDP port for brownout protection for motor
            int PDBPort = 0;
            //Time for motor to be lowered in power to tell the driver or operator to not do that or for the battery to stabalize again
            double TimeTimedOut = 0;
            //The amount to lower the motor by every time the motor goes over amp
            double LowerAmount = 0;
            double CurrentPowerTarget = 0;

            double ABSVal(double val)
            {
                return (val < 0 ? val * -1 : val);
            }

            int SignVal(double val)
            {
                return (val >= 0 ? 1 : -1);
            }
            //The rate for the motor to return to normal power (ramp up by this amount)
            double RegenRate = 0.01;
            double PersonalLowerRate = 0;

            //PIDs for ramping and encoder positioning
            PIDProfile *PositionProfile = nullptr;
            PIDProfile *PowerProfile = nullptr;

            //The default encoder made alongside the motor
            EncoderItem* ConnectedEncoder = nullptr;
            //The encoder that is set in the config by encoderlinks and will be returned when called GetEncoder()
            EncoderItem* LinkedEncoder = nullptr;

            bool IsReversed = false;

            enum MotorMode
            {
                raw = 0,
                ramp = 1,
                rate = 2,
                pos = 3
            };

            MotorMode Mode;

            double TargetVal = 0;
            double CurrentVal = 0;

            int OutputReturn = 0;
            double Threshold = 0;

            //List of motors to have power match up to this motor power times the sync bias set in the config
            vector<Motor*> LinkedMotors;
            Motor* ParentMotor = nullptr;
            double SyncBias = 1;

            double MotorBias = 1;
            bool IsFinished = false;

            double CurrentPower = 0;
            double PowerTolerance = 0.01;
        public:

            Motor() : OutputComponent(){}
            //Constructor
            Motor(string name, bool reverse) : OutputComponent(name)
            {
                Motor::InitProfiles();
                IsReversed = reverse;
                LinkedEncoder = new EncoderItem(name + "-LinkedEncoder", this);
                ConnectedEncoder = LinkedEncoder;
            }

            //Sets the linked encoder object to the motor
            void SetLinkedEncoder(EncoderItem* Encoder)
            {
                LinkedEncoder = Encoder;
            }

            //Returns the linked encoder
            EncoderItem* GetEncoder()
            {
                return LinkedEncoder;
            }

            //When the motor is copying another motor, the main motor or the parent motor is set to this motor to prevent any accidental infinite loops for syncing 
            void SetParentMotor(Motor* Mtr)
            {
                ParentMotor = Mtr;
            }

            //Adds a child motor and sets the child motor's parent to itself
            void AddLinkedMotor(Motor* Mtr)
            {
                LinkedMotors.push_back(Mtr);
                Mtr->SetParentMotor(this);
            }

            //Method to sync all children motors to this motor power time sync bias
            void SyncMotors(double Power)
            {
                for(int i = 0; i < (int)LinkedMotors.size(); i++)
                {
                    if(LinkedMotors[i] != ParentMotor)
                        LinkedMotors[i]->SetPercent(Power * LinkedMotors[i]->GetSyncBias());
                }
            }

            //Sets the sync bias
            void SetSyncBias(double Bias)
            {
                SyncBias = Bias;
            }

            //Gets the sync bias
            double GetSyncBias()
            {
                return SyncBias;
            }

            //Sets the motor multiplier so in the config, one control can control more than one motor at different speeds
            void SetMotorSetMult(double Val)
            {
                MotorBias = abs(Val);
            }

            //Returns the motor multiplier
            double GetMotorSetMult()
            {
                return MotorBias;
            }

            //Sets the PDB channel that the motor controller is connected to
            void SetPDBChannel(int val) { PDBPort = val; }
            //Gets the PDB number channel that is set
            int GetPDBChannel() {return PDBPort;}

            //Initializes both the position and ramp PID objects with default values
            void InitProfiles(double Ppos = 0.8, double Ipos = 0.01, double Dpos = 0, double Pspe = 1, double Ispe = 1, double Dspe = 0)
            {
                PositionProfile = new PIDProfile(Ppos, Ipos, Dpos);
                PowerProfile = new PIDProfile(Pspe, Ispe, Dspe);
                PowerProfile->SetMaxChange(0.2);
                PowerProfile->SetBias(50);
            }

            //Initializes the position PID object with default values
            void SetPositonProfile(double Ppos = 0.8, double Ipos = 0.01, double Dpos = 0, double Bias = -1)
            {
                if(PositionProfile != nullptr)
                {
                    delete PositionProfile;
                }
                PositionProfile = new PIDProfile(Ppos, Ipos, Dpos, (Bias < 0 ? Ppos * 100 : Bias));
            }

            //Initializes ramp PID object with default values
            void SetPowerProfile(double Ppos = 0.8, double Ipos = 0.01, double Dpos = 0, double Bias = -1)
            {
                if(PowerProfile != nullptr)
                {
                    delete PowerProfile;
                }
                PowerProfile = new PIDProfile(Ppos, Ipos, Dpos, (Bias < 0 ? Ppos * 100 : Bias));
            }

            //Sets the values of the position PID by using PID profile data
            void SetPositonProfile(ProfileData* Data)
            {
                if(PositionProfile != nullptr)
                {
                    delete PositionProfile;
                }
                PositionProfile = new PIDProfile(Data);
            }

            //Sets the values of the ramp PID by using PID profile data
            void SetPowerProfile(ProfileData* Data)
            {
                if(PowerProfile != nullptr)
                {
                    delete PowerProfile;
                }
                PowerProfile = new PIDProfile(Data);
            }

            //Deletes PIDs and default encoder
            void CleanUpProfiles()
            {
                if(PositionProfile != nullptr)
                {
                    delete PositionProfile;
                }
                if(PowerProfile != nullptr)
                {
                    delete PowerProfile;
                }
                if(ConnectedEncoder != nullptr)
                {
                    delete ConnectedEncoder;
                }
            }

            PIDProfile* GetPositionProfile(){return PositionProfile;};
            PIDProfile* GetPowerProfile(){return PowerProfile;};

            //Sets return condition that will be checked in combo control to be used in logic

            //Combo control will check if linked encoder is within the given angle put in robotvalues in the control in the config and
            //return true in the control if the angle is within the target threshold
            void SetReturnAngle()
            {
                OutputReturn = 1;
            }

            //Combo control will get the current power percentage an compare it the the given values put in robotvalues in the config
            //return true in the control if the power satisfies the conditions given in the config
            void SetReturnPower()
            {
                OutputReturn = 0;
            }

            //returns the number to change how the motor will be used in the logic of a combo control
            int GetReturn()
            {
                return OutputReturn;
            }

            //Sets threshold for the encoder angle check
            void SetThreshold(double Thres)
            {
                Threshold = Thres;
            }

            //returns true if the linked encoder angle is within the target angle threshold
            bool WithinAngle(double Angle)
            {
                return PositionProfile->Inrange(Angle, GetAngle(), Threshold);
            }

            //Gets the current angle of the encoder (-360 to 360 if encoder ticks per rev is set correctly)
            double GetAngle()
            {
                return LinkedEncoder->GetAngle();
            }

            //Sets the default encoder ticks per rev, good for use in talon and spark max motors to get angle and set ticks per rev
            void SetTicksPerRev(double Ticks)
            {
                ConnectedEncoder->SetTicksPerRev(Ticks);
            }

            //Sets the delta time between each loop and controls how the motor will move based on other methods that were called such as SetPercent or SetPower
            void SetD_Time(double D_Time) override
            {
                this->D_Time = D_Time;
                if(Mode == MotorMode::ramp)
                {
                    SetMotorPower(TargetVal, D_Time);
                }
                else if(Mode == MotorMode::rate)
                {
                    if(CurrentVal == FP_ILOGBNAN)
                    {
                        SetMotorSpeed(TargetVal, LinkedEncoder->Get(), D_Time);
                    }
                    else
                    {
                        SetMotorSpeed(TargetVal, CurrentVal, D_Time);
                    }
                }
                else if(Mode == MotorMode::pos)
                {
                    if(CurrentVal == FP_ILOGBNAN)
                    {
                        SetMotorPosition(TargetVal, LinkedEncoder->Get(), D_Time);
                    }
                    else
                    {
                        SetMotorPosition(TargetVal, CurrentVal, D_Time);
                    }
                }
                else
                {
                    PowerProfile->Reset();
                    PositionProfile->Reset();
                    IsFinished = true;
                    SetToMotor(TargetVal);
                }            
            }

        private:
            void SetToMotor(double Power)
            {
                if (CurrentPower != Power)
                {
                    if (!((CurrentPower + PowerTolerance) > Power && (CurrentPower - PowerTolerance) < Power))
                    {
                        CurrentPower = Power;
                        Set(Power);
                    }
                    else if (Power == 0)
                    {
                        CurrentPower = Power;
                        Set(Power);
                    }
                }
            }

            //Sets the mode the motor will move in and the values needed to perform the action
            void SetVals(double Target, double DTime, MotorMode Mode, double Current = FP_ILOGBNAN)
            {
                this->Mode = Mode;
                this->TargetVal = Target;
                this->CurrentVal = Current;
                this->D_Time = DTime;
            }

            //Controls how the motor moves in ramp mode
            void SetMotorPower(double Power, double Time)
            {
                if(Time > 0)
                {
                    if(Power != 0)
                    {
                        if(!PowerProfile->Inrange(Power, Get(), PowerProfile->GetThres()) && Mode == MotorMode::ramp)
                        {
                            double Current = Get();
                            double Change = abs(PowerProfile->CalPIDRamp(Power, Get(), Time)) * (IsReversed ? -1 : 1);
                            Current = Change;
                            IsFinished = false;
                            SetToMotor(Current);
                        }
                        else
                        {
                            IsFinished = true;
                        }
                    }
                    else
                    {
                        IsFinished = true;
                        SetToMotor(0);
                    }
                }
            }

            //Controls how the motor will move in position mode
            void SetMotorPosition(double Target, double Current, double dTime)
            {
                IsFinished = PositionProfile->Inrange(Target, Current, PositionProfile->GetThres());
                SetToMotor(PositionProfile->Calculate(Target, Current, dTime) * (IsReversed ? -1 : 1));
            }

            //Controls how the motor will move when reaching an RPM
            void SetMotorSpeed(double Rate, double CurrentPosition, double Time)
            {
                Rate = (Rate * D_Time) / 60;
                IsFinished = PowerProfile->ReachedSpeed();
                SetToMotor(PowerProfile->CalSpeed(Rate, Get(), CurrentPosition, Time, LinkedEncoder->GetTicksPerRev()) * (IsReversed ? -1 : 1));
            }
        
        public:

            //Sets the motor to ramp mode to a given percent power and delta time
            void SetPower(double Power, double dTime)
            {
                if(Power != CurrentPowerTarget)
                {
                    CurrentPowerTarget = Power;
                    PowerProfile->Reset();
                }
                SetVals(Power, dTime, MotorMode::ramp);
            }

            //Sets the motor to ramp mode to a given percent power and using the delta time between each loop given by the ActiveCollection
            void SetPower(double Power)
            {
                SetPower(Power, D_Time);
            }

            //Sets the motor to position mode and is also given a current position and delta time
            void SetPosition(double Target, double CurrentPosition, double Time)
            {
                SetVals(Target, Time, MotorMode::pos, CurrentPosition);
            }

            //Sets the motor to position mode and using the linked encoder for current position and delta tme
            void SetPosition(double Target, double Time)
            {
                SetVals(Target, Time, MotorMode::pos);
            }

            //Sets the motor to position mode and uses the linked encoder for current position and the delta time given by the ActiveCollection
            void SetPosition(double Target)
            {
                SetPosition(Target, D_Time);
            }

            //Sets the motor to position mode and is also given a current position and delta time
            //returns true when reached
            bool SetToPosition(double Target, double CurrentPosition, double Time)
            {
                SetPosition(Target, CurrentPosition, Time);
                return PositionProfile->Inrange(Target, CurrentPosition, PositionProfile->GetThres());
            }

            //Sets the motor to position mode and using the linked encoder for current position and delta tme
            //returns true when reached
            bool SetToPosition(double Target, double Time)
            {
                return SetToPosition(Target, LinkedEncoder->Get(), Time);
            }

            //Sets the motor to position mode and uses the linked encoder for current position and the delta time given by the ActiveCollection
            //returns true when reached
            bool SetToPosition(double Target)
            {
                return SetToPosition(Target, D_Time);
            }

            //Sets the motor to RPM mode and is given a current position and delta time
            void SetSpeed(double Rate, double CurrentPosition, double Time)
            {
                SetVals(Rate, Time, MotorMode::rate, CurrentPosition);
            }

            //Sets motor to RPM mode and uses linked encoder for current position and delta time
            void SetSpeed(double Rate, double Time)
            {
                SetVals(Rate, Time, MotorMode::rate);
            }

            //Sets motor to RPM mode and uses linked encoder for current position and delta time given by the ActiveCollection
            void SetSpeed(double Rate)
            {
                SetSpeed(Rate, D_Time);
            }

            //Sets the motor to RPM mode and is given a current position and delta time
            //returns true when reached
            bool SetToSpeed(double Rate, double CurrentPosition, double Time)
            {
                SetSpeed(Rate, CurrentPosition, Time);
                return PowerProfile->ReachedSpeed();
            }

            //Sets motor to RPM mode and uses linked encoder for current position and delta time
            //returns true when reached
            bool SetToSpeed(double Rate, double Time)
            {
                return SetToSpeed(Rate, LinkedEncoder->Get(), Time);
            }

            //Sets motor to RPM mode and uses linked encoder for current position and delta time given by the ActiveCollection
            //returns truen when reached
            bool SetToSpeed(double Rate)
            {
                return SetToSpeed(Rate, D_Time);
            }

            //Sets motor to power mode and just sets the motor to the percent power
            void SetPercent(double Power)
            {
                SetVals(Power, 0, MotorMode::raw);
            }

            //Returns true when the motor has reached its target for the mode that was set
            bool IsMotorFinished()
            {
                return IsFinished;
            }

            //Sets the rate to go back to set power when motor is pulling to many amps
            void SetRegenRate(double Rate) {RegenRate = Rate;}
            //Sets the rate to lower power when motor is pulling to many amps
            void SetLowerRate(double Rate) {PersonalLowerRate = Rate;}
            //Sets the time it takes from going from lower rate and the motor then not pulling to many amps to the regen rate where it goes back to its set power
            void SetTimeOut(double Time, double lowerAmount) {TimeTimedOut = Time; LowerAmount += (PersonalLowerRate == 0 ? lowerAmount : PersonalLowerRate);}
            
            //MUST BE CALLED IN SET() IN MOTOR FOR SYNC MOTORS AND PDB PROTECTION AND SIMULATION TO GET CORRECT VALUES
            double CalculateVal(double val)
            {
                if(val == 0)
                {
                    if(PrintOut)
                        Log::General("Stopping Motor: " + name);
                    SetPercent(val);
                }
                SyncMotors(val);
                double ReturnVal = val;
                if (val != 0)
                {
                    if (TimeTimedOut > 0)
                    {
                        TimeTimedOut -= 0.1;
                    }
                    else
                    {
                        if (LowerAmount > 0)
                            LowerAmount -= RegenRate;
                        if (LowerAmount < 0)
                            LowerAmount = 0;
                    }

                    double NewVal = ABSVal(val) - ABSVal(LowerAmount);
                    if (NewVal < 0)
                    {
                        NewVal = 0;
                    }
                    ReturnVal = NewVal * SignVal(val);
                }
                else
                {
                    TimeTimedOut = 0;
                    LowerAmount = 0;
                }
                ReturnVal *= MotorBias;
                OutputTable->PutNumber(name, ReturnVal);
                return ReturnVal;
            }

            virtual double Get() = 0;
            virtual void Set(DoubleSolenoid::Value value) = 0;
            virtual void Set(double val) = 0;
            virtual void DefaultSet() = 0;
            virtual void Stop() = 0;
    };
}

#endif /* SRC_Motor_motor_H_ */