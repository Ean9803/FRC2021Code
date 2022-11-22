/****************************** Header ******************************\
Class Name: (multiple classes)
File Name: FRC2019_Goals.cpp
Summary: All implemented Goals go here. Region statements categorize
types
Project: BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Chris Weeks
Email: chrisrweeks@aol.com
\********************************************************************/

#define _USE_MATH_DEFINES
#include "FRC_Goals.h"

using namespace std;
using namespace Util;

//?HINT ctrl+k then ctrl+0 will collapse all regions
//?ctrl+k then ctrl+j will uncollapse all regions





#pragma region AtomicGoals
#pragma region TimerGoals
/***********************Goal_Wait_ac***********************/
void Goal_Wait_ac::Activate()
{
    m_Status = eActive;
}
Goal::Goal_Status Goal_Wait_ac::Process(double dTime)
{
    if (m_Status == eActive)
    {
        m_currentTime += dTime;
        if (m_currentTime >= m_timeOut)
        {
            Terminate();
            return m_Status = eCompleted;
        }

        return eActive;
    }
    else
    {
        return m_Status;
    }
}

void Goal_Wait_ac::Terminate()
{
}
/***********************Goal_TimeOut***********************/

Goal::Goal_Status Goal_TimeOut::Process(double dTime)
{
    if (m_Status == eActive)
    {
        Log::General("timeOutProcess");
        m_currentTime += dTime;
        SmartDashboard::PutNumber("timeOut",m_currentTime);
        if (m_currentTime >= m_timeOut)
        {
            Terminate();
            return m_Status = eFailed;
        }
        //cout << m_activeCollection->GetNavX()->GetAngle() << endl; //DEBUG
        return eActive;
    }
    else
    {
        return m_Status;
    }
}
void Goal_TimeOut::Terminate()
{
}
/***********************Goal_DriveWithTimer***********************/
Goal::Goal_Status Goal_DriveWithTimer::Process(double dTime)
{
    Log::General("b4");
    if (m_Status == eActive)
    {
        Log::General("processing");
        m_currentTime += dTime;
        SetDrive(m_leftSpeed, m_rightSpeed, m_activeCollection);
        if (m_currentTime >= m_timeOut)
        {

            Terminate();
            return m_Status = eCompleted;
        }
        return m_Status = eActive;
    }
    else
    {
        return m_Status;
    }
}

void Goal_DriveWithTimer::Terminate()
{
    StopDrive(m_activeCollection);
}
#pragma endregion

#pragma region ControllerOverride

void Goal_ControllerOverride::Activate()
{
    m_Status = eActive;
}

Goal::Goal_Status Goal_ControllerOverride::Process(double dTime)
{
    if(eActive)
    {
		if (m_controller == 0)
			TestDriver();
		else if (m_controller == 1)
			TestOperator();
        else
        {
			TestDriver();
			if (m_Status==eActive)
				TestOperator();
        }
    }
	return m_Status;
}

void Goal_ControllerOverride::Terminate()
{
    SetCallbacks(false);
}

void Goal_ControllerOverride::TestDriver()
{
    if (m_IsDriveInUse)
	{
		DriverValueChanged(new SenderEventArgs<Goal_ControllerOverride*>(this));  
		m_Status = eFailed;
	}
}

void Goal_ControllerOverride::TestOperator()
{
	if (m_IsOperatorInUse)
	{
		OperatorValueChanged(new SenderEventArgs<Goal_ControllerOverride*>(this));
		m_Status = eFailed;
	}
}

void Goal_ControllerOverride::SetCallbacks(bool bind)
{
    auto onValueChanged = [&](EventArgs* e) {
        try{
            auto args = (TEventArgs<double, Controls::ControlItem*>*)e;
            if(args->GetSender()->joy->GetPort() == 0){
                m_IsDriveInUse = true;
            }
            if(args->GetSender()->joy->GetPort() == 1){
                m_IsOperatorInUse = true;
            }
        }catch(exception &e){
            Log::Error("Known Exception Thrown in onValueChanged in a ControllerOverride! This can cause fatal Runtime Errors! Check your logs and XML.");
            Log::Error(e.what());
        }catch(...){
            Log::Error("UnknownException Thrown in onValueChanged in ControllerOverride! This can cause fatal Runtime Errors! Check your XML and yell at the programmers!");
	    }
    };

    if(bind){
        for(Event *e : m_activeCollection->EventMap)
            (*e).subscribeFirstPriority(onValueChanged);
    }else{
        for(Event *e : m_activeCollection->EventMap)
            (*e).unsubscribeFirstPriority();
    }
}
#pragma endregion

/**********************ResetSwerveDrive*************************/

