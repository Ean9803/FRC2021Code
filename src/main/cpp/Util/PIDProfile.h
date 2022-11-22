/****************************** Header ******************************\
Class Name: PIDProfile
File Name:	PIDProfile.h
Summary: Class that holds the PID values and calculates things
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ian Poll
Email: irobot9803@gmail.com
\********************************************************************/

#ifndef SRC_PIDProf_H_
#define SRC_PIDProf_H_

#include <iostream>
#include "cmath"
#include "ProfileData.h"

using namespace std;
using namespace frc;

namespace Util
{
    class PIDProfile
    {
        public:
            PIDProfile(double P, double I, double D)
            {
                SetP(P);
                SetI(I);
                SetD(D);
                SetBias(P * 100); //Defalt value has been used and gives an ok result (it depends on what this is used for i.e. drive train or shooter or turret)
            };

			PIDProfile(double P, double I, double D, double Bias)
            {
                SetP(P);
                SetI(I);
                SetD(D);
                SetBias(Bias);
            };

			PIDProfile(ProfileData* Data)
            {
                SetAtt(Data);
            };

			void SetAtt(ProfileData* Data)
			{
				DataProf = Data;
				SetP(Data->Pval);
                SetI(Data->Ival);
                SetD(Data->Dval);
                SetBias(Data->Bias);
				SetMaxChange(Data->Change);
				SetMin(Data->Min);
				SetMax(Data->Max);
				SetInnerMax(Data->InnerMax);
				SetInnerMin(Data->InnerMin);
				SetThres(Data->Thres);
				SetPrint(Data->Print);
				if(FirstTime)
				{
					FirstTime = false;
					Name = Data->Name;
				}
				_TotalE = 0;
                _PrevE = 0;
                _PrevR = 0;
                _ErrorTo = 0;
				LastResult = 0;
				CurrentSpeed = 0;
				LastWheelEncoderVal = 0;
				CurrentRamp = 0;
				SpeedReached = false;
			};

            void SetP(double P)
            {
                Pval = P;
            };

            void SetI(double I)
            {
                Ival = I;
            };

            void SetD(double D)
            {
                Dval = D;
            };

            void SetBias(double BiasVal)
            {
                BiasV = BiasVal;
            };

			void SetMaxChange(double Change)
			{
				MaxChange = Change;
			};

            void SetMax(double Max)
            {
                MaxPower = Max;
            };
            
			void SetMin(double Min)
            {
                MinPower = Min;
            };

            void SetChange(double delta)
            {
                MaxChange = delta;
            };

			void SetInnerMax(double Max)
            {
                InnerMax = Max;
            };

			void SetInnerMin(double Min)
            {
                InnerMin = Min;
            };

			void SetThres(double Thres)
            {
                this->Thres = Thres;
            };

            void Reset()
            {
				Reseted = true;
				if(DataProf != nullptr)
                	SetAtt(DataProf);
				else
				{
					_TotalE = 0;
            	    _PrevE = 0;
        	        _PrevR = 0;
    	            _ErrorTo = 0;
					LastResult = 0;
					CurrentSpeed = 0;
					LastWheelEncoderVal = 0;
					CurrentRamp = 0;
					SpeedReached = false;
				}
            };

            double ABSValue(double val)
        	{
        		if (val < 0)
	        		val *= -1;
		        return val;
	        };

	        double roundValue(double val)
        	{
		        return round((float)val);
        	};

	        bool Inrange(double a, double v, double T)
        	{
		        if (Distance(a, v) <= T)
	        	{
	        		return true;
	        	}
	        	return false;
        	};

        	bool Inrange(double Target, double Value)
        	{
		        if(Distance(Value, Target) == 0)
	        	{
	        		return true;
        		}
        		return false;
        	};

        	double Sign(double Value){
        		if(Value < 0){
		        	return -1;
        		}
        		else{
		        	return 1;
		        }
	        };

	        double Scale(double Value, double Min, double MaxValue)
	        {
		        double A = (ABSValue(Value) / MaxValue) + Min;
		        return A * Sign(Value);
	        };

	        double Constrain(double Value, double Min, double Max)
	        {
		        if((Value) < Min)
		        {
			        return Min;// * Sign(Value);
		        }
		        else if((Value) > Max)
		        {
			        return Max;// * Sign(Value);
		        }
		        else
		        {
			        return Value;
		        }
	        };

	        double PIDCalculae(double P, double I, double D, double& TotalError, double Error, double PrevE, double ChangeInTime)
	        {
		        TotalError += Error * ChangeInTime;
		        return ((P * Error) + (I * TotalError) + (D * ((Error - PrevE) / ChangeInTime)));
	        };

