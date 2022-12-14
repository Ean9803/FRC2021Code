#include "Global.h"
#ifndef __Use_RobotBase__

#include "Robot.h"
#include <string>
#include <frc/TimedRobot.h>
#include <frc/smartdashboard/SendableChooser.h>

#include <string>
//#include <frc/WPILib.h>
//#include <frc/RobotBase.h>
#include "frc/smartdashboard/Smartdashboard.h"
#include <networktables/NetworkTableEntry.h>
#include <networktables/NetworkTableInstance.h>
#include <frc/DriverStation.h>
#include <frc/livewindow/LiveWindow.h>
#include <frc/shuffleboard/Shuffleboard.h>
#include <hal/DriverStation.h>
#include <networktables/NetworkTable.h>

#include "Config/Config.h"
#include "Config/ActiveCollection.h"
#include "Systems/Drive.h"
#include "Controls/AxisControl.h"
#include "Autonomi/Autons.h"
#include "Util/Log.h"
#include "Goals/FRC_Goals.h"
#include "Goals/GoalSelector.h"
#include "Util/VisionTarget.h"
#include "Limelight/limelight.h"
#include "Util/UtilityFunctions.h"
#include "Util/LinePaths.h"

#include "Util/RobotStatus.h"
#include "Util/FrameworkCommunication.h"
//Simulation includes
#include "Simulation/Modules/Robot/SwerveRobot/Simulator_Interface.h"

using namespace frc;
using namespace System;
using namespace Controls;
using namespace Configuration;
using namespace Autonomi;
using namespace Logger;
using namespace Lime;
using namespace std;

#define VERSION 1    //!< Defines the program version for the entire program.
#define REVISION "A" //!< Defines the revision of this version of the program.