void ResetSwerveDrive::Activate()
{
    ManagerDT =  (SwerveManager*)ActiveCol->Get(GetStringData(0));
    if(ManagerDT != nullptr)
    {
        Log::General("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@Reset Swerve Drive@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
        ManagerDT->ResetSwivelEnc();
        ActiveCol->GetNavX()->ResetNav();
    }
    else
    {
        Log::General("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Cannot find SwerveManger!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    }
    m_Status = eCompleted;
}

Goal::Goal_Status ResetSwerveDrive::Process(double dTime)
{
    return m_Status = eCompleted;
}

void ResetSwerveDrive::Terminate()
{
    m_Status = eCompleted;
}

/**********************Goal_StopAllMotors*************************/

void Goal_StopAllMotors::Activate()
{
    for(int i = 0; i < (int)Stringdata.size(); i++)
    {
        Motor* Sub = ActiveCol->GetMotor(GetStringData(i));
        if(Sub != nullptr)
        {
            Log::General("~~~~~~~~~~~~~Stopping Motor: " + GetStringData(i));
            Sub->Stop();
        }
    }
    ActiveCol->EnableAllControlsExcept(vector<string>{});
    Log::General("Stopped given motors");
    m_Status = eCompleted;
}

Goal::Goal_Status Goal_StopAllMotors::Process(double dTime)
{
    return m_Status = eCompleted;
}

void Goal_StopAllMotors::Terminate()
{
    m_Status = eCompleted;
}

/**********************Goal_SetControl*************************/

void Goal_SetControl::Activate()
{
    m_Status = eActive;
}

Goal::Goal_Status Goal_SetControl::Process(double dTime)
{
    
    if(isDriver)
        ActiveCol->GetDriverControl(NameCont)->FeedIn(dTime, Vals);
    else
        ActiveCol->GetOperatorControl(NameCont)->FeedIn(dTime, Vals);
    
    return m_Status = eCompleted;
}

void Goal_SetControl::Terminate()
{
    m_Status = eCompleted;
}

/**********************Goal_SetComponent*************************/

void Goal_SetComponent::Activate()
{
    m_Status = eActive;
}

Goal::Goal_Status Goal_SetComponent::Process(double dTime)
{
    ActiveCol->GetOutput(NameCont)->Set(Vals);
    ActiveCol->GetOutput(NameCont)->UpdateComponent();

    return m_Status = eCompleted;
}

void Goal_SetComponent::Terminate()
{
    m_Status = eCompleted;
}

#pragma region FeedbackLoopGoals


/**********************REVColorSensorV3*************************/

void Goal_REVColorSensorV3::Activate()
{
    m_Status = eActive;
}

Goal::Goal_Status Goal_REVColorSensorV3:: Process(double dTime)
{
    if(CurrentT < MaxT && m_Status == eActive)
    {
         if(TargetString == Color -> Get()){
            Spinner->Set(0.1);
            return m_Status = eActive;
         }
         else if(TargetString == Color -> Get()){
             Spinner->Set(0);
            return m_Status = eCompleted;
         }
         CurrentT += dTime;
    }
    else if(m_Status == eCompleted)
    {
        Spinner->Set(0);
        return m_Status = eCompleted;
    }
    else
    {
        Spinner->Set(0);
        return m_Status = eFailed;
    }
    return m_Status = eActive;
}

void Goal_REVColorSensorV3::Terminate()
{
    m_Status = eInactive;
}

/**********************Goal_MotorPosition**************************/

void Goal_MotorPosition::Activate()
{
    Subject = m_activeCollection->GetMotor(GetStringData(0));
    TargetPos += GetData(0);
    m_Status = eActive;
}

Goal::Goal_Status Goal_MotorPosition::Process(double dTime)
{
    if(m_Status == eActive)
    {
        TargetPos += GetData(0);
        //Log::General("----------------------------Rate: " + to_string(GetData(0)) + " --Running to target: " + to_string(TargetPos) + " --Current: " + to_string(Subject->GetEncoder()->Get()) + "--------");
        if(Subject->SetToPosition(TargetPos, dTime))
            m_Status = eCompleted;
    }
    return m_Status;
}

void Goal_MotorPosition::Terminate()
{
    m_Status = eFailed;
}

/*************************Goal_Position****************************/

void Position::Activate()
{
    Spinner->SetQuadraturePosition(0);
    Bias = 100;
    m_Status = eActive;
}
Goal::Goal_Status Position::Process(double dTime)
{
    if(CurrentT < MaxT)
    {
        CurrentT += dTime;
        if(m_Status == eActive)
        {
            double Error = Spinner->Get() - m_Calculate;
            Spinner->Set(Constrain(PIDCal(P, I, D, TotalE, Error, LastE, dTime, 0.2, 0.05, LastResult, Bias), 0, 0.2));
        }
        if(Inrange(Spinner->Get(), m_Calculate, 100))
        {
            Spinner->Set(0);
            return m_Status = eCompleted;
        }
        else
        {
            return m_Status = eActive;
        }
    }
    else
    {
        Spinner->Set(0);
        if(Inrange(Spinner->Get(), m_Calculate, 100))
        {
            return m_Status = eCompleted;
        }
        else
        {
            return m_Status = eFailed;
        }
    }
}

void Position:: Terminate()
{
    Spinner->Set(0);
    m_Status = eInactive;
}

/**************************Goal_Intake****************************/

void Goal_Intake::Activate()
{
    if(DeployIntake)
        Wrist->SetForward();
    else
        Wrist->SetReverse();
    IntakeMotor->Set(Sped);
    Floor->Set(Sped);
    IndexL->Set(Sped);
    IndexR->Set(Sped);    
    m_Status = eActive;
}

Goal::Goal_Status Goal_Intake::Process(double dTime)
{
    return m_Status = eCompleted;
}

void Goal_Intake::Terminate()
{
    m_Status = eInactive;
}


/**********************Goal_ButtonMove*************************/

void Goal_ButtonMove::Activate()
{
    enc0 ->GetEncoder()->Reset();
    navx = ActiveCol->GetNavX();
	navx -> Reset();
    m_Status = eActive;
    Moving = true;
    Done = false;
    TimePassed = 0;
    NumberAtTarget = 0;
    Log::General("-------------------Distance to: " + to_string(GetStringDataAsDouble(0)));
}

Goal::Goal_Status Goal_ButtonMove::Process(double dTime)
{
   if(!Done && m_Status == eActive)
    {
       {
       if(NumberAtTarget < 5 && TimePassed < GetStringDataAsDouble(1) && (GetStringDataAsDouble(0)) != 0)
    	{
            enc = (enc0->GetEncoder()->Get());
            currentValue = navx->GetNavXAngle(); //get new navx angle
    		//Angle PID
	    	double Error = abs(currentValue);
            double Result = ActiveCol->GetPIDProfile(ActiveCol->GetConfigVariable("CorrectionPID")->StringValue())->Calculate(0, Error, dTime);
            //Distance PID
    		double ErrorE = GetStringDataAsDouble(0) - enc;
            double ResultE = ActiveCol->GetPIDProfile(ActiveCol->GetConfigVariable("DrivePID")->StringValue())->Calculate(abs(GetStringDataAsDouble(0)), abs(enc), dTime);
            Log::General("Error: " + to_string(ErrorE) + " | Dist: " + to_string(GetStringDataAsDouble(0)) + " | ResultE: " + to_string(ResultE));
	    	if(Inrange(0, ErrorE, 0.5))
            {
                NumberAtTarget++;
	    	}
            bool IsNegative = GetStringDataAsDouble(0) < 0;
            ActiveCol->GetDriverControl(ActiveCol->GetConfigVariable("LeftDTControl")->StringValue())->FeedIn(dTime, vector<double> {(ResultE + Result) * (IsNegative ? -1 : 1)});
            ActiveCol->GetDriverControl(ActiveCol->GetConfigVariable("RightDTControl")->StringValue())->FeedIn(dTime, vector<double> {(ResultE - Result) * (IsNegative ? -1 : 1)});
            TimePassed += dTime;
        }
        else if(GetStringDataAsDouble(1) <= TimePassed)
        {
            Done = true;
            Moving = true;
        }
        else
        {
            Done = true;
            Moving = false;
        }
       }
    }
    else
    {
        ActiveCol->GetDriverControl(ActiveCol->GetConfigVariable("LeftDTControl")->StringValue())->Enable();
        ActiveCol->GetDriverControl(ActiveCol->GetConfigVariable("RightDTControl")->StringValue())->Enable();
    }
    if(!Done && Moving)
    {
        return m_Status = eActive;
    }
    else if(Done && !Moving)
    {
        ActiveCol->GetDriverControl(ActiveCol->GetConfigVariable("LeftDTControl")->StringValue())->Enable();
        ActiveCol->GetDriverControl(ActiveCol->GetConfigVariable("RightDTControl")->StringValue())->Enable();
        return m_Status = eCompleted;
    }
    else
    {
        ActiveCol->GetDriverControl(ActiveCol->GetConfigVariable("LeftDTControl")->StringValue())->Enable();
        ActiveCol->GetDriverControl(ActiveCol->GetConfigVariable("RightDTControl")->StringValue())->Enable();
        return m_Status = eCompleted;
    }
}

void Goal_ButtonMove::Terminate()
{
    m_Status = eCompleted;
    ActiveCol->GetDriverControl(ActiveCol->GetConfigVariable("LeftDTControl")->StringValue())->Enable();
    ActiveCol->GetDriverControl(ActiveCol->GetConfigVariable("RightDTControl")->StringValue())->Enable();    
    Log::General("Done Moving");
}

/************************Goal_MoveForward*************************/

void Goal_MoveForward::Activate()
{
    enc0 ->GetEncoder()->Reset();
    navx = m_activeCollection->GetNavX();
	navx -> Reset();
    m_Status = eActive;
    Moving = true;
    Bias = (5000);
    BiasE = (100);
    Log::General("-------------------Distance to: " + to_string(distTo));
}

Goal::Goal_Status Goal_MoveForward::Process(double dTime)
{
    if(!Done && m_Status == eActive)
    {
       {
       if(NumberAtTarget < 5 && TimePassed < TotalTime && (distTo != 0))
    	{
            enc = (enc0->GetEncoder()->Get());
            currentValue = navx->GetNavXAngle(); //get new navx angle
    		//Angle PID
	    	double Error = abs(currentValue);
            double Result = m_activeCollection->GetPIDProfile(m_activeCollection->GetConfigVariable("CorrectionPID")->StringValue())->Calculate(0, Error, dTime);
            //Distance PID
    		double ErrorE = distTo - enc;
            double ResultE = m_activeCollection->GetPIDProfile(m_activeCollection->GetConfigVariable("DrivePID")->StringValue())->Calculate(abs(distTo), abs(enc), dTime);
            Log::General("Error: " + to_string(ErrorE) + " | Dist: " + to_string(distTo) + " | ResultE: " + to_string(ResultE));
	    	if(Inrange(0, ErrorE, 0.5))
            {
                NumberAtTarget++;
	    	}
            m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("LeftDTControl")->StringValue())->FeedIn(dTime, vector<double> {(ResultE + Result) * (IsNegative ? -1 : 1)});
            m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("RightDTControl")->StringValue())->FeedIn(dTime, vector<double> {(ResultE - Result) * (IsNegative ? -1 : 1)});
            TimePassed += dTime;
        }
        else if(TotalTime <= TimePassed)
        {
            Done = true;
            Moving = true;
        }
        else
        {
            Done = true;
            Moving = false;
        }
       }
    }
    else
    {
        m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("LeftDTControl")->StringValue())->FeedIn(dTime, vector<double> {0});
        m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("RightDTControl")->StringValue())->FeedIn(dTime, vector<double> {0});
    }
    if(!Done && Moving)
         return m_Status = eActive;
    else if(Done && !Moving)
         return m_Status = eCompleted;
    else
        return m_Status = eFailed;
    
}