	        double PIDCalculae(double P, double I, double D, double& TotalError, double Error, double PrevE, double ChangeInTime, double& ErrorTo, double Target)
	        {
		        TotalError += Error * ChangeInTime;
		        ErrorTo += (Error - Target) * ChangeInTime;
				double ICal = (I * TotalError);
				double DCal = (D * (ErrorTo));
				double Val = ((P * Error) + ICal + DCal);
		        return Val;
	        };

	        bool BelowMaxRate(double Val1, double Val2, double MaxRate)
	        {
		        return ABSValue(ABSValue(Val1) - ABSValue(Val2)) < MaxRate;
	        };

	        double GetMax(double V1, double V2)
	        {
		        return (V1 > V2 ? V1 : V2);
	        };

	        double GetMin(double V1, double V2)
	        {
		        return (V1 < V2 ? V1 : V2);
	        };

			double CloserTo(double Val, double Min, double Max)
			{
				if(abs((Val) - (Min)) < abs((Val) - (Max)))
					return Min;
				return Max;
			};

			double RoundTo(double Val, int DecimalPlaces)
			{
				int Places = 1;
				for(int i = 0; i < DecimalPlaces; i++)
				{
					Places *= 10;
				}
				int Whole = (int)(Val * Places + 0.5);
				return (double)Whole / (double)Places; 
			};

			double Distance(double Val, double OtherVal)
			{
				return ABSValue(ABSValue(Val) - ABSValue(OtherVal));
			};

            double PIDCal(double P, double I, double D, double Target, double Current, double& LastResult, double& TotalError, double& PrevError, double& ErrorTo, double ChangeInTime, double MaxPower, double MinPower, double MaxChange, double Bias, bool ConstrainVal = true)
	        {
				if(Inrange(Target, Current, Thres))
				{
					return 0;
				}
				double dir = (Sign(Bias) * Sign(Current - Target));
				double Raw = 0;
        		double Result = 0;
				Raw = PIDCalculae(P, I, D, TotalError, abs(Current - Target), PrevError, ChangeInTime, ErrorTo, Target);
				Result = Raw;
        		PrevError = abs(Current - Target);
				Result = Scale(Result, 0, abs(Bias));
				Raw = Scale(Result, 0, abs(Bias));
				//Log::General("+++++++++++++++++++Result after cal: " + to_string(Result));
				if(ConstrainVal)
        			Result = Constrain(Result, MinPower, MaxPower);
				//Log::General("+++++++++++++++++++Result after Scale: " + to_string(Result));
				Result = abs(Result) * dir;
				//Log::General("+++++++++++++++++++Result after Dir: " + to_string(Result));
				if(ConstrainVal)
        			Result += (Sign(Result) > 0 ? InnerMax : InnerMin);
				//Log::General("+++++++++++++++++++Result after InnerBound: " + to_string(Result) + " InnerMax: " + to_string(InnerMax) + " InnerMin: " + to_string(InnerMin));
				if(ConstrainVal)
        			Result = Constrain(Result, MinPower, MaxPower);
				//Log::General("+++++++++++++++++++Result after OuterBound: " + to_string(Result));
        		if(!BelowMaxRate(Result, LastResult, MaxChange))
	        	{
					Log::General("//////////////Sign: " + to_string(dir) + "   ///Bias sign: " + to_string(Sign(Bias)) + "   ///Bias: " + to_string(Bias) + "   ///Error: " + to_string(Sign(Current - Target)));
	        		Log::General("!!!!ERROR:-------------PIDCal for " + Name + " went over max change, Change = " + to_string(ABSValue(ABSValue(Result) - ABSValue(LastResult))) + "! Max change allowed: " + to_string(MaxChange) + " Raw value = " + to_string(Raw));
	        		Result = LastResult;
	        	}
				//Log::General("*******************************Result: " + to_string(Result));
        		LastResult = Result;

				if(Print)
				{
					double E = Current - Target;
					Log::General("PID Name: " + Name + "Error: " + to_string(E) + " Output: " + to_string(Result) + " Dir: " + to_string(dir) + " Bias: " + to_string(Bias) + " P: " + to_string(P) + " I: " + to_string(I) + " D: " + to_string(D) + "\nMax Change: " + to_string(MaxChange) + " Constrain:" + (ConstrainVal ? " Yes" : " No") + " InnerMin: " + to_string(InnerMin) + " InnerMax: " + to_string(InnerMax) + " Min: " + to_string(MaxPower) + " Min: " + to_string(MinPower));
				}

		        return Result;
        	};