class Robot : public frc::TimedRobot
{
private:
#pragma region _members_
    Drive *m_drive;
    ActiveCollection *m_activeCollection = nullptr; //!< Pointer to the only instantiation of the ActiveCollection Class in the program
    const string m_driveStraight = "drive";
    MultitaskGoal *m_masterGoal;
    MultitaskGoal *m_teleOpMasterGoal;
    std::shared_ptr<NetworkTable> AutoTable = nt::NetworkTableInstance::GetDefault().GetTable("SmartDashboard");
    nt::NetworkTableInstance m_inst; //!Network tables
    shared_ptr<NetworkTable> m_dashboardTable;
    string m_autonOptions[5] = {"DriveStraight", "OnePieceAuto", "TwoPieceAuto", "DEBUG", "NONE"};
    string m_positionOptions[5] = {"Level 1 Left", "Level 1 Center", "Level 1 Right", "Level 2 Left", "Level 2 Right"};
    std::atomic<bool> m_exit = false;
    frc::Timer m_Timer;         //use frc timer to take advantage of stepping in simulation (works fine for actual roboRIO too)
    double m_LastTime = 0.0;    //used for time slices
    double m_simLastTime = 0.0; //for simulation time slices
    Module::Robot::Simulator_Interface m_simulation;
    bool m_IsConfigLoaded = false;
#pragma endregion
    void LoadConfig(bool RobotRunning)
    {
        if (!m_IsConfigLoaded)
        {
            m_IsConfigLoaded = true;
            Config *config = new Config(m_activeCollection, m_drive, "config"); //!< Pointer to the configuration file of the robot
        }

        nt::NetworkTableInstance::GetDefault().GetTable("SmartDashboard")->PutBoolean("RUN_ROBOT", RobotRunning);
    }
    void TimeSlice(bool IsAuton = false)
    {

        const double CurrentTime = m_Timer.GetFPGATimestamp();
#if 1
        const double DeltaTime = CurrentTime - m_LastTime;
#else
        const double DeltaTime = 0.01; //It's best to use synthetic time for simulation to step through code
#endif
        m_LastTime = CurrentTime;
        m_drive->Update(DeltaTime);
    }
    void SimulatorTimeSlice()
    {
        const double CurrentTime = m_Timer.GetFPGATimestamp();
#if 1
        const double DeltaTime = CurrentTime - m_simLastTime;
#else
        const double DeltaTime = 0.01;
#endif
        m_simLastTime = CurrentTime;
        //sanity check
        //frc::SmartDashboard::PutNumber("time_delta",DeltaTime);
        m_simulation.TimeSlice(DeltaTime);
    }

public:
    //defaults to 20ms... really want 10
    Robot() : TimedRobot(10_ms)
    {
        m_activeCollection = new ActiveCollection();
        m_drive = new Drive(m_activeCollection);
        //extend the life of the active region's copy by adding a reference to this variable
        //TODO: Events
    }
    ~Robot()
    {
        delete m_drive;
        m_drive = nullptr;
        delete m_activeCollection;
        m_activeCollection = nullptr;
    }
    void RobotInit() override
    {
        //Log::restartfile();
        FrameworkCommunication::GetInstance();
        nt::NetworkTableInstance::GetDefault().GetTable("SmartDashboard")->PutBoolean("RUN_ROBOT", false);
        nt::NetworkTableInstance::GetDefault().GetTable("SmartDashboard")->PutString("3A_Auto_Selector", "????");
        Robot::LoadConfig(false);
        Log::General("Program Version: " + to_string(VERSION) + " Revision: " + REVISION, true);
        SmartDashboard::init();                          //!< Must have this for smartdashboard to work properly
        m_inst = nt::NetworkTableInstance::GetDefault(); //!< Network tables
        m_dashboardTable = m_inst.GetTable("DASHBOARD_TABLE");
        m_dashboardTable->PutStringArray("AUTON_OPTIONS", m_autonOptions);
        m_dashboardTable->PutStringArray("POSITION_OPTIONS", m_positionOptions);
// Util::FrameworkCommunication::GetInstance().SendData("MESSAGE","yeetus");//? Temporary
//Give active collection to simultion to access motors and encoders:
#ifdef _Use_Timed_Sim_
        m_simulation.ActiveCollection_Init(m_activeCollection);
#endif
    }
    void RobotPeriodic() override
    {
    }
    void TestInit() override
    {
        Robot::LoadConfig(true);

        Util::RobotStatus::GetInstance().NotifyState(Util::RobotState::Test);
    }
    void TestPeriodic() override
    {
        AutoTable->PutNumber("FPGA", m_Timer.GetFPGATimestamp());
        TimeSlice(true);
    }
    void TeleopInit() override
    {
        Robot::LoadConfig(true);
        m_drive->StartUp(m_activeCollection->GetDeadControls());
        Util::RobotStatus::GetInstance().NotifyState(Util::RobotState::Teleop);
        m_activeCollection->ResetSuperior_Goal(); //!< Destroy any pre-existing masterGoal that was not properly disposed of

        Log::General("Teleoperation Started.");
        double LastTime = GetTime();
    }
    void TeleopPeriodic() override
    {
        //Note: I may want to change how this works for this code, unlike example code which uses the same
        //code path for teleop and autonomous... for now it is like the example code
        AutoTable->PutNumber("FPGA", m_Timer.GetFPGATimestamp());
        TimeSlice();
    }
    void DisabledInit() override
    {
        nt::NetworkTableInstance::GetDefault().GetTable("SmartDashboard")->PutBoolean("RUN_ROBOT", false);
        Util::RobotStatus::GetInstance().NotifyState(Util::RobotState::Disabled);
    }
    void DisabledPeriodic() override
    {
        AutoTable->PutNumber("FPGA", m_Timer.GetFPGATimestamp());
        if (nt::NetworkTableInstance::GetDefault().GetTable("SmartDashboard")->GetBoolean("0A-RESET_ROBOT_VALUES", false))
            Robot::LoadConfig(false);
    }

private:
    std::shared_ptr<AutoPath> PathA;

public:
    void AutonomousInit() override
    {
        Log::restartfile();
        Robot::LoadConfig(true);
        Util::RobotStatus::GetInstance().NotifyState(Util::RobotState::Auton);
        string SELECTED_AUTO = "";
        m_drive->StartAuto(m_activeCollection->GetAutoControls());
        if (m_activeCollection->ConfigOverride())
        {
            SELECTED_AUTO = m_activeCollection->GetAuto();
        }
        else
        {
            SELECTED_AUTO = AutoTable->GetString("3A_Auto_Selector", "") + ".txt";
        }

        Log::General("!--------------- " + SELECTED_AUTO + " AUTO Selected---------------!------------Scale: " + to_string(m_activeCollection->GetAutoScale()));
        if (PathA)
            PathA.reset();
        //! DO NOT CALL THE EVENT FOR NOTIFYROBOTSTATE AT THIS TIME!
        PathA = std::make_shared<AutoPath>(m_activeCollection, Map(SELECTED_AUTO, m_activeCollection->IsSwerveDrive()), SELECTED_AUTO, 10, m_activeCollection->IsSwerveDrive(), m_activeCollection->GetAutoScale());
        PathA->Activate();
    }
    void AutonomousPeriodic() override
    {
        //Note: using synthetic time
        AutoTable->PutNumber("FPGA", m_Timer.GetFPGATimestamp());
        TimeSlice();
        PathA->Process(0.01);
    }
    void SimulationInit() override
    {
#ifdef _Use_Timed_Sim_
        m_simulation.SimulationInit();
#endif
    }
    void SimulationPeriodic() override
    {
#ifdef _Use_Timed_Sim_
        AutoTable->PutNumber("FPGA", m_Timer.GetFPGATimestamp());
        SimulatorTimeSlice();
#endif
    }
};

#ifndef RUNNING_FRC_TESTS
int main()
{
    return frc::StartRobot<Robot>();
}
#endif

#endif