void Goal_MoveForward::Terminate()
{
    m_Status = eCompleted;
    m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("LeftDTControl")->StringValue())->FeedIn(0.01, vector<double> {0});
    m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("RightDTControl")->StringValue())->FeedIn(0.01, vector<double> {0});
    
    Log::General("Done Moving");
}


/*********************Goal_TurnPID**********************************/

void Goal_TurnPIDF::Activate()
{
    navx->ResetNav();
    m_Status = eActive;
    Moving = true;
    Bias = ((P * (RealTarget))*(1.5 * (1000/RealTarget)));
    FrameworkCommunication::GetInstance().SendData("Entered", 1);
    Log::General("Turn to: " + to_string(RealTarget));
}

void Goal_TurnPIDF::SetTarget(double Angle, double TotalT)
{
    IsNegative = Angle < 0;
    RealTarget = (Angle);
    TotalTime = TotalT;
}

Goal::Goal_Status Goal_TurnPIDF::Process(double dTime)
{
    if(!Done && m_Status == eActive)
    {
       if(NumberAtTarget < 10 && TimePassed < TotalTime && (RealTarget != 0))
    	{
            double AngleNavX = (double)navx->GetNavXAngle();
    		currentValue = ABSValue(AngleNavX); //get new navx angle
    		//Angle PIDF
	    	double Error = RealTarget - AngleNavX;
            double Result = (m_activeCollection->GetPIDProfile(m_activeCollection->GetConfigVariable("TurnPID")->StringValue())->Calculate((RealTarget), (AngleNavX), dTime));
            //double Result = (0.025 * (IsNegative ? 1 : -1)) + (PIDCal(P, I, D, totalE, Error, PrevE, dTime, MaxPower, Limit, Pevpower, Bias, ErrorTo, RealTarget) * (IsNegative ? 1 : -1) * (SBias + 2));
            Log::General("Error: " + to_string(Error));
            if(Inrange(0, Error, 3))
            {
		    	NumberAtTarget++;
	    	}
            m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("LeftDTControl")->StringValue())->FeedIn(dTime, vector<double> {Result/* * (IsNegative ? -1 : 1)*/});
            m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("RightDTControl")->StringValue())->FeedIn(dTime, vector<double> {-Result/* * (IsNegative ? -1 : 1)*/});
            //SetNeoDrive(Result, Result, m_activeCollection); //set drive to new powers
            TimePassed += dTime;
        }
        else if(TotalTime <= TimePassed)
        {
            Done = true;
            Moving = true;
        }
        else
        {
            Done = true;
            Moving = false;
        }
    }
    else
    {
        m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("LeftDTControl")->StringValue())->FeedIn(dTime, vector<double> {0});
        m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("RightDTControl")->StringValue())->FeedIn(dTime, vector<double> {0});
            
        //StopDrive(m_activeCollection);
    	//StopNeoDrive(m_activeCollection); //once finished, stop drive
    }
    if(!Done && Moving)
        return m_Status = eActive;
    else if(Done && !Moving)
    {
        Log::General("Time out on Gyro at Angle: " + to_string(navx->GetNavXAngle()) + "| with a target of: " + to_string(RealTarget));
        m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("LeftDTControl")->StringValue())->FeedIn(dTime, vector<double> {0});
        m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("RightDTControl")->StringValue())->FeedIn(dTime, vector<double> {0});
        return m_Status = eCompleted;
    }
    else
    {
        if(TotalTime != 0 && Attempts < 1)
        {
            Log::Error("Time out Turn at Angle: " + to_string(navx->GetNavXAngle()) + "| with a target of: " + to_string(RealTarget) + "! TRYING AGAIN");
            //RealTarget = ABSValue((RealTarget * (IsNegative ? -1 : 1)) - navx->GetNavXAngle());
            Log::Error("New Angle Target: " + to_string(RealTarget));
            NumberAtTarget = TimePassed = 0;
            SBias += 1;
            Attempts++;
            Moving = true;
            Done = false;
            return m_Status = eActive;
        }
        else if(TotalTime != 0 && Attempts >= 1)
        {
            Log::Error("Too many attempts to turn to: " + to_string(RealTarget));
            m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("LeftDTControl")->StringValue())->FeedIn(dTime, vector<double> {0});
            m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("RightDTControl")->StringValue())->FeedIn(dTime, vector<double> {0});
            return m_Status = eFailed;
        }
        else
        {
            Log::General("Skipping Due to Angle near or at zero!");
            m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("LeftDTControl")->StringValue())->FeedIn(dTime, vector<double> {0});
            m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("RightDTControl")->StringValue())->FeedIn(dTime, vector<double> {0});
            return m_Status = eCompleted;
        }
    }
}

void Goal_TurnPIDF::Terminate()
{
    m_Status = eCompleted;
    //StopDrive(m_activeCollection);
    m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("LeftDTControl")->StringValue())->FeedIn(0.01, vector<double> {0});
    m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("RightDTControl")->StringValue())->FeedIn(0.01, vector<double> {0});
    //StopNeoDrive(m_activeCollection);
    Log::General("Done Moving");
}

/********************Goal_SwerveCord*****************/

void Goal_SwerveCord::Activate()
{
    m_Status = eActive;
    Xaxis->SetBackgroundInfo(0, Goal_SwerveCord::GetData(4), Goal_SwerveCord::GetData(5), 0);
    Yaxis->SetBackgroundInfo(0, Goal_SwerveCord::GetData(6), Goal_SwerveCord::GetData(7), 0);
}

void Goal_SwerveCord::Savedata()
{
    if (Xaxis != nullptr)
    {
        Goal_SwerveCord::Setdata(4, Xaxis->GetLastErrorV());
        Goal_SwerveCord::Setdata(5, Xaxis->GetLastResult());
        delete Xaxis;
    }
    if (Yaxis != nullptr)
    {
        Goal_SwerveCord::Setdata(6, Yaxis->GetLastErrorV());
        Goal_SwerveCord::Setdata(7, Yaxis->GetLastResult());
        delete Yaxis;
    }

    Goal_SwerveCord::Setdata(8, Y);
    Goal_SwerveCord::Setdata(9, X);
    //DT->ResetLoc();
    DT->Set(0);
    DT->SetSwivel(0);
}

Goal::Goal_Status Goal_SwerveCord::Process(double dTime)
{
    DT->UpdateSystem(dTime);
    double Xpos = Xaxis->RoundTo(DT->GetSimPos()->X, 2);
    double Ypos = Yaxis->RoundTo(DT->GetSimPos()->Y, 2);
    double XTar = Xaxis->RoundTo(X, 2);
    double YTar = Yaxis->RoundTo(Y, 2);
    
    if (done)
    {
        Goal_SwerveCord::Savedata();
        return m_Status = eCompleted;
    }
    if(m_Status == eActive)
    {
        
        double XPower = Xaxis->Calculate(XTar, Xpos, dTime);
        double YPower = -Yaxis->Calculate(YTar, Ypos, dTime);

        double gyro = m_activeCollection->GetNavX()->GetConstAngle() * M_PI / 180;

        double temp = XPower * cos(gyro) + YPower * sin(gyro);
        YPower = -XPower * sin(gyro) + YPower * cos(gyro);
        XPower = temp;
        
        Log::General("---------------Target Pos: " + to_string(XTar) + "," + to_string(YTar) + " --Pos: " + to_string(Xpos) + "," + to_string(Ypos) + " --Power: " + to_string(XPower) + "," + to_string(YPower));
        
        DT->Set(YPower, XPower, 0);
    }
    if(Xaxis->Inrange(Xpos, XTar, Thres) && Yaxis->Inrange(Ypos, YTar, Thres))
    {
        Log::General("---------------------In Range");
        done = true;
    }
    else
    {
        Log::General("---------------------Distance: " + to_string(Xaxis->Distance(Xpos, XTar)) + "," + to_string(Yaxis->Distance(Ypos, YTar)));
    }
    return m_Status = eActive;
}

void Goal_SwerveCord::Terminate()
{
    Goal_SwerveCord::Savedata();
    m_Status = eCompleted;
}

/********************Goal_CurvePath******************/

void Goal_CurvePath::Activate()
{
    encL->GetEncoder()->Reset();
    encR->GetEncoder()->Reset();
    navx->ResetNav();
    m_Status = eActive;
    Moving = true;
    
    TotalDistance = (abs(AngleTarget) / 360) * (abs(RadiusTarget) * 2 * 3.1415);
    double LeftDist = (abs(AngleTarget) / 360) * ((abs(RadiusTarget) + (Base / 2)) * 2 * 3.1415);
    double RightDist = (abs(AngleTarget) / 360) * ((abs(RadiusTarget) - (Base / 2)) * 2 * 3.1415);

    if(LeftDist > RightDist)
    {
        RightBias = RightDist / LeftDist;
    }
    else if(LeftDist > RightDist)
    {
        LeftBias = LeftDist / RightDist;
    }

    if(AngleTarget > 0)
    {
       double Temp = LeftBias;
       LeftBias = RightBias;
       RightBias = Temp;
    }

    Log::General("Distance to travel: Angle: " + to_string(AngleTarget) + " | Radius: " + to_string(RadiusTarget));
}

Goal::Goal_Status Goal_CurvePath::Process(double dTime)
{
    if(!Done && m_Status == eActive)
    {
        if(TimeAtTarget < 1 && TimePassed < TotalTime)
        {
            double DistError = (encL->GetEncoder()->GetDistance(WheelRadi) + encR->GetEncoder()->GetDistance(WheelRadi)) / 2;
            double SpeedCal = m_activeCollection->GetPIDProfile(PIDName)->Calculate(TotalDistance, DistError, dTime);
            Log::General("-----------" + to_string(TotalDistance) + "----------" + to_string(DistError));
            //Log::General("---------" + to_string(SpeedCal) + "----" + to_string(m_activeCollection->GetPIDProfile(PIDName)->GetP()) + "----" + to_string(m_activeCollection->GetPIDProfile(PIDName)->GetI()) + "----" + to_string(m_activeCollection->GetPIDProfile(PIDName)->GetD()));
            double ResultRight = (RightBias) * SpeedCal;
            double ResultLeft = (LeftBias) * SpeedCal;
            //Log::General("Error Dist: " + to_string(DistError) + " | SpeedCal: " + to_string(SpeedCal) + " | Left: " + to_string(ResultLeft) + " | Right: " + to_string(ResultRight));
            if(Inrange(TotalDistance, DistError, m_activeCollection->GetPIDProfile(PIDName)->GetThres() + 0.05))
            {
                TimeAtTarget += dTime;
	    	}
            m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("LeftDTControl")->StringValue())->FeedIn(dTime, vector<double> {ResultLeft});
            m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("RightDTControl")->StringValue())->FeedIn(dTime, vector<double> {-ResultRight});
            //SetNeoDrive((ResultLeft), -(ResultRight), m_activeCollection);
            TimePassed += dTime;
        }
        else if(TotalTime <= TimePassed)
        {
            Done = true;
            Moving = true;
            Log::General("<!Time Out!>");
        }
        else
        {
            Done = true;
            Moving = false;
        }
    }
    if(!Done && Moving)
         return m_Status = eActive;
    else if(Done && !Moving)
    {
        m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("LeftDTControl")->StringValue())->FeedIn(dTime, vector<double> {0});
        m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("RightDTControl")->StringValue())->FeedIn(dTime, vector<double> {0});
        //StopNeoDrive(m_activeCollection);
        return m_Status = eCompleted;
    }
    else
    {
        m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("LeftDTControl")->StringValue())->FeedIn(dTime, vector<double> {0});
        m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("RightDTControl")->StringValue())->FeedIn(dTime, vector<double> {0});
        //StopNeoDrive(m_activeCollection);
        return m_Status = eFailed;
    }
}

void Goal_CurvePath::Terminate()
{
    m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("LeftDTControl")->StringValue())->FeedIn(0.01, vector<double> {0});
    m_activeCollection->GetDriverControl(m_activeCollection->GetConfigVariable("RightDTControl")->StringValue())->FeedIn(0.01, vector<double> {0});
    //StopNeoDrive(m_activeCollection);
    m_Status = eCompleted;
}

/******************Goal_LimelightTrack***************/

void Goal_LimelightTrack::Activate()
{
    Lime->SetLED(0);
    Lime->SetCamMode(true);
    navx->ResetNav();
    m_Status = eActive;
}

Goal::Goal_Status Goal_LimelightTrack::Process(double dTime)
{
    if(CurrentTime < Time)
    {
        Lime->SetLED(0);
        Lime->SetCamMode(true);
        CurrentTime += dTime;
        if(!Lime->SeesTarget())
        {
            if(SRobo)
            {
                Log::General("Scaning for target");
                SetNeoDrive(Speed, Speed, m_activeCollection);
                return m_Status = eActive;
            }
            else
            {
                Log::General("Didnt See Target");
                Lime->SetLED(1);
                Lime->SetCamMode(false);
                return m_Status = eFailed;
            }
        }
        else
        {
            if(!StartUp)
            {
                Log::General("Found Target");
                StartUp = true;
            }
            double XOff = Lime->HorizontalOffset();
            SetNeoDrive(XOff / 100, XOff / 100, m_activeCollection);
            if(Inrange(ABSValue(XOff), 0, 3))
            {
                T->SetTarget(navx->GetAngle(), Time + 2);
                Log::General("Angle: " + to_string(navx->GetAngle()));
                Lime->SetLED(1);
                Lime->SetCamMode(false);
                return m_Status = eCompleted;
            }
            else
            {
                return m_Status = eActive;
            }
        
        }
    }
    else
    {
        Log::Error("Limelight failed time");
        Lime->SetLED(1);
        Lime->SetCamMode(false);
        return m_Status = eFailed;
    }
    
}

void Goal_LimelightTrack::Terminate()
{
    m_Status = eCompleted;
}

/*********************AutoPath-Goal******************/
void AutoPath::Activate()
{
    Log::General("*******************Path Name: " + NamePath + " ********************");
    
    if(NamePath == "TestRun")
    {
        AddSubgoal(new Goal_MoveForward(m_activeCollection, 1, 1, 5));
        AddSubgoal(new Goal_TurnPIDF(m_activeCollection, 90, 1, 5));
        AddSubgoal(new Goal_MoveForward(m_activeCollection, 1, 1, 5));
        AddSubgoal(new Goal_TurnPIDF(m_activeCollection, 90, 1, 5));
        
        //AddSubgoal(new Goal_Wait_ac(m_activeCollection, 0.5));
        //AddSubgoal(new Goal_TurnPIDF(m_activeCollection, 90, 1, 5));
        AutoPath::Shoot(true, 10);
    }
    else if(NamePath == "Run")
    {
        AddSubgoal(new Goal_MoveForward(m_activeCollection, 5, 1, 5));
    }
    else if(NamePath == "Shoot")
    {
        AddSubgoal(new Goal_MoveForward(m_activeCollection, 4, 1, 5));
        AddSubgoal(new Goal_TurnPIDF(m_activeCollection, -90, 1, 5));
        AddSubgoal(new Goal_MoveForward(m_activeCollection, 3, 1, 5));
        AddSubgoal(new Goal_TurnPIDF(m_activeCollection, -90, 1, 5));
        
        AutoPath::Shoot(true, 10);
    }
    else if(NamePath == "Pos")
    {
        AddSubgoal(new Goal_MoveForward(m_activeCollection, 4, 1, 5));
        AddSubgoal(new Goal_TurnPIDF(m_activeCollection, -90, 1, 5));
        AddSubgoal(new Goal_MoveForward(m_activeCollection, 3, 1, 5));
        AddSubgoal(new Goal_TurnPIDF(m_activeCollection, -90, 1, 5));
    }
    /*
    for(int i = 0; i < lenght; i++)
    {
        if (!IsSwerve)
        {
            AddSubgoal(new Goal_CurvePath(m_activeCollection, "WheelLeftT", "WheelRightT", "CurveControl", Angle[i], Radius[i] * Scale, 10, 1, 23));
        }
        else
        {
            AddSubgoal(new Goal_SwerveCord(m_activeCollection, "SwerveDT", Angle[i] * Scale, Radius[i] * Scale));
        }
        if(Actions[i] != 0)
        {
            if(Actions[i] == 1)
            {
                //intake
                AddSubgoal(new Goal_Intake(m_activeCollection, 0.2, true));
            }
            else if(Actions[i] == 2)
            {
                //stop intake and bring in intake
                AddSubgoal(new Goal_Intake(m_activeCollection, 0, false));
            }
            else if(Actions[i] == 3)
            {
                //Shoot
                AddSubgoal(new Goal_ShooterBunch(m_activeCollection, 0));
            }
            else if(Actions[i] == 4)
            {
                //stop intake and NOT bring in intake
                AddSubgoal(new Goal_Intake(m_activeCollection, 0, true));
            }
            else if(Actions[i] == 5)
            {
                Goal_TurnPIDF* Tu = new Goal_TurnPIDF(m_activeCollection, 0, 0.3, 10, 1);
                AddSubgoal(new Goal_LimelightTrack(m_activeCollection, true, 0.1, Tu, 10));
                //Add Goals in here
                //*****************
                
                //*****************
                AddSubgoal(Tu);
            }
            //Add more cases here
        }
    }
    if (IsSwerve)
    {
        AddSubgoal(new Goal_SwerveCord(m_activeCollection, "SwerveDT", Angle[lenght - 1] * Scale, Radius[lenght - 1] * Scale));
    }
    */
    m_Status = eActive;
}

void AutoPath::Terminate()
{
    Log::General("Auto Finished");
    m_Status = eInactive;
}

#pragma endregion
#pragma endregion

#pragma region CompositeGoals
/***********************Goal_WaitThenDrive***********************/
void Goal_WaitThenDrive::Activate()
{
    AddSubgoal(new Goal_Wait_ac(m_activeCollection, m_waitTime));
    AddSubgoal(new Goal_DriveWithTimer(m_activeCollection, m_leftSpeed, m_rightSpeed, m_driveTime));

    m_Status = eActive;
}


#pragma endregion
//#pragma region MultitaskGoals
#pragma region MultitaskGoals

#pragma endregion

void Goal_ShooterYeet::Activate()
{
    m_Status = eActive;
    ShooterMotor->SetQuadraturePosition(0);
    lastPos = (ShooterMotor->GetQuadraturePosition());
}

Goal::Goal_Status Goal_ShooterYeet::Process(double dTime)
{
    if (m_Status == eActive)
    {
        //TODO: Have Limelight modify m_Speed depending on distance from target
        if(LimeLightTrack)
        {
            m_Speed = 69420;//Hehe
        }
        Bias = ((P * m_Speed) * 5);
        
        if((ShooterMotor->GetQuadraturePosition()) != lastPos && !FirstRun)
        {
            double EncoderValue = (ShooterMotor->GetQuadraturePosition());
            revSpeed = (int)(EncoderValue - LastE);
            {
                double Error = (m_Speed + revSpeed);
                double Result = PIDCalculae(P, I, D, total, Error, PrevE, dTime);
                PrevE = Error;

                Log::Error("Result : " + to_string(Scale(Result, SlowDownBias, (Bias))));
                double SpedSpeed = (IsNegative? Constrain(Scale(Result, SlowDownBias, (Bias)), -m_MaxSpeed, 0) : Constrain(Scale(Result, SlowDownBias, (Bias)), 0, m_MaxSpeed));
                Log::Error("Result Scaled: " + to_string(SpedSpeed));
                Reached = Inrange(revSpeed, m_Speed, 500);
                Shoot_DA_BOOL = Inrange(revSpeed, m_Speed, 50);

                SpedSpeed = (ABSValue(ABSValue(LastResult) - ABSValue(SpedSpeed)) < 0.5? SpedSpeed : LastResult);

                {
                    ShooterMotor -> Set(SpedSpeed);
                    ShooterMotor2 ->Set(SpedSpeed);
                }
                Log::Error("Power Output: " + to_string(SpedSpeed)  + ", Error: " + to_string(Error) + ", Real Power output: " + to_string(ShooterMotor->Get()) + ", Reached: " + to_string((Reached? 1 : 0)));
                Log::Error("");
                LastE = EncoderValue;
                LastSpe = revSpeed;
                LastResult = SpedSpeed;
                lastPos = (ShooterMotor->GetQuadraturePosition());
            }
        }
        else if(FirstRun)
        {
            ShooterMotor -> Set((IsNegative? -0.1 : 0.1));
            ShooterMotor2 ->Set((IsNegative? -0.1 : 0.1));
            LastResult = 0.1;
            FirstRun = false;
        }

        return eActive;
    }
    else if (m_Status = eInactive){
        ShooterMotor -> Set(0);
        ShooterMotor2 ->Set(0);
        return eInactive;
    }
    return eInactive;
}

void Goal_ShooterYeet::Terminate()
{
    ShooterMotor -> Set(0);
    ShooterMotor2 ->Set(0);
    Log::Error("Goal Shooter Yeet Stopped");
    m_Status = eInactive;
}

void Goal_ShooterBunch::Activate()
{
    Log::Error("Start");
    ShootWheel->Activate();
    m_Status = eActive;
    Lime->SetLED(0);
}
//TODO: Controller Overide
Goal::Goal_Status Goal_ShooterBunch::Process(double dTime)
{
    if(numShots < 5 && m_Status == eActive/* && CurrentTime < 10*/)
    {
        Log::Error("Target speed: " + to_string(ShootWheel->m_Speed) + ", Rate: " + to_string(-ShootWheel->revSpeed));
        Log::Error("Reached: " + to_string(ShootWheel->Reached) + ", Number Shots: " + to_string(numShots));
        ShootWheel->Process(dTime);
        if(ShootWheel->Reached)
        {
            Prep = true;
            if(ShootWheel->Shoot_DA_BOOL)
            {
                Shoot = true;
            }
        }
        if(Prep)
        {
            Log::General("Move Index");
            MovingFloor->Set(0);
            IndexL->Set(Speed);
            IndexR->Set(Speed);
            if(Shoot)
            {
                if(Increment)
                {
                    numShots++;
                    Increment = false;
                }
                Valve->SetForward();
                Prep = false;
                Shoot = false;
            }
        }
        else
        {
            MovingFloor->Set(0);
            IndexL->Set(0);
            IndexR->Set(0);
            Valve->SetReverse();
            Increment = true;
        }
        CurrentTime += dTime;
        return m_Status = eActive;
    }
    else
    {
        ShootWheel->ShooterMotor->Set(0);
        ShootWheel->ShooterMotor2->Set(0);
        Valve->SetReverse();
        MovingFloor->Set(0);
        IndexL->Set(0);
        IndexR->Set(0);
        return m_Status = eCompleted;
    }
}

void Goal_ShooterBunch::Terminate()
{
    ShootWheel->Terminate();
    m_Status = eInactive;
    Lime->SetLED(1);
    ShootWheel->ShooterMotor->Set(0);
    ShootWheel->ShooterMotor2->Set(0);
    Valve->SetReverse();
    MovingFloor->Set(0);
    IndexL->Set(0);
    IndexR->Set(0);
    Log::Error("Stop");
}