            double GetP() {return Pval;};
            double GetI() {return Ival;};
            double GetD() {return Dval;};

            double GetBias() {return BiasV;};
			double GetThres() {return Thres;};
            
            double GetTotalError() {return _TotalE;};
            double GetLastErrorV() {return _PrevE;};
            double GetLastResult() {return _PrevR;};
			double GetErrorTo() {return _ErrorTo;};

			string GetName() {return Name;};
			void SetName(string Name) {this->Name = Name;};
            
            void SetTotalError(double val) { _TotalE = val;};
            void SetLastError(double val) { _PrevE = val;};
            void SetLastResult(double val) { _PrevR = val;};
			void SetErrorTo(double val) { _ErrorTo = val;};

			void SetPrint(bool PrintOut){Print = PrintOut;};

			void SetBackgroundInfo(double TotalError, double LastError, double LastResult, double ErrorTo)
			{
				SetTotalError(TotalError);
				SetLastError(LastError);
				SetLastResult(LastResult);
				SetErrorTo(ErrorTo);
			};

            double Calculate(double Target, double Current, double D_Time, bool ConstrainVal = true)
            {
				return PIDCal(Pval, Ival, Dval, Target, Current, _PrevR, _TotalE, _PrevE, _ErrorTo, D_Time, MaxPower, MinPower, MaxChange, BiasV, ConstrainVal);
            };

			double CalSpeed(double SPEEEED, double MotorPower, double Enc, double D_Time, double Rev = 1)
			{
				if (SPEEEED != 0)
    			{
            		if ((LastWheelEncoderVal) == 0)
            		{
                		LastWheelEncoderVal = (Enc);
            		}
        			double rate = (LastWheelEncoderVal - (Enc)) / Rev;
					double Change = abs(CalPIDRamp((SPEEEED), (rate), D_Time));
					double dir = (Sign(BiasV) * Sign(rate - SPEEEED));
					CurrentSpeed = Change + CurrentSpeed;

					if(!BelowMaxRate(CurrentSpeed, LastResult, MaxChange))
	        		{
	        			Log::General("!!!!ERROR:-------------PIDCal for " + Name + " went over max change for rate, Change = " + to_string(ABSValue(ABSValue(CurrentSpeed) - ABSValue(LastResult))) + "! Max change allowed: " + to_string(MaxChange) + " Raw value = " + to_string(CurrentSpeed));
	    				CurrentSpeed = LastResult;
	    			}
            		
					LastResult = CurrentSpeed;
			        LastWheelEncoderVal = (Enc);
			        
					SpeedReached = Inrange(abs(SPEEEED), abs(rate), 50);
		            return (CurrentSpeed);
    			}
    			else
    			{
					SpeedReached = false;
        			return (0);
    			}
			};

			double CalPIDChange(double Target, double Current, double D_Time, bool ConstrainVal = false)
			{
				double Val = Calculate(Target, Current, D_Time, ConstrainVal);
				double Change = 0;
				if(Reseted)
				{
					Reseted = false;
				}
				else
				{
					Change = (Val - LastPIDResult);
				}
				LastPIDResult = Val;
				return Change;
			};

			double CalPIDRamp(double Target, double Current, double D_Time)
			{
				if(Inrange(Target, Current, Thres))
					return CurrentRamp;
				double Change = abs(CalPIDChange(Target, Current, D_Time, false)) * Sign(Current - Target);
				CurrentRamp += Change;
				return CurrentRamp;
			};

			bool ReachedSpeed() {return SpeedReached;};

            virtual ~PIDProfile(){};

        private:
            double Pval = 0;
            double Ival = 0;
            double Dval = 0;
            double BiasV = 0;

            double _TotalE = 0;
            double _PrevE = 0;
            double _PrevR = 0;
            double _ErrorTo = 0;

            double MinPower = -1;
            double MaxPower = 1;
            double MaxChange = 1.5;

			double InnerMin = 0;
			double InnerMax = 0;
			double Thres = 0.01;

			double LastWheelEncoderVal = 0;
            double LastResult = 0;
			double LastPIDResult = 0;
			double CurrentRamp = 0;

			double CurrentSpeed = 0;

			bool SpeedReached = false;
			bool Reseted = true;
			bool Print = false;

			string Name = "PID";
			ProfileData *DataProf = nullptr;

			bool FirstTime = true;
    };

	
}

#endif