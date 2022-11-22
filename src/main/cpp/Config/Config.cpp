/****************************** Header ******************************\
Class Name: Config
File Name:	Config.h
Summary: 	Manages and loads the configuration file.
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ryan Cooper, Dylan Watson, Chris Weeks
Email: cooper.ryan@centaurisoftware.co, dylantrwatson@gmail.com, chrisrweeks@aol.com
\********************************************************************/

#include "Config.h"
#include <string.h>
#include <stdio.h>
#include <iostream>
/*
#include <filesystem>
namespace fs = std::filesystem;
*/
using namespace std;
using namespace System;
using namespace Configuration;
using namespace pugi;
using namespace frc;

/**
 * Load in the document
 * Config Version and Comment (different from the Robot.cpp Version and Comment)
 * * Verbose Output: Need to give Ian the logging TODO first
 * ? AutonEnabled
 * Secondary Camera Server
 * * Vision Initialization -> need vision working with that
 * Allocate Components
 *TODO: Encoders-> Add encoders to motors
 * DI
 *TODO: *DO-> Write the abstraction classes
 *TODO:*AI-> Write the abstraction classes
 *TODO:*AO-> Write the abstraction classes
 *TODO: Lower and Upper Limit for DAI
 * Motors
 *?	Drive class integration? Probably Post-Season
 * Solenoids
 *?	Relays
 * Potentiometers
 * *Allocate the Joysticks via the XML
 * *Allocate Controls (make another method for this, prolly one for each joystick)
 * 
**/ 
Config::Config(ActiveCollection *_activeCollection, Drive *_drive, string ConfigFile) {
//? make doc a member variable?

	string FileS = ConfigFile + ".xml";

	_activeCollection->DeleteAll();
	_drive->DeleteAll();

	xml_document doc;
	xml_parse_result result;
	if(ConfigFile.size() > 0)
		result = doc.load_file(FileS.c_str());
	else
	{
		Log::General("ConfigFile is \"\" using config.xml!!!");
		FileS = "config.xml";
		result = doc.load_file(FileS.c_str());
	}
	
	m_activeCollection = _activeCollection;
	m_drive = _drive;
	//std::cout << "{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{Current File Path:____" << fs::current_path() << "____}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}\n";
	if (result)
	{
		Log::General("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~XML Config parsed without errors");
		Real = true;
		LoadValues(doc);
	}
	else
	{
		Log::General("description: ");
		Log::General(result.description());
		Log::Error("Cannot load " + FileS + " in robot");
		try
		{
			FileS = "src/main/cpp/Configs/" + FileS;
			xml_parse_result SimResult = doc.load_file(FileS.c_str());
			if(SimResult)
			{
				Log::General("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~(SIM)XML Config parsed without errors");
				Real = false;
				LoadValues(doc);
			}
			else
			{
				Log::General("(SIM) description: ");
				Log::General(result.description());
				Log::Error("Cannot load " + FileS + " in robot");
				Log::General("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~(SIM)XML not found!!!");
			}
		}
		catch(...)
		{
			Log::Error("XML Config and backup config parsed with errors");
			string desc = result.description();
			Log::Error("Error description: " + desc);
			Log::Error("Error offset: " + result.offset);
			Log::Error("No config available, returning to Robot.cpp\nTHIS IS A BIG ERROR!");
			//In simulation we should really get the message across
			#ifdef _Win32
			assert(false);  
			#endif
		}
	}
}

void Config::LoadValues(xml_document &doc){
	xml_node root = doc.child("Robot");
	
	if(root){
		Log::General("Config Root found");
	}
	else{
		Log::General("Robot was not found in Config! I hope you didn't intend to configure anything! Returning to Robot.cpp");
		return;
	}

	#pragma region MetaData

	#pragma region Comp

	bool comp = root.child("Competition").attribute("AtComp").as_bool();
	if(comp)
	{
		Log::atComp = comp;
	}
	else
	{
		Log::atComp = false;
	}

	#pragma endregion Comp

	#pragma region Version

	xml_attribute version = root.child("Version").attribute("version");
	if(version)
		Log::General("Config Version: " + version.as_int(), true);
	else
		Log::Error("Config Version not found");

	#pragma endregion Version

	#pragma region Comment

	xml_attribute comment = root.child("Comment").attribute("comment");
	if(comment){
		string comm = comment.as_string();
		Log::General("Comment: " + comm, true);
	}
	else
		Log::Error("Comment not found");

	#pragma endregion Comment

	/*#pragma region TimeLoop

	xml_attribute Loop = root.child("Time").attribute("seconds");
	if(Loop){
		double timeL = Loop.as_double();
		m_activeCollection->SetLoopTime(timeL);
		Log::General("Loop Time set: " + to_string(timeL), true);
	}
	else
		Log::Error("Comment not found");

	#pragma endregion TimeLoop*/

	#pragma region NavX

	xml_node useNavX = root.child("UseNavX");
	//TODO: Addition of other NavX ports
	if(useNavX){
		if(Config::GetBoolAttribute(useNavX, "value"))
		{
			NavX *Nav = new NavX(!Real);
			m_activeCollection->Add(Nav);
			Nav->SetReturn((NavX::AxisAngle)Config::GetIntAttDic(useNavX, "Return", NavReturnType), Config::GetBoolAttribute(useNavX, "Constant"));
			Nav->SetOutprint(Config::GetBoolAttribute(useNavX, "Print"));
			Log::General("NavX detected");
		}
		else
			Log::General("Failed to load the NavX: Disabling NavX by default");
			
	}
	else
		Log::Error("UseNavX not found. Disabling by default");

	#pragma endregion NavX

	#pragma region PDB

	if (root.child("PDBManager"))
	{
		xml_attribute MaxCur = root.child("PDBManager").attribute("MaxCurrrent");
		xml_attribute TimeOut = root.child("PDBManager").attribute("TimeOut");
		xml_attribute Lower = root.child("PDBManager").attribute("LowerAmountBy");
		xml_attribute Run = root.child("PDBManager").attribute("Update");
		if (MaxCur && TimeOut)
		{
			m_activeCollection->SetPDP(TimeOut.as_double(), MaxCur.as_double(), Lower.as_double(), Run.as_bool());
		}
	}
	#pragma endregion PDB

	#pragma region AutoSelection

	xml_node AtoE = root.child("Selected_Auto");
	if(AtoE)
	{
		xml_attribute Ato = AtoE.attribute("AutoName");
		if(Ato)
		{
			string AtoFile = Ato.as_string();
			Log::General("Selected Auto: " + AtoFile);
			m_activeCollection->SetAuto(AtoFile);
		}

		xml_attribute AtoOverride = AtoE.attribute("OverrideDS");
		if(AtoOverride)
		{
			bool AtoOver = AtoOverride.as_bool();
			m_activeCollection->SetAutoOverride(AtoOver);
		}
		xml_attribute AtoScale = AtoE.attribute("Scale");
		if(AtoScale)
		{
			double Scale = AtoScale.as_double();
			m_activeCollection->SetAutoScale(Scale);
		}
		xml_attribute Drive = AtoE.attribute("Swerve");
		if(Drive)
		{
			bool TypeDrive = Drive.as_bool();
			m_activeCollection->SetIsSwerveDrive(TypeDrive);
		}
	}
	else
	{
		Log::Error("Auto not found");
	}

	#pragma endregion AutoSelection

	#pragma region ColorSen

	xml_node CS = root.child("ColorSensor");
	if(CS){
		REVColorSensorV3 *tmp;
		if(CS.attribute("name"))
			tmp = new REVColorSensorV3(CS.attribute("name").as_string(), Real);
		else
			tmp = new REVColorSensorV3("Color", Real);
		m_activeCollection->Add(tmp);
		tmp->SetOutprint(Config::GetBoolAttribute(CS, "Print"));
		Log::General("Added Color Sensor");
	}
	else{
		Log::Error("Color Sensor definitions not found in config, skipping...");
	}

	#pragma endregion ColorSen

	#pragma region SecondaryCameraServer

	//TODO: make it so we can mess with the camera during the running of the robot: ie, switch which stream we are using 
	xml_node enableSecondaryCamera = root.child("RobotCameraServer");
	if(enableSecondaryCamera)
	{
		//TODO: chris dum
		//CameraServer::GetInstance()->StartAutomaticCapture(0);
		if(enableSecondaryCamera.attribute("enabled").as_bool())
		{
			
			for(xml_node camera = enableSecondaryCamera.first_child(); camera; camera = camera.next_sibling())
			{
				Log::General("first for loop");
				if(camera)
				{
				xml_attribute enabled = camera.attribute("enabled");
				if(enabled.as_bool())
				{
					xml_attribute port = camera.attribute("port");
					xml_attribute fps = camera.attribute("fps");
					xml_attribute width = camera.attribute("width");
					xml_attribute height = camera.attribute("height");
					int iport, ifps, iwidth, iheight;
					if(port)
					{
						iport = port.as_int();
					}
					else
					{
						Log::Error("CAMERA IS MISSING PORT! DISABLED!");
						continue;
					}

					if(fps)
					{
						ifps = fps.as_int();
					}
					else
					{
						Log::Error("Camera FPS Missing! Using default value!");
						ifps = 15;
					}
					
					if(width && height)
					{
						iwidth = width.as_int();
						iheight = height.as_int();
					}
					else
					{
						Log::Error("Camera Width or Height Missing! Using default values!");
						iwidth = 160;
						iheight = 120;
					}
					Log::General("iport" + iport,true);


					 cs::UsbCamera cam = CameraServer::GetInstance()->StartAutomaticCapture(iport);
					 cam.SetFPS(ifps);
					 cam.SetResolution(iwidth,iheight);

					
					//cam;
				}
				else
				{
					Log::General("Camera Disabled");
				}
				}
			}
		}
	}
		
		

	#pragma endregion SecondaryCameraServer

	#pragma region Vision
	shared_ptr<NetworkTable> vision_table = nt::NetworkTableInstance::GetDefault().GetTable("VISION_2019");
	xml_node vision = root.child("Vision");
	if(vision)
	{
		//LS
		if(vision.attribute("LS"))
		{
			vision_table->PutNumber("LS",vision.attribute("LS").as_int());
		}
		else
		{
			Log::Error("Vision LS not found! Vision server does not have proper values to work with and will likely fail!");
		}
		//US
		if(vision.attribute("US"))
		{
			vision_table->PutNumber("US",vision.attribute("US").as_int());
		}
		else
		{
			Log::Error("Vision US not found! Vision server does not have proper values to work with and will likely fail!");
		}
		
		//LH
		if(vision.attribute("LH"))
		{
			vision_table->PutNumber("LH",vision.attribute("LH").as_int());
		}
		else
		{
			Log::Error("Vision LH not found! Vision server does not have proper values to work with and will likely fail!");
		}
		
		//UH
		if(vision.attribute("UH"))
		{
			vision_table->PutNumber("UH",vision.attribute("UH").as_int());
		}
		else
		{
			Log::Error("Vision UH not found! Vision server does not have proper values to work with and will likely fail!");
		}
		
		//LV
		if(vision.attribute("LV"))
		{
			vision_table->PutNumber("LV",vision.attribute("LV").as_int());
		}
		else
		{
			Log::Error("Vision LV not found! Vision server does not have proper values to work with and will likely fail!");
		}
		
		//UV
		if(vision.attribute("UV"))
		{
			vision_table->PutNumber("UV",vision.attribute("UV").as_int());
		}
		else
		{
			Log::Error("Vision UV not found! Vision server does not have proper values to work with and will likely fail!");
		}
		
		//MinA
		if(vision.attribute("MinA"))
		{
			vision_table->PutNumber("MinA",vision.attribute("MinA").as_int());
		}
		else
		{
			Log::Error("Vision MinA not found! Vision server does not have proper values to work with and will likely fail!");
		}
		
		//MaxA
		if(vision.attribute("MaxA"))
		{
			vision_table->PutNumber("MaxA",vision.attribute("MaxA").as_int());
		}
		else
		{
			Log::Error("Vision MaxA not found! Vision server does not have proper values to work with and will likely fail!");
		}
		
		//MaxO
		if(vision.attribute("MaxO"))
		{
			vision_table->PutNumber("MaxO",vision.attribute("MaxO").as_int());
		}
		else
		{
			Log::Error("Vision MaxO not found! Vision server does not have proper values to work with and will likely fail!");
		}
		
		//Lower bound
		if(vision.attribute("LOWER_BOUND"))
		{
			vision_table->PutNumber("LOWER_BOUND",vision.attribute("LOWER_BOUND").as_int());
		}
		else
		{
			Log::Error("Vision Lower bound not found! Vision server does not have proper values to work with and will likely fail!");
		}
		
		//upper bound
		if(vision.attribute("UPPER_BOUND"))
		{
			vision_table->PutNumber("UPPER_BOUND",vision.attribute("UPPER_BOUND").as_int());
		}
		else
		{
			Log::Error("Vision Upper bound not found! Vision server does not have proper values to work with and will likely fail!");
		}
		
		//left bound
		if(vision.attribute("LEFT_BOUND"))
		{
			vision_table->PutNumber("LEFT_BOUND",vision.attribute("LEFT_BOUND").as_int());
		}
		else
		{
			Log::Error("Vision left bound not found! Vision server does not have proper values to work with and will likely fail!");
		}
		
		//right bound
		if(vision.attribute("RIGHT_BOUND"))
		{
			vision_table->PutNumber("RIGHT_BOUND",vision.attribute("RIGHT_BOUND").as_int());
		}
		else
		{
			Log::Error("Vision right bound not found! Vision server does not have proper values to work with and will likely fail!");
		}
		
	}
	else
	{
		Log::Error("Vision not found. Vision server does not have proper values to work with and will likely fail!");
	}
	
	#pragma endregion Vision

	#pragma endregion MetaData

	AllocateComponents(root);

	xml_node controls = root.child("Controls");

	if(controls){
		Log::General("Controls tag found");
	}
	else{
		Log::Error("Control definitions were not found in Config! Returning to Robot.cpp");
		return;
	}

	#pragma region LimeLight

	xml_node LM = root.child("limeLight");
 	if(LM)
	{
		limelight* lime = new limelight(!Real);
		m_activeCollection->Add(lime);
		lime->SetTargetHeight(Config::GetDoubleAttribute(LM, "TargetHeight", 1));
		lime->SetLimeLightHeight(Config::GetDoubleAttribute(LM, "Height", 1));
		lime->SetReturn(Config::GetIntAttDic(LM, "Return", LimeReturnType));
		lime->SetOutprint(Config::GetBoolAttribute(LM, "Print"));
		Log::General("Added Limelight");
	}
	else
	{
		Log::Error("No limelight in RobotConfig...we need that");
	}

	#pragma endregion LimeLight
		
	AllocateDriverControls(controls);
	AllocateOperatorControls(controls);

	xml_node AutoC = root.child("AutoControls");
	if(AutoC)
	{
		for(xml_node ControlSet = AutoC.first_child(); ControlSet; ControlSet = ControlSet.next_sibling())
		{
			vector<string> List = getBindingStringList(Config::GetStringAttribute(ControlSet, "Controls"));
			for (int i = 0; i < (int)List.size(); i++)
			{
				m_activeCollection->AddAutoControl(List[i]);
			}
		}
	}

	xml_node DeadC = root.child("DeadControls");
	if(DeadC)
	{
		for(xml_node ControlSet = DeadC.first_child(); ControlSet; ControlSet = ControlSet.next_sibling())
		{
			vector<string> List = getBindingStringList(Config::GetStringAttribute(ControlSet, "Controls"));
			for (int i = 0; i < (int)List.size(); i++)
			{
				m_activeCollection->AddDeadControl(List[i]);
			}
		}
	}

	xml_node VarsC = root.child("Variables");
	if(VarsC)
	{
		for(xml_node ControlSet = VarsC.first_child(); ControlSet; ControlSet = ControlSet.next_sibling())
		{
			string NameC = ControlSet.name();
			m_activeCollection->AddConfigData(NameC, Config::GetStringAttribute(ControlSet, "String", "N/A", "String Data for " + NameC + " is not present"), Config::GetDoubleAttribute(ControlSet, "Double", 0, "String Data for " + NameC + " is not present"), Config::GetBoolAttribute(ControlSet, "Bool", false, "Bool Data for " + NameC + " is not present"));
		}
	}
}

void Config::AllocateComponents(xml_node &root){
	xml_node robot = root.child("RobotConfig");
	if(!robot){
		Log::General("RobotConfig was not found in Config! I hope you didn't intend to allocate any components! Returning to Config::LoadValues()");
		return;
	}

	//TODO: When/if we create a drivetrain class, we will need drive/aux motors
	//TODO: Look into setting encoders to motors like we used to do in the C# code
	//TODO: Upper and lower limits that can be either AI or DI

	#pragma region VictorSP
	
	xml_node VictorSP = robot.child("VictorSP");
	if(VictorSP){
		for(xml_node victorSp = VictorSP.first_child(); victorSp; victorSp = victorSp.next_sibling()){
			string name = victorSp.name();
			if(Config::HasAttribute(victorSp, "channel", "Failed to load VictorSP " + name + ". This may cause a fatal runtime error!"))
			{
				VictorSPItem *tmp = new VictorSPItem(name, Config::GetIntAttribute(victorSp, "channel"), Config::GetBoolAttribute(victorSp, "reversed"));
				m_activeCollection->Add(tmp);
				Log::General("Added VictorSP " + name + ", Channel: " + Config::GetStringAttribute(victorSp, "channel") + ", Reversed: " + Config::GetStringAttribute(victorSp, "reversed"));
				Config::SetMotorGroup(tmp, victorSp);
				tmp->SetTicksPerRev(Config::GetDoubleAttribute(victorSp, "Ticks", 1));
				tmp->SetThreshold(Config::GetDoubleAttribute(victorSp, "Threshold"));
				tmp->SetMotorSetMult(Config::GetDoubleAttribute(victorSp, "Mult", 1));
				if(Config::GetBoolAttDic(victorSp, "Return", ReturnType))
					tmp->SetReturnAngle();
				else
					tmp->SetReturnPower();
			}
		}
	}
	else{
		Log::Error("VictorSP definitions not found in config, skipping...");
	}

	#pragma endregion VictorSP

	#pragma region VictorSPX

	xml_node VictorSPX = robot.child("VictorSPX");
	if(VictorSPX){
		for(xml_node victorSpx = VictorSPX.first_child(); victorSpx; victorSpx = victorSpx.next_sibling()){
			string name = victorSpx.name();
			if(Config::HasAttribute(victorSpx, "channel", "Failed to load VictorSPX " + name + ". This may cause a fatal runtime error!")){
				VictorSPXItem *tmp = new VictorSPXItem(Config::GetIntAttribute(victorSpx, "channel"), name, Config::GetBoolAttribute(victorSpx, "reversed"));
				m_activeCollection->Add(tmp);
				Log::General("Added VictorSPX " + name + ", Channel: " + Config::GetStringAttribute(victorSpx, "channel") + ", Reversed: " + Config::GetStringAttribute(victorSpx, "reversed"));
				Config::SetMotorGroup(tmp, victorSpx);
				tmp->SetTicksPerRev(Config::GetDoubleAttribute(victorSpx, "Ticks", 1));
				tmp->SetThreshold(Config::GetDoubleAttribute(victorSpx, "Threshold"));
				tmp->SetMotorSetMult(Config::GetDoubleAttribute(victorSpx, "Mult", 1));
				if(Config::GetBoolAttDic(victorSpx, "Return", ReturnType))
					tmp->SetReturnAngle();
				else
					tmp->SetReturnPower();
			}
		}
	}
	else{
		Log::Error("VictorSPX definitions not found in config, skipping...");
	}

#pragma endregion VictorSPX

	#pragma region SparkMax
	xml_node SparkMax = robot.child("SparkMax");
	if(SparkMax){
		for(xml_node sparkMax = SparkMax.first_child(); sparkMax; sparkMax = sparkMax.next_sibling()){
			
			string name = sparkMax.name();
			if(Config::HasAttribute(sparkMax, "channel", "Failed to load SparkMax " + name + ". This may cause a fatal runtime error!"))
			{
				SparkMaxItem *tmp = new SparkMaxItem(Config::GetIntAttribute(sparkMax, "channel"), name, Config::GetIntAttribute(sparkMax, "reversed"), Real);
				m_activeCollection->Add(tmp);
				Log::General("Added SparkMax " + name + ", Channel: " + Config::GetStringAttribute(sparkMax, "channel") + ", Reversed: " + Config::GetStringAttribute(sparkMax, "reversed"));
				Config::SetMotorGroup(tmp, sparkMax);
				tmp->SetTicksPerRev(Config::GetDoubleAttribute(sparkMax, "Ticks", 24));
				tmp->SetThreshold(Config::GetDoubleAttribute(sparkMax, "Threshold"));
				tmp->SetMotorSetMult(Config::GetDoubleAttribute(sparkMax, "Mult", 1));
				if(Config::GetBoolAttDic(sparkMax, "Return", ReturnType))
					tmp->SetReturnAngle();
				else
					tmp->SetReturnPower();
			}
		}
	}
	else{
		Log::Error("SparkMax definitions not found in config, skipping...");
	}

#pragma endregion SparkMax

	#pragma region TalonSRX

	xml_node TalonSRX = robot.child("TalonSRX");
	if(TalonSRX){
		for(xml_node talonSrx = TalonSRX.first_child(); talonSrx; talonSrx = talonSrx.next_sibling()){
			string name = talonSrx.name();
			if(Config::HasAttribute(talonSrx, "channel", "Failed to load TalonSRX " + name + ". This may cause a fatal runtime error!"))
			{
				TalonSRXItem *tmp = new TalonSRXItem(Config::GetIntAttribute(talonSrx, "channel"), name, Config::GetBoolAttribute(talonSrx, "reversed"), Config::GetBoolAttribute(talonSrx, "enableEncoder"), Real);
				m_activeCollection->Add(tmp);
				Log::General("Added TalonSRX " + name + ", Channel: " + Config::GetStringAttribute(talonSrx, "channel") + ", Reversed: " + Config::GetStringAttribute(talonSrx, "reversed") + ", EnableEncoder: " + Config::GetStringAttribute(talonSrx, "enableEncoder"));
				Config::SetMotorGroup(tmp, talonSrx);
				tmp->SetTicksPerRev(Config::GetDoubleAttribute(talonSrx, "Ticks", 1440));
				tmp->SetThreshold(Config::GetDoubleAttribute(talonSrx, "Threshold"));
				tmp->SetMotorSetMult(Config::GetDoubleAttribute(talonSrx, "Mult", 1));
				if(Config::GetBoolAttDic(talonSrx, "Return", ReturnType))
					tmp->SetReturnAngle();
				else
					tmp->SetReturnPower();
			}
		}
	}
	else{
		Log::Error("TalonSRX definitions not found in config, skipping...");
	}

#pragma endregion TalonSRX

	#pragma region Potentiometer

	xml_node Pot = robot.child("Potentiometer");
	if(Pot){
		for(xml_node pot = Pot.first_child(); pot; pot = pot.next_sibling()){
			string name = pot.name();
			if(Config::HasAttribute(pot, "channel", "Failed to load Potentiometer " + name + ". This may cause a fatal runtime error!"))
			{
				PotentiometerItem *tmp = new PotentiometerItem(Config::GetIntAttribute(pot, "channel"), name, Real);
				m_activeCollection->Add(tmp);
				Log::General("Added Potentiometer " + name + ", Channel: " + Config::GetStringAttribute(pot, "channel"));
				tmp->SetTotalVal(Config::GetDoubleAttribute(pot, "Ticks"));
				tmp->SetThreshold(Config::GetDoubleAttribute(pot, "Threshold"));
			}
		}
	}
	else{
		Log::Error("Potentiometer definitions not found in config, skipping...");
	}

	#pragma endregion Potentiometer

	#pragma region Encoder

	xml_node Encoder = robot.child("Encoder");
	if(Encoder){
		for(xml_node encoder = Encoder.first_child(); encoder; encoder = encoder.next_sibling()){
			string name = encoder.name();
			if(Config::HasAttribute(encoder, "aChannel", "Failed to load Encoder " + name + ". This may cause a fatal runtime error!") && Config::HasAttribute(encoder, "bChannel", "Failed to load Encoder " + name + ". This may cause a fatal runtime error!"))
			{
				EncoderItem *tmp = new EncoderItem(name, Config::GetIntAttribute(encoder, "aChannel"), Config::GetIntAttribute(encoder, "bChannel"), Config::GetIntAttribute(encoder, "reversed"), Real);
				m_activeCollection->Add(tmp);
				Log::General("Added Encoder " + name + ", A-Channel: " + Config::GetStringAttribute(encoder, "aChannel") + ", B-Channel: " + Config::GetStringAttribute(encoder, "bChannel") + ", Reversed: " + Config::GetStringAttribute(encoder, "reversed"));
				tmp->SetTicksPerRev(Config::GetDoubleAttribute(encoder, "Ticks"));
				tmp->SetThreshold(Config::GetDoubleAttribute(encoder, "Threshold"));
			}
		}
	}
	else{
		Log::Error("Encoder definitions not found in config, skipping...");
	}

	#pragma endregion Encoder

	#pragma region Servo

	xml_node Servo = robot.child("Servo");
	if(Servo){
		for(xml_node servo = Servo.first_child(); servo; servo = servo.next_sibling()){
			string name = servo.name();
			if(Config::HasAttribute(servo, "port", "Failed to load Servo " + name + ". This may cause a fatal runtime error!") && Config::HasAttribute(servo, "Max", "Failed to load Servo " + name + ". This may cause a fatal runtime error!")){
				ServoItem *tmp = new ServoItem(name, Config::GetIntAttribute(servo, "port"), Config::GetDoubleAttribute(servo, "Max"), Config::GetStringAttribute(servo, "Type").compare("Limited") == 0 ? ServoItem::ServoType::Limited : ServoItem::ServoType::Continuous, Real);
				m_activeCollection->Add(tmp);
				string _print = Config::GetStringAttribute(servo, "Type").compare("Limited") == 0 ? "Limited" : "Continuous" ;
				Log::General("Added Encoder " + name + ", Port: " + Config::GetStringAttribute(servo, "port") + ", Max Angle: " + Config::GetStringAttribute(servo, "Max") + ", Type: " + _print);
				tmp->SetOutprint(Config::GetBoolAttribute(servo, "Print"));
			}
		}
	}
	else{
		Log::Error("Servo definitions not found in config, skipping...");
	}

	#pragma endregion Servo

	#pragma region DoubleSolenoid

	xml_node Solenoid = robot.child("DoubleSolenoid");
	if(Solenoid){
		for(xml_node solenoid = Solenoid.first_child(); solenoid; solenoid = solenoid.next_sibling()){
			string name = solenoid.name();
			xml_attribute fChannel = solenoid.attribute("fChannel");
			xml_attribute rChannel = solenoid.attribute("rChannel");
			bool reversed = solenoid.attribute("reversed").as_bool();
			xml_attribute _default = solenoid.attribute("default");
			DoubleSolenoid::Value _def;
			if(_default){
				if(strcmp(_default.as_string(),"reverse") == 0)
					_def = DoubleSolenoid::Value::kReverse;
				else if(strcmp(_default.as_string(),"forward") == 0)
					_def = DoubleSolenoid::Value::kForward;
				else{
					_def = DoubleSolenoid::Value::kOff;
				}
			}else{
				_def = DoubleSolenoid::Value::kOff;
			}
			string reversed_print = reversed ? "true" : "false";
			if(fChannel && rChannel){
				DoubleSolenoidItem *tmp = new DoubleSolenoidItem(name , fChannel.as_int(), rChannel.as_int(), _def, reversed, Real);
				m_activeCollection->Add(tmp);
				tmp->SetTimeTaken(Config::GetDoubleAttribute(solenoid, "Time", 2));
				tmp->SetTimeRetract(Config::GetDoubleAttribute(solenoid, "TimeIn", Config::GetDoubleAttribute(solenoid, "Time", 2)));
				tmp->SetOutprint(Config::GetBoolAttribute(solenoid, "Print"));
				Log::General("Added DoubleSolenoid " + name + ", F-Channel: " + to_string(fChannel.as_int()) + ", R-Channel: " + to_string(rChannel.as_int()) + ", Default: " + to_string(_def) + ", Reversed: " + reversed_print);
			}
			else{
				Log::Error("Failed to load DoubleSolenoid " + name + ". This may cause a fatal runtime error!");
			}


		}
	}
	else{
		Log::Error("DoubleSolenoid definitions not found in config, skipping...");
	}

#pragma endregion DoubleSolenoid

	#pragma region DigitalInput

	xml_node DI = robot.child("DigitalInput");
	if(DI){
		for(xml_node di = DI.first_child(); di; di = di.next_sibling()){
			string name = di.name();
			xml_attribute channel = di.attribute("channel");
			if(channel){
				DigitalInputItem *tmp = new DigitalInputItem(channel.as_int(), name, Real);
				m_activeCollection->Add(tmp);
				Log::General("Added DigitalInput " + name + ", Channel: " + to_string(channel.as_int()));
				tmp->SetOutprint(Config::GetBoolAttribute(di, "Print"));
			}
			else{
				Log::Error("Failed to load DigitalInput " + name + ". This may cause a fatal runtime error!");
			}
		}
	}
	else{
		Log::Error("DigitalInput definitions not found in config, skipping...");
	}

	#pragma endregion DigitalInput

	#pragma region DigitalSwitch

	xml_node DSw = robot.child("DigitalSwitch");
	if(DSw)
	{
		for(xml_node dSw = DSw.first_child(); dSw; dSw = dSw.next_sibling())
		{
			string name = dSw.name();
			DigitalSwitch *tmp = new DigitalSwitch(name, Config::GetDoubleAttribute(dSw, "Default"));
			m_activeCollection->Add(tmp);
			Log::General("Added DigitalSwitch: " + name);
			tmp->SetOutprint(Config::GetBoolAttribute(dSw, "Print"));
			tmp->SetTimeExtend(Config::GetDoubleAttribute(dSw, "TimeOut", 2));
			tmp->SetTimeRetract(Config::GetDoubleAttribute(dSw, "TimeIn", Config::GetDoubleAttribute(dSw, "TimeOut", 2)));
		}
	}
	else
	{
		Log::Error("DigitalSwitch definitions not found in config, skipping...");
	}

	#pragma endregion DigitalSwitch


	#pragma region SwerveModule

	xml_node Modules = robot.child("SwerveModules");
	if (Modules)
	{
		for(xml_node module = Modules.first_child(); module; module = module.next_sibling())
		{
			string name = module.name();
			string SwivelName = module.attribute("swivel").as_string();
			string WheelName = module.attribute("wheel").as_string();
			if (m_activeCollection->Get(SwivelName) != nullptr && m_activeCollection->Get(WheelName) != nullptr)
			{
				double Ticksperrev = module.attribute("swivelTicks").as_double();
				double WheelTicksperrev = module.attribute("wheelTicks").as_double();
				xml_attribute Location = module.attribute("location");
				if(Location)
				{
					SwerveModule *tmp = new SwerveModule(name, (Motor*)m_activeCollection->Get(SwivelName), (Motor*)m_activeCollection->Get(WheelName), Ticksperrev, WheelTicksperrev);
					tmp->SetLocation(Config::GetLocation(Location.as_string()));
					m_activeCollection->Add(tmp);
					tmp->SetOutprint(Config::GetBoolAttribute(module, "Print"));
					Log::General("Added Swerve Module :" + name);
				}
				else
				{
					Log::Error("Swerve Module " + name + " cannot find location specified in the config!");
				}
			}
			else
			{
				Log::Error("Swerve Module " + name + " cannot find one or both motors specified in the config!");
			}
		}
	}

	#pragma endregion SwerveModule

	#pragma region SwerveManager

	if (Modules)
	{
		xml_node Man = robot.child("SwerveManager");
		if (Man)
		{
			string name = Man.attribute("name").as_string();
			bool WaitB = Man.attribute("wait").as_bool();
			double MaxPower = Man.attribute("Max").as_double();
			xml_attribute SwerveModules = Man.attribute("modules");
			
			double length = Man.attribute("length").as_double();
			double width = Man.attribute("width").as_double();
			if (SwerveModules)
			{
				istringstream ss(SwerveModules.as_string());
    			string word;
				vector<SwerveModule*> Parts;
				bool AllHere = true;
    			while (ss >> word) 
    			{
					if(m_activeCollection->Get(word) != nullptr)
					{
						Parts.push_back((SwerveModule*)m_activeCollection->Get(word));
					}
					else
					{
						AllHere = false;
					}
    			}

				if (AllHere)
				{
					SwerveManager *Manager = new SwerveManager(name, WaitB, Parts, m_activeCollection->GetNavX(), length, width, (Man.attribute("WheelDiameter") ? Man.attribute("WheelDiameter").as_double() : 1), (Man.attribute("Scale") ? Man.attribute("Scale").as_double() : 1));
					Manager->SetMaxPow(MaxPower);
					Manager->SetOutprint(Config::GetBoolAttribute(Man, "Print"));
					m_activeCollection->Add(Manager);
					Log::General("Added Swerve Manager with location");
				}
				else
				{
					Log::Error("One or modules not found!");
				}
			}
			else
			{
				Log::Error("Swerve Manager cannot find one or more swerve modules specified in the code!");
			}
		}
	}

	#pragma endregion SwerveManager

	#pragma region Link_Motor

	xml_node MotorProfiles = robot.child("MotorLinks");
	if(MotorProfiles){
		for(xml_node P = MotorProfiles.first_child(); P; P = P.next_sibling())
		{
			string name = P.name();
			xml_attribute MotorName = P.attribute("Motor");
			xml_attribute ProfileChild = P.attribute("ChildMotor");
			xml_attribute ProfileBias = P.attribute("Bias");
			if(MotorName)
			{
				if(ProfileChild)
				{
					string Name = MotorName.as_string();
					Motor* MotorPtr = (Motor*)m_activeCollection->Get(Name);
					if(MotorPtr != nullptr)
					{
						string ChildName = ProfileChild.as_string();
						Motor* MotorChildPtr = (Motor*)m_activeCollection->Get(ChildName);
						if(MotorChildPtr != nullptr)
						{
							MotorPtr->AddLinkedMotor(MotorChildPtr);
							MotorChildPtr->SetSyncBias(ProfileBias.as_double());
							Log::General("Correctly Set Motor: " + ChildName + " to Motor: " + Name);
						}
						else
						{
							Log::Error("Motor " + ChildName + " does not exist!");
						}
					}
					else
					{
						Log::Error("Motor " + Name + " does not exist!");
					}
				}
				else
				{
					Log::Error(name + " Link not complete!");
				}
			}
			else
			{
				Log::Error(name + " Link not complete!");
			}
		}
	}
	else{
		Log::Error("Motor Link definitions not found in config, skipping...");
	}

	#pragma endregion Link_Motor

	#pragma region _PID

	xml_node Profiles = robot.child("Profiles");
	if(Profiles){
		for(xml_node P = Profiles.first_child(); P; P = P.next_sibling()){
			string name = P.name();
			xml_attribute Pval = P.attribute("P");
			xml_attribute Ival = P.attribute("I");
			xml_attribute Dval = P.attribute("D");
			xml_attribute MaxChange = P.attribute("Change");
			xml_attribute Bias = P.attribute("Bias");
			xml_attribute Min = P.attribute("Min");
			xml_attribute Max = P.attribute("Max");
			xml_attribute IMin = P.attribute("IMin");
			xml_attribute IMax = P.attribute("IMax");
			xml_attribute Thres = P.attribute("Threshold");
			xml_attribute Index = P.attribute("Index");
			if(Pval && Ival && Dval)
			{
				if(MaxChange && Bias)
				{
					if(Min)
					{
						if(Max)
						{
							if(Index)
							{
								if(IMin && IMax && Thres){
									m_activeCollection->CreateAndAddProfile(name, Pval.as_double(), Ival.as_double(), Dval.as_double(), MaxChange.as_double(), Bias.as_double(), Min.as_double(), Max.as_double(), IMin.as_double(), IMax.as_double(), Thres.as_double(), Index.as_int(), GetBoolAttribute(P, "Print"));
								}
								else
									m_activeCollection->CreateAndAddProfile(name, Pval.as_double(), Ival.as_double(), Dval.as_double(), MaxChange.as_double(), Bias.as_double(), Min.as_double(), Max.as_double(), 0, 0, 0.01, Index.as_int(), GetBoolAttribute(P, "Print"));
							}
							else
								m_activeCollection->CreateAndAddProfile(name, Pval.as_double(), Ival.as_double(), Dval.as_double(), MaxChange.as_double(), Bias.as_double(), Min.as_double(), Max.as_double());
						}
						else
							m_activeCollection->CreateAndAddProfile(name, Pval.as_double(), Ival.as_double(), Dval.as_double(), MaxChange.as_double(), Bias.as_double(), Min.as_double());
					}
					else
					{
						m_activeCollection->CreateAndAddProfile(name, Pval.as_double(), Ival.as_double(), Dval.as_double(), MaxChange.as_double(), Bias.as_double());
					}
				}
				else
				{
					m_activeCollection->CreateAndAddProfile(name, Pval.as_double(), Ival.as_double(), Dval.as_double());
				}
			}
			else
			{
				Log::Error("Profile not complete");
			}
		}
	}
	else{
		Log::Error("PID Profiles definitions not found in config, skipping...");
	}

	#pragma endregion _PID

	#pragma region Link_PID_Power

	xml_node PowerProfiles = robot.child("PowerLinks");
	if(PowerProfiles){
		for(xml_node P = PowerProfiles.first_child(); P; P = P.next_sibling()){
			string name = P.name();
			xml_attribute MotorName = P.attribute("Motor");
			xml_attribute ProfileName = P.attribute("Profile");
			xml_attribute ProfileIndex = P.attribute("ProfileIndex");
			if(MotorName)
			{
				if(ProfileName)
				{
					string Name = MotorName.as_string();
					Motor* MotorPtr = (Motor*)m_activeCollection->Get(Name);
					if(MotorPtr != nullptr)
					{
						MotorPtr->SetPowerProfile(m_activeCollection->GetProfile(ProfileName.as_string()));
						Log::General("Correctly Set Motor: " + Name + " to PID numbers from: " + ProfileName.as_string());
					}
					else
					{
						Log::Error("Motor " + Name + " does not exist!");
					}
				}
				else if(ProfileIndex)
				{
					string Name = MotorName.as_string();
					Motor* MotorPtr = (Motor*)m_activeCollection->Get(Name);
					if(MotorPtr != nullptr)
					{
						ProfileData *Data = m_activeCollection->GetProfile(ProfileIndex.as_int());
						MotorPtr->SetPowerProfile(Data);
						Log::General("Correctly Set Motor: " + Name + " to PID numbers from index: " + Data->Name);
					}
					else
					{
						Log::Error("Motor " + Name + " does not exist!");
					}
				}
				else
				{
					Log::Error("Link not complete!");
				}
			}
			else
			{
				Log::Error("Link not complete!");
			}
		}
	}
	else{
		Log::Error("Power Link definitions not found in config, skipping...");
	}

	#pragma endregion Link_PID_Power

	#pragma region Link_PID_Position

	xml_node PositionProfiles = robot.child("PositionLinks");
	if(PositionProfiles){
		for(xml_node P = PositionProfiles.first_child(); P; P = P.next_sibling()){
			string name = P.name();
			xml_attribute MotorName = P.attribute("Motor");
			xml_attribute ProfileName = P.attribute("Profile");
			xml_attribute ProfileIndex = P.attribute("ProfileIndex");
			if(MotorName)
			{
				if(ProfileName)
				{
					string Name = MotorName.as_string();
					Motor* MotorPtr = (Motor*)m_activeCollection->Get(Name);
					if(MotorPtr != nullptr)
					{
						MotorPtr->SetPositonProfile(m_activeCollection->GetProfile(ProfileName.as_string()));
						Log::General("Correctly Set Motor: " + Name + " to PID numbers from: " + ProfileName.as_string());
					}
					else
					{
						Log::Error("Motor " + Name + " does not exist!");
					}
				}
				else if(ProfileIndex)
				{
					string Name = MotorName.as_string();
					Motor* MotorPtr = (Motor*)m_activeCollection->Get(Name);
					if(MotorPtr != nullptr)
					{
						ProfileData *Data = m_activeCollection->GetProfile(ProfileIndex.as_int());
						MotorPtr->SetPositonProfile(Data);
						Log::General("Correctly Set Motor: " + Name + " to PID numbers from index: " + Data->Name);
					}
					else
					{
						Log::Error("Motor " + Name + " does not exist!");
					}
				}
				else
				{
					Log::Error("Link not complete!");
				}
			}
			else
			{
				Log::Error("Link not complete!");
			}
		}
	}
	else{
		Log::Error("Position Link definitions not found in config, skipping...");
	}

	#pragma endregion Link_PID_Position

	#pragma region Link_PID_Collection

	xml_node CollectionProfiles = robot.child("CollectionLinks");
	if(CollectionProfiles){
		for(xml_node P = CollectionProfiles.first_child(); P; P = P.next_sibling()){
			string name = P.name();
			xml_attribute ProfileName = P.attribute("Profile");
			xml_attribute ProfileIndex = P.attribute("ProfileIndex");
			if(ProfileName)
			{
				m_activeCollection->AddPIDProfile(m_activeCollection->GetProfile(ProfileName.as_string()), name);
				string ProfName = ProfileName.as_string();
				Log::General("Correctly added PID numbers from: " + ProfName + " in AC");
			}
			else if(ProfileIndex)
			{
				m_activeCollection->AddPIDProfile(m_activeCollection->GetProfile(ProfileIndex.as_int()), name);
				string ProfIndex = ProfileIndex.as_string();
				Log::General("Correctly added PID numbers from: " + ProfIndex + " in AC");
			}
			else
			{
				Log::Error("Link not complete!");
			}
		}
	}
	else{
		Log::Error("Collection Link definitions not found in config, skipping...");
	}

	#pragma endregion Link_PID_Collection

	#pragma region Encoder_Links

	xml_node EncoderLinks = robot.child("EncoderLinks");
	if(EncoderLinks){
		for(xml_node P = EncoderLinks.first_child(); P; P = P.next_sibling()){
			string name = P.name();
			xml_attribute MotorName = P.attribute("Motor");
			xml_attribute EncoderName = P.attribute("Encoder");
			if(MotorName)
			{
				if(EncoderName)
				{
					string MotName = MotorName.as_string();
					string EncName = EncoderName.as_string();
					if (m_activeCollection->GetEncoder(EncName) != nullptr)
					{
						if(m_activeCollection->Get(MotName) != nullptr)
						{
							((Motor*)m_activeCollection->Get(MotName))->SetLinkedEncoder(m_activeCollection->GetEncoder(EncName));
						}
						else
						{
							Log::Error(MotName + " doesnt exist!");
						}
					}
					else
					{
						if(m_activeCollection->GetMotor(EncName) != nullptr)
						{
							if(m_activeCollection->Get(MotName) != nullptr)
							{
								((Motor*)m_activeCollection->Get(MotName))->SetLinkedEncoder(m_activeCollection->GetMotor(EncName)->GetEncoder());
							}
							else
							{
								Log::Error(MotName + " doesnt exist!");
							}
						}
						else
						{
							Log::Error(EncName + " doesnt exist!");
						}
					}
				}
				else
				{
					Log::Error("Link not complete!");
				}
			}
			else
			{
				Log::Error("Link not complete!");
			}
		}
	}
	else{
		Log::Error("Encoder Link definitions not found in config, skipping...");
	}

	#pragma endregion Encoder_Links

}

void Config::AllocateDriverControls(xml_node &controls){
	xml_node drive = controls.child("Driver");
	if(!drive){
		Log::Error("Drive Control definitions not found in config! Skipping...");
		return;
	}
	int slot = drive.attribute("slot").as_int();
	Log::General("Configured Driver Joystick at slot " + slot, true);
	m_driveJoy = new Joystick(slot);

	#pragma region AxisControl
	
	xml_node AxisControls = drive.child("AxisControls");
	if(AxisControls){
		for(xml_node axis = AxisControls.first_child(); axis; axis = axis.next_sibling()){
			string name = axis.name();
			xml_attribute channel = axis.attribute("axis");
			if(channel){
				bool reversed = axis.attribute("reversed").as_bool();
				bool useOverdrive = axis.attribute("useOverdrive").as_bool();
				double deadZone;
				double multiply;
				xml_attribute deadZone_xml = axis.attribute("deadZone");
				xml_attribute multiply_xml = axis.attribute("powerMultiplier");
				bool isLift = axis.attribute("isLift").as_bool();
				if(!deadZone_xml){
					Log::Error("No DeadZone detected for AxisControl " + name + ". Defaulting to 0.085. This may cause driving errors!");
					deadZone = 0.085;
				}
				else 
					deadZone = deadZone_xml.as_double();
				if(!multiply_xml){
					Log::Error("No Power Multiplier detected for AxisControl " + name + ". Defaulting to 1.0. This may cause driving errors!");
					multiply = 1.0;
				}
				else
					multiply = multiply_xml.as_double();
				AxisControl *tmp = new AxisControl(m_driveJoy, name, channel.as_int(), deadZone, reversed, multiply, m_activeCollection, (axis.attribute("mode") ? axis.attribute("mode").as_int() : 0),  Config::GetBoolAttribute(axis, "useOverdrive"));
				m_drive->AddControlDrive(tmp);
				tmp->SetOutprint(Config::GetBoolAttribute(axis, "Print"));
				string reversed_print = reversed ? "true" : "false" ;
				Log::General("Added AxisControl " + name + ", Axis: " + to_string(channel.as_int()) + ", DeadZone: " + to_string(deadZone) + ", Reversed: " + reversed_print + ", Power Multiplier: " + to_string(multiply) + "Is Lift: " + to_string(isLift));
				xml_attribute bindings = axis.attribute("bindings");
				if(bindings){
					string bind_string = bindings.as_string();
					vector<string> bind_vector = getBindingStringList(bind_string);
					setBindingsToControl(bind_vector, tmp);
				}
				else{
					Log::Error("Control bindings not found for " + name + ". Did you intend to bind this control to anything?");
				}
				if(isLift)
					tmp->SetLift(1.5, m_activeCollection);
				xml_attribute bind_event_xml = axis.attribute("bindEvent");
				bool bind_event = bind_event_xml.as_bool(); 
				if(!bind_event_xml || bind_event){
					m_activeCollection->AddEvent(&(tmp->ValueChanged));
				}
			}
			else{
				Log::Error("Failed to load AxisControl " + name + ". This may cause a fatal runtime error!");
			}
		}
	}
	else{
		Log::Error("Axis Control Driver definitions not found! Skipping...");
	}
		

	#pragma endregion AxisControl

	#pragma region ButtonControl

	xml_node ButtonControls = drive.child("ButtonControls");
	if(ButtonControls){
		for(xml_node button = ButtonControls.first_child(); button; button = button.next_sibling()){
			string name = button.name();
			xml_attribute channel = button.attribute("button");
			if(channel){
				bool reversed = button.attribute("reversed").as_bool();

				double multiply;
				xml_attribute multiply_xml = button.attribute("powerMultiplier");
				bool actOnRelease = button.attribute("actOnRelease").as_bool();
				bool isSolenoid = button.attribute("isSolenoid").as_bool();
				bool isOverdrive = button.attribute("isOverdrive").as_bool();
				if(!multiply_xml){
					Log::Error("No Power Multiplier detected for ButtonControl " + name + ". Defaulting to 1.0. This may cause driving errors!");
					multiply = 1.0;
				}
				else
					multiply = multiply_xml.as_double();
				ButtonControl *tmp = new ButtonControl(m_driveJoy, name, channel.as_int(), actOnRelease, reversed, multiply, isSolenoid, m_activeCollection, (button.attribute("Mode") ? button.attribute("Mode").as_int() : 0), (button.attribute("Hold") ? button.attribute("Hold").as_double() : 2), isOverdrive);
				m_drive->AddControlDrive(tmp);
				tmp->SetOutprint(Config::GetBoolAttribute(button, "Print"));
				string actOnRelease_print = actOnRelease ? "true" : "false";
				string reversed_print = reversed ? "true" : "false";
				string isSolenoid_print = isSolenoid ? "true" : "false";
				string isOverdrive_print = isOverdrive ? "true" : "false";
				Log::General("Added Button Control " + name + ", Button: " + to_string(channel.as_int()) + ", ActOnRelease: " + actOnRelease_print + ", Reversed: " + reversed_print + ", PowerMultiplier: " + to_string(multiply) + ", IsSolenoid: " + isSolenoid_print + ", IsOverdrive: " + isOverdrive_print);
				xml_attribute bindings = button.attribute("bindings");
				if(bindings){
					string bind_string = bindings.as_string();
					vector<string> bind_vector = getBindingStringList(bind_string);
					setBindingsToControl(bind_vector, tmp);
				}
				else{
					Log::Error("Control bindings not found for " + name + ". Did you intend to bind this control to anything?");
				}
				//TODO: make this work lol
				xml_attribute bind_event_xml = button.attribute("bindEvent");
				bool bind_event = bind_event_xml.as_bool(); 
				if(!bind_event_xml || bind_event){
					m_activeCollection->AddEvent(&(tmp->ValueChanged));
				}
			}
			else{
				Log::Error("Failed to load ButtonControl " + name + ". This may cause a fatal runtime error!");
			}
		}
	}
	else{
		Log::Error("Button Control Driver definitions not found! Skipping...");
	}

	#pragma endregion ButtonControl 

	#pragma region POVControl

	xml_node POVControls = drive.child("POVControls");
	if(POVControls){
		for(xml_node axis = POVControls.first_child(); axis; axis = axis.next_sibling()){
			string name = axis.name();
			int Direction = GetIntAttribute(axis, "Direction", 0, "Cannot find POVControl Direction");
			double Mult = GetDoubleAttribute(axis, "powerMultiplier", 1, "Cannot find POVControl powerMultiplier");
			int Mode = GetIntAttribute(axis, "Mode", 0, "Cannot find POVControl Mode");
			int ControlMode = GetIntAttribute(axis, "ControlMode", 0, "Cannot find POVControl Control Mode");
			POVControl *tmp = new POVControl(m_driveJoy, name, Direction, Mult, m_activeCollection, Mode, (POVControl::POVControlMode)ControlMode);
			m_drive->AddControlDrive(tmp);
			vector<string> bind_vector = getBindingStringList(GetStringAttribute(axis, "bindings", "", "Cannot find POVControl bindings"));
			setBindingsToControl(bind_vector, tmp);
			tmp->SetOutprint(Config::GetBoolAttribute(axis, "Print"));
			
			if(GetBoolAttribute(axis, "bindEvent", false, "Cannont find POVControl bindEvent"))
			{
				m_activeCollection->AddEvent(&(tmp->ValueChanged));
			}
		}
	}
	else{
		Log::Error("POV Control Driver definitions not found! Skipping...");
	}

	#pragma endregion POVControl

	#pragma region ToggleButtonControl

	xml_node ToggleButtonControls = drive.child("ToggleButtonControls");
	if(ToggleButtonControls){
		for(xml_node button = ToggleButtonControls.first_child(); button; button = button.next_sibling()){
			string name = button.name();
			xml_attribute channel = button.attribute("button");
			if(channel){
				bool reversed = button.attribute("reversed").as_bool();
				double multiply;
				xml_attribute multiply_xml = button.attribute("powerMultiplier");
				if(!multiply_xml){
					Log::Error("No Power Multiplier detected for ToggleButtonControl " + name + ". Defaulting to 1.0. This may cause driving errors!");
					multiply = 1.0;
				}
				else
					multiply = multiply_xml.as_double();
				ToggleButtonControl *tmp = new ToggleButtonControl(m_driveJoy, name, channel.as_int(), reversed, multiply, Config::GetDoubleAttribute(button, "Negative"), m_activeCollection);
				m_drive->AddControlDrive(tmp);
				tmp->SetOutprint(Config::GetBoolAttribute(button, "Print"));
				xml_attribute bindings = button.attribute("bindings");
				if(bindings){
					string bind_string = bindings.as_string();
					vector<string> bind_vector = getBindingStringList(bind_string);
					setBindingsToControl(bind_vector, tmp);
				}
				else{
					Log::Error("Control bindings not found for " + name + ". Did you intend to bind this control to anything?");
				}
				xml_attribute bind_event_xml = button.attribute("bindEvent");
				bool bind_event = bind_event_xml.as_bool(); 
				if(!bind_event_xml || bind_event){
					m_activeCollection->AddEvent(&(tmp->ValueChanged));
				}
			}
			else{
				Log::Error("Failed to load ToggleButtonControl " + name + ". This may cause a fatal runtime error!");
			}
		}
	}
	else{
		Log::Error("Toggle Button Control Driver definitions not found! Skipping...");
	}	

	#pragma endregion ToggleButtonControl

	#pragma region ServoControl

	xml_node ServoControls = drive.child("ServoControls");
	if(ServoControls){
		for(xml_node button = ServoControls.first_child(); button; button = button.next_sibling()){
			string name = button.name();
			if(Config::HasAttribute(button, "Type", "Failed to load Servo Control " + name + ". This may cause a fatal runtime error!"))
			{

				istringstream ss(Config::GetStringAttribute(button, "Buttons"));
    			string word;
				vector<double> Buttons;
				while (ss >> word) 
    			{
					Buttons.push_back(stod(word));	
    			}

				istringstream sA(Config::GetStringAttribute(button, "Angles"));
    			string Aword;
				vector<double> Angles;
    			while (sA >> Aword) 
    			{
					Angles.push_back(stod(Aword));
    			}

				istringstream sP(Config::GetStringAttribute(button, "POVs"));
    			string Pword;
				vector<double> POV;
    			while (sP >> Pword) 
    			{
					POV.push_back(stod(Pword));
    			}

				ServoControl *tmp = new ServoControl(m_driveJoy, name, m_activeCollection, (ServoControl::ServoControlPart)Config::GetIntAttribute(button, "Type"), Config::GetBoolAttribute(button, "cycle"), Angles, Buttons, Config::GetDoubleAttribute(button, "Axis"), POV, Config::GetDoubleAttribute(button, "Mult"), Config::GetBoolAttribute(button, "Inc"), Config::GetBoolAttribute(button, "Mode"));
				m_drive->AddControlDrive(tmp);
				tmp->SetOutprint(Config::GetBoolAttribute(button, "Print"));
				xml_attribute bindings = button.attribute("bindings");
				if(bindings){
					string bind_string = bindings.as_string();
					vector<string> bind_vector = getBindingStringList(bind_string);
					setBindingsToControl(bind_vector, tmp);
				}
				else{
					Log::Error("Control bindings not found for " + name + ". Did you intend to bind this control to anything?");
				}
				xml_attribute bind_event_xml = button.attribute("bindEvent");
				bool bind_event = bind_event_xml.as_bool(); 
				if(!bind_event_xml || bind_event){
					m_activeCollection->AddEvent(&(tmp->ValueChanged));
				}
			}
		}
	}
	else{
		Log::Error("Servo Control Driver definitions not found! Skipping...");
	}	

	#pragma endregion ServoControl

	#pragma region GoalButtonControl
	
	xml_node GoalButtonControls = drive.child("GoalButtonControls");
	if(GoalButtonControls){
		for(xml_node button = GoalButtonControls.first_child(); button; button = button.next_sibling()){
			string name = button.name();
				xml_attribute goal = button.attribute("goal");
				xml_attribute repeat = button.attribute("repeat");
				xml_attribute ID = button.attribute("ID");
				xml_attribute IndexStart = button.attribute("StartIndex");
				xml_attribute OtherKeys = button.attribute("RemoveKeys");
				xml_attribute AxisNumbers = button.attribute("button");
				xml_attribute StringsData = button.attribute("StringData");
				xml_attribute State = button.attribute("State");
				xml_attribute Mult = button.attribute("mult");
				if(goal && repeat && ID && OtherKeys && AxisNumbers && StringsData && State && Mult){
					TeleOpGoal goalToAdd = getTeleOpGoal(goal.as_string());
					if(goalToAdd != TeleOpGoal::eNone){

						vector<int> RemKeys;
						vector<string> bind_vector_Key = getBindingStringList(OtherKeys.as_string());
						for(int i = 0; i < bind_vector_Key.size(); i++)
							RemKeys.push_back(stoi(bind_vector_Key.at(i)));

						vector<int> Axis;
						vector<string> bind_vector_Axis = getBindingStringList(AxisNumbers.as_string());
						for(int i = 0; i < bind_vector_Axis.size(); i++)
							Axis.push_back(stoi(bind_vector_Axis.at(i)));

						vector<string> StringDataVector = getBindingStringList(StringsData.as_string());

						GoalButtonControl* tmp = new GoalButtonControl(m_driveJoy, name, Axis, m_activeCollection, goalToAdd, StringDataVector, IndexStart.as_int(), ID.as_int(), RemKeys, repeat.as_bool(), (GoalButtonControl::ControlButtonState)GetActivationState(State.as_string()), Mult.as_double());
						m_drive->AddControlDrive(tmp);
						tmp->SetOutprint(Config::GetBoolAttribute(button, "Print"));
						Log::General("Added GoalButtonControl " + name + ", Goal: " + goal.as_string());
						xml_attribute bind_event_xml = button.attribute("bindEvent");
						bool bind_event = bind_event_xml.as_bool(); 
						if(!bind_event_xml || bind_event){
							m_activeCollection->AddEvent(&(tmp->ValueChanged));
						}
					}
					else {
						Log::Error("Failed to load GoalButtonControl " + name + ". This may cause a fatal runtime eror!");
					}
				}
				else{
					Log::Error("Failed to load GoalButtonControl " + name + ". This may cause a fatal runtime eror!");
				}
		}
	}
	else{
		Log::Error("GoalButtonControl Driver definitions not found! Skipping...");
	}

	#pragma endregion

	#pragma region GoalAxisControl
	
	xml_node GoalAxisControls = drive.child("GoalAxisControls");
	if(GoalAxisControls){
		for(xml_node button = GoalAxisControls.first_child(); button; button = button.next_sibling()){
			string name = button.name();
				xml_attribute goal = button.attribute("goal");
				xml_attribute repeat = button.attribute("repeat");
				xml_attribute ID = button.attribute("ID");
				xml_attribute IndexStart = button.attribute("StartIndex");
				xml_attribute OtherKeys = button.attribute("RemoveKeys");
				xml_attribute AxisNumbers = button.attribute("Axis");
				xml_attribute StringsData = button.attribute("StringData");
				xml_attribute DeadZ = button.attribute("DeadZone");
				xml_attribute Mult = button.attribute("mult");
				if(goal && repeat && ID && OtherKeys && AxisNumbers && StringsData && DeadZ && Mult){
					TeleOpGoal goalToAdd = getTeleOpGoal(goal.as_string());
					if(goalToAdd != TeleOpGoal::eNone){

						vector<int> RemKeys;
						vector<string> bind_vector_Key = getBindingStringList(OtherKeys.as_string());
						for(int i = 0; i < bind_vector_Key.size(); i++)
							RemKeys.push_back(stoi(bind_vector_Key.at(i)));

						vector<int> Axis;
						vector<string> bind_vector_Axis = getBindingStringList(AxisNumbers.as_string());
						for(int i = 0; i < bind_vector_Axis.size(); i++)
							Axis.push_back(stoi(bind_vector_Axis.at(i)));

						vector<string> StringDataVector = getBindingStringList(StringsData.as_string());

						GoalAxisControl* tmp = new GoalAxisControl(m_driveJoy, name, Axis, m_activeCollection, goalToAdd, StringDataVector, IndexStart.as_int(), ID.as_int(), RemKeys, repeat.as_bool(), DeadZ.as_double(), Mult.as_double());
						m_drive->AddControlDrive(tmp);
						tmp->SetOutprint(Config::GetBoolAttribute(button, "Print"));
						Log::General("Added GoalAxisControl " + name + ", Goal: " + goal.as_string());
						xml_attribute bind_event_xml = button.attribute("bindEvent");
						bool bind_event = bind_event_xml.as_bool(); 
						if(!bind_event_xml || bind_event){
							m_activeCollection->AddEvent(&(tmp->ValueChanged));
						}
					}
					else {
						Log::Error("Failed to load GoalAxisControl " + name + ". This may cause a fatal runtime eror!");
					}
				}
				else{
					Log::Error("Failed to load GoalAxisControl " + name + ". This may cause a fatal runtime eror!");
				}
		}
	}
	else{
		Log::Error("GoalAxisControl Driver definitions not found! Skipping...");
	}

	#pragma endregion

	#pragma region POVGoalControl

	xml_node GoalPOVControls = drive.child("POVGoalControls");
	if(GoalPOVControls){
		for(xml_node button = GoalPOVControls.first_child(); button; button = button.next_sibling()){
			string name = button.name();
				xml_attribute goal = button.attribute("goal");
				xml_attribute repeat = button.attribute("repeat");
				xml_attribute ID = button.attribute("ID");
				xml_attribute IndexStart = button.attribute("StartIndex");
				xml_attribute OtherKeys = button.attribute("RemoveKeys");
				xml_attribute Dir = button.attribute("Direction");
				xml_attribute StringsData = button.attribute("StringData");
				xml_attribute Mode = button.attribute("Mode");
				xml_attribute State = button.attribute("State");
				xml_attribute Mult = button.attribute("mult");
				if(goal && repeat && ID && OtherKeys && Dir && StringsData && Mode && Mult){
					TeleOpGoal goalToAdd = getTeleOpGoal(goal.as_string());
					if(goalToAdd != TeleOpGoal::eNone){

						vector<int> RemKeys;
						vector<string> bind_vector_Key = getBindingStringList(OtherKeys.as_string());
						for(int i = 0; i < bind_vector_Key.size(); i++)
							RemKeys.push_back(stoi(bind_vector_Key.at(i)));

						vector<string> StringDataVector = getBindingStringList(StringsData.as_string());

						POVGoalControl *tmp = new POVGoalControl(m_driveJoy, name, Dir.as_double(), Mult.as_double(), m_activeCollection, goalToAdd, StringDataVector, IndexStart.as_int(), ID.as_int(), RemKeys, repeat.as_bool(),  (Controls::POVGoalControl::POVGoalControlMode)Mode.as_int(), (Controls::POVGoalControl::POVGoalControlState) State.as_int());
						m_drive->AddControlDrive(tmp);
						tmp->SetOutprint(Config::GetBoolAttribute(button, "Print"));
						Log::General("Added POVGoalControls " + name + ", Goal: " + goal.as_string());
						xml_attribute bind_event_xml = button.attribute("bindEvent");
						bool bind_event = bind_event_xml.as_bool(); 
						if(!bind_event_xml || bind_event){
							m_activeCollection->AddEvent(&(tmp->ValueChanged));
						}
					}
					else {
						Log::Error("Failed to load POVGoalControls " + name + ". This may cause a fatal runtime eror!");
					}
				}
				else{
					Log::Error("Failed to load POVGoalControls " + name + ". This may cause a fatal runtime eror!");
				}
		}
	}
	else{
		Log::Error("POVGoalControls Driver definitions not found! Skipping...");
	}

	#pragma endregion POVGoalControl

	#pragma region SwerveControl

	xml_node Swerve = drive.child("SwerveControl");
	if (Swerve)
	{
		string drivemode = Swerve.attribute("driveMode").as_string();
		string name = Swerve.attribute("name").as_string();
		int H = Swerve.attribute("H-Axis").as_int();
		int V = Swerve.attribute("V-Axis").as_int();
		int S = Swerve.attribute("S-Axis").as_int();
		double dz = Swerve.attribute("deadZone").as_double();
		double mult = Swerve.attribute("powerMultiplier").as_double();
		bool reversed = Swerve.attribute("reversed").as_bool();
		string ManagerName = Swerve.attribute("manager").as_string();

		SwerveControl::DriveCalculation Cal;
		if (drivemode.compare("Field") == 0)
		{
			Cal = SwerveControl::DriveCalculation::Field_Oriented;
		}
		else if (drivemode.compare("Robot") == 0)
		{
			Cal = SwerveControl::DriveCalculation::Robot_Oriented;
		}
		else if (drivemode.compare("Warthog") == 0)
		{
			Cal = SwerveControl::DriveCalculation::Warthog;
		}
		else if (drivemode.compare("Field_Warthog") == 0)
		{
			Cal = SwerveControl::DriveCalculation::Warthog_Field_Oriented;
		}
		else
		{
			Cal = SwerveControl::DriveCalculation::Robot_Oriented;
		}

		if (m_activeCollection->Get(ManagerName) != nullptr)
		{
			SwerveControl *Control = new SwerveControl(m_driveJoy, Cal, name, V, H, S, dz, reversed, mult, m_activeCollection, (SwerveManager*)m_activeCollection->Get(ManagerName));
			m_drive->AddControlDrive(Control);
			Control->SetOutprint(Config::GetBoolAttribute(Swerve, "Print"));
			Log::General("Added swerve control that is " + drivemode + " oriented");
		}
		else
		{
			Log::Error("Swerve control cannot find manager with name " + ManagerName);
		}
	}

	#pragma endregion SwerveControl

	#pragma region Combo

	xml_node ComboControls = drive.child("ComboControls");
	if(ComboControls)
	{
		for(xml_node Ccontrol = ComboControls.first_child(); Ccontrol; Ccontrol = Ccontrol.next_sibling())
		{
			string name = Ccontrol.name();
			if(Config::HasAttribute(Ccontrol, "isGoal", "Cannot tell if control " + name + " is a goal or not...skipping"))
			{
				bool AllHere = true;
				bool useOut = false;
				double Output = 0;

				#pragma region ControlInput
				istringstream ss(Config::GetStringAttribute(Ccontrol, "ControlInput"));
    			string word;
				vector<ControlItem*> Icontrols;
				vector<bool> Negate;
				vector<bool> ABS;
				vector<double> LorG;
				vector<bool> AO;
    			while (ss >> word) 
    			{
					Negate.push_back(false);
					ABS.push_back(false);
					AO.push_back(false);
					LorG.push_back(0);
					for (int i = 0; i < word.length(); i++)
					{
						if(word[i] == '-')
						{
							Negate.back() = !Negate.back();
						}
						else if(word[i] == '&')
						{
							AO.back() = true;
						}
						else if(word[i] == '_')
						{
							AO.back() = false;
						}
						else if(word[i] == '|')
						{
							ABS.back() = !ABS.back();
						}
						else if(word[i] == '>')
						{
							LorG.back() = 1;
						}
						else if(word[i] == '<')
						{
							LorG.back() = -1;
						}
						else if(word[i] == '^')
						{
							LorG.back() = 0;
						}
						else
						{
							word = word.substr(i);
							break;
						}
					}
					
					if(m_activeCollection->GetDriverControl(word) != nullptr)
					{
						Icontrols.push_back(m_activeCollection->GetDriverControl(word));
					}
					else
					{
						AllHere = false;
					}
    			}
				#pragma endregion ControlInput

				#pragma region RobotInput
				istringstream Rs(Config::GetStringAttribute(Ccontrol, "RobotInput"));
    			string Rword;
				vector<bool> RAO;
				vector<NativeComponent*> Rcontrols;
    			while (Rs >> Rword) 
    			{
					RAO.push_back(false);

					for (int i = 0; i < Rword.length(); i++)
					{
						
						if(word[i] == '*')
						{
							RAO.back() = !RAO.back();
						}
						else if(word[i] == '_')
						{
							RAO.back() = !RAO.back();
						}
						
						else
						{
							Rword = Rword.substr(i);
							break;
						}
					}

					if(m_activeCollection->Get(Rword) != nullptr)
					{
						Rcontrols.push_back(m_activeCollection->Get(Rword));
					}
					else
					{
						AllHere = false;
					}
    			}
				#pragma endregion RobotInput

				#pragma region Input
				istringstream Is(Config::GetStringAttribute(Ccontrol, "UseInput"));
    			string Iword;
				vector<ControlItem*> Incontrols;
				vector<bool> INegate;
				vector<bool> IABS;
				vector<double> ILorG;
    			while (Is >> Iword) 
    			{
					INegate.push_back(false);
					IABS.push_back(false);
					ILorG.push_back(0);
					if(!isDouble(Iword))
					{
					for (int i = 0; i < Iword.length(); i++)
					{
						if(Iword[i] == '-')
						{
							INegate.back() = !INegate.back();
						}
						else if(Iword[i] == '|')
						{
							IABS.back() = !IABS.back();
						}
						else if(Iword[i] == '>')
						{
							ILorG.back() = 1;
						}
						else if(Iword[i] == '<')
						{
							ILorG.back() = -1;
						}
						else if(Iword[i] == '^')
						{
							ILorG.back() = 0;
						}
						else
						{
							Iword = Iword.substr(i);
							break;
						}
					}
					}
					else
					{
						Output = stod(Iword);
						useOut = true;
						continue;
					}

					if(m_activeCollection->GetDriverControl(Iword) != nullptr)
					{
						Incontrols.push_back(m_activeCollection->GetDriverControl(Iword));
					}
					else
					{
						AllHere = false;
					}
    			}
				#pragma endregion Input

				#pragma region ValInput
				istringstream IVs(Config::GetStringAttribute(Ccontrol, "InputValues"));
    			string IVword;
				vector<double> IValues;
				vector<bool> NotVal;
    			vector<int> GreaterVal;
    			while (IVs >> IVword) 
    			{
					NotVal.push_back(false);
					GreaterVal.push_back(0);
					for (int i = 0; i < IVword.length(); i++)
					{
						if(IVword[i] == '!')
						{
							NotVal.back() = !NotVal.back();
						}
						else if(IVword[i] == '>')
						{
							GreaterVal.back() = 1;
						}
						else if(IVword[i] == '<')
						{
							GreaterVal.back() = -1;
						}
						else if(IVword[i] == '=')
						{
							GreaterVal.back() = 0;
						}
						else
						{
							IVword = IVword.substr(i);
							break;
						}
					}
					IValues.push_back(stod(IVword));
    			}
				#pragma endregion ValInput

				#pragma region RobotInput
				istringstream IRs(Config::GetStringAttribute(Ccontrol, "RobotValues"));
    			string IRword;
				vector<double> RValues;
				vector<bool> NotRVal;
    			vector<int> GreaterRVal;
    			while (IRs >> IRword) 
    			{
					NotRVal.push_back(false);
					GreaterRVal.push_back(0);
					for (int i = 0; i < IRword.length(); i++)
					{
						if(IRword[i] == '!')
						{
							NotRVal.back() = !NotRVal.back();
						}
						else if(IRword[i] == '>')
						{
							GreaterRVal.back() = 1;
						}
						else if(IRword[i] == '<')
						{
							GreaterRVal.back() = -1;
						}
						else if(IRword[i] == '=')
						{
							GreaterRVal.back() = 0;
						}
						else
						{
							IRword = IRword.substr(i);
							break;
						}
					}
					RValues.push_back(stod(IRword));
    			}
				#pragma endregion RobotInput

				if(AllHere)
				{
					bool Fireone = Config::GetBoolAttribute(Ccontrol, "FireOnce");
					int Mode = Config::GetIntAttribute(Ccontrol, "Mode");
					ComboControl* tmp;
					if(Config::GetBoolAttribute(Ccontrol, "isGoal"))
					{
						vector<int> RemKeys;
						vector<string> bind_vector_Key = getBindingStringList(Config::GetStringAttribute(Ccontrol, "RemoveKeys"));
						for(int i = 0; i < bind_vector_Key.size(); i++)
							RemKeys.push_back(stoi(bind_vector_Key.at(i)));
						
						vector<string> StringDataVector = getBindingStringList(Config::GetStringAttribute(Ccontrol, "StringData"));

						tmp = new ComboControl(m_driveJoy, name, m_activeCollection, Config::GetBoolAttribute(Ccontrol, "primary"), Fireone, true, getTeleOpGoal(Config::GetStringAttribute(Ccontrol, "Goal", "N/A")), StringDataVector, Config::GetIntAttribute(Ccontrol, "StartIndex"), Config::GetIntAttribute(Ccontrol, "ID"), RemKeys, Config::GetBoolAttribute(Ccontrol, "Repeat"), Mode, Icontrols, Rcontrols, Incontrols, IValues, RValues, LorG, ABS, Negate, AO, RAO, ILorG, IABS, INegate, NotRVal, NotVal, GreaterVal, GreaterRVal, useOut, Output);
					}
					else
					{
						tmp = new ComboControl(m_driveJoy, name, m_activeCollection, Config::GetBoolAttribute(Ccontrol, "primary"), Fireone, false, TeleOpGoal::eNone, vector<string> {}, 0, 0, vector<int> {}, false, Mode, Icontrols, Rcontrols, Incontrols, IValues, RValues, LorG, ABS, Negate, AO, RAO, ILorG, IABS, INegate, NotRVal, NotVal, GreaterVal, GreaterRVal, useOut, Output);
					}
					m_drive->AddControlDrive(tmp);
					tmp->SetOutprint(Config::GetBoolAttribute(Ccontrol, "Print"));
					xml_attribute bindings = Ccontrol.attribute("bindings");
					if(bindings){
						string bind_string = bindings.as_string();
						vector<string> bind_vector = getBindingStringList(bind_string);
						setBindingsToControl(bind_vector, tmp);
					}
					else{
						Log::Error("Control bindings not found for " + name + ". Did you intend to bind this control to anything?");
					}
					xml_attribute bind_event_xml = Ccontrol.attribute("bindEvent");
					bool bind_event = bind_event_xml.as_bool(); 
					if(!bind_event_xml || bind_event){
						m_activeCollection->AddEvent(&(tmp->ValueChanged));
					}
				}
				else
				{
					Log::Error("Combo control failed to find some components");
				}
			}
		}
	}

	#pragma endregion Combo
}

void Config::AllocateOperatorControls(xml_node &controls){
	xml_node _operator = controls.child("Operator");
	if(!_operator){
		Log::Error("Operator Control definitions not found in config! Skipping...");
		return;
	}
	int slot = _operator.attribute("slot").as_int();
	Log::General("Configured Operator Joystick at slot " + to_string(slot), true);
	m_operatorJoy = new Joystick(slot);

	#pragma region AxisControl
	
	xml_node AxisControls = _operator.child("AxisControls");
	if(AxisControls){
		for(xml_node axis = AxisControls.first_child(); axis; axis = axis.next_sibling()){
			string name = axis.name();
			xml_attribute channel = axis.attribute("axis");
			if(channel){
				bool reversed = axis.attribute("reversed").as_bool();
				double deadZone;
				double multiply;
				xml_attribute deadZone_xml = axis.attribute("deadZone");
				xml_attribute multiply_xml = axis.attribute("powerMultiplier");
				bool isLift = axis.attribute("isLift").as_bool();
				if(!deadZone_xml){
					Log::Error("No DeadZone detected for AxisControl " + name + ". Defaulting to 0.085. This may cause driving errors!");
					deadZone = 0.085;
				}
				else 
					deadZone = deadZone_xml.as_double();
				if(!multiply_xml){
					Log::Error("No Power Multiplier detected for AxisControl " + name + ". Defaulting to 1.0. This may cause driving errors!");
					multiply = 1.0;
				}
				else
					multiply = multiply_xml.as_double();
				AxisControl *tmp = new AxisControl(m_operatorJoy, name, channel.as_int(), deadZone, reversed, multiply, m_activeCollection, (axis.attribute("mode") ? axis.attribute("mode").as_int() : 0), Config::GetBoolAttribute(axis, "useOverdrive"));
				m_drive->AddControlOperate(tmp);
				tmp->SetOutprint(Config::GetBoolAttribute(axis, "Print"));
				string reversed_print = reversed ? "true" : "false" ;
				Log::General("Added AxisControl " + name + ", Axis: " + to_string(channel.as_int()) + ", DeadZone: " + to_string(deadZone) + ", Reversed: " + reversed_print + ", Power Multiplier: " + to_string(multiply) + "Is Lift: " + to_string(isLift));
				xml_attribute bindings = axis.attribute("bindings");
				if(bindings){
					string bind_string = bindings.as_string();
					vector<string> bind_vector = getBindingStringList(bind_string);
					setBindingsToControl(bind_vector, tmp);
				}
				else{
					Log::Error("Control bindings not found for " + name + ". Did you intend to bind this control to anything?");
				}
				xml_attribute bind_event_xml = axis.attribute("bindEvent");
				bool bind_event = bind_event_xml.as_bool(); 
				if(!bind_event_xml || bind_event){
					m_activeCollection->AddEvent(&(tmp->ValueChanged));
				}
				if(isLift){
					tmp->SetLift(6.9, m_activeCollection);
					Log::General("SET LIFT");
				}
			}
			else{
				Log::Error("Failed to load AxisControl " + name + ". This may cause a fatal runtime error!");
			}
		}
	}
	else{
		Log::Error("Axis Control Operator definitions not found! Skipping...");
	}
		

	#pragma endregion AxisControl

	#pragma region ButtonControl

	xml_node ButtonControls = _operator.child("ButtonControls");
	if(ButtonControls){
		for(xml_node button = ButtonControls.first_child(); button; button = button.next_sibling()){
			string name = button.name();
			xml_attribute channel = button.attribute("button");
			if(channel){
				bool reversed = button.attribute("reversed").as_bool();
				bool useOverdrive = button.attribute("isOverdrive").as_bool();
				double multiply;
				xml_attribute multiply_xml = button.attribute("powerMultiplier");
				bool actOnRelease = button.attribute("actOnRelease").as_bool();
				bool isSolenoid = button.attribute("isSolenoid").as_bool();
				if(!multiply_xml){
					Log::Error("No Power Multiplier detected for ButtonControl " + name + ". Defaulting to 1.0. This may cause driving errors!");
					multiply = 1.0;
				}
				else
					multiply = multiply_xml.as_double();
				ButtonControl *tmp = new ButtonControl(m_operatorJoy, name, channel.as_int(), actOnRelease, reversed, multiply, isSolenoid, m_activeCollection, (button.attribute("Mode") ? button.attribute("Mode").as_int() : 0), (button.attribute("Hold") ? button.attribute("Hold").as_double() : 2), useOverdrive);
				m_drive->AddControlOperate(tmp);
				tmp->SetOutprint(Config::GetBoolAttribute(button, "Print"));
				xml_attribute bindings = button.attribute("bindings");
				if(bindings){
					string bind_string = bindings.as_string();
					vector<string> bind_vector = getBindingStringList(bind_string);
					setBindingsToControl(bind_vector, tmp);
				}
				else{
					Log::Error("Control bindings not found for " + name + ". Did you intend to bind this control to anything?");
				}
				xml_attribute bind_event_xml = button.attribute("bindEvent");
				bool bind_event = bind_event_xml.as_bool(); 
				if(!bind_event_xml || bind_event){
					m_activeCollection->AddEvent(&(tmp->ValueChanged));
				}
			}
			else{
				Log::Error("Failed to load ButtonControl " + name + ". This may cause a fatal runtime error!");
			}
		}
	}
	else{
		Log::Error("Button Control Operator definitions not found! Skipping...");
	}

	#pragma endregion ButtonControl 

	#pragma region POVControl

	xml_node POVControls = _operator.child("POVControls");
	if(POVControls){
		for(xml_node axis = POVControls.first_child(); axis; axis = axis.next_sibling()){
			string name = axis.name();
			int Direction = GetIntAttribute(axis, "Direction", 0, "Cannot find POVControl Direction");
			double Mult = GetDoubleAttribute(axis, "powerMultiplier", 1, "Cannot find POVControl powerMultiplier");
			int Mode = GetIntAttribute(axis, "Mode", 0, "Cannot find POVControl Mode");
			int ControlMode = GetIntAttribute(axis, "ControlMode", 0, "Cannot find POVControl Control Mode");
			POVControl *tmp = new POVControl(m_operatorJoy, name, Direction, Mult, m_activeCollection, Mode, (POVControl::POVControlMode)ControlMode);
			m_drive->AddControlOperate(tmp);
			vector<string> bind_vector = getBindingStringList(GetStringAttribute(axis, "bindings", "", "Cannot find POVControl bindings"));
			setBindingsToControl(bind_vector, tmp);
			tmp->SetOutprint(Config::GetBoolAttribute(axis, "Print"));
			if(GetBoolAttribute(axis, "bindEvent", false, "Cannont find POVControl bindEvent"))
			{
				m_activeCollection->AddEvent(&(tmp->ValueChanged));
			}
		}
	}
	else{
		Log::Error("POV Control Operator definitions not found! Skipping...");
	}

	#pragma endregion POVControl

	#pragma region ToggleButtonControl

	xml_node ToggleButtonControls = _operator.child("ToggleButtonControls");
	if(ToggleButtonControls){
		for(xml_node button = ToggleButtonControls.first_child(); button; button = button.next_sibling()){
			string name = button.name();
			xml_attribute channel = button.attribute("button");
			if(channel){
				bool reversed = button.attribute("reversed").as_bool();
				double multiply;
				xml_attribute multiply_xml = button.attribute("powerMultiplier");
				if(!multiply_xml){
					Log::Error("No Power Multiplier detected for ToggleButtonControl " + name + ". Defaulting to 1.0. This may cause driving errors!");
					multiply = 1.0;
				}
				else
					multiply = multiply_xml.as_double();
				ToggleButtonControl *tmp = new ToggleButtonControl(m_operatorJoy, name, channel.as_int(), reversed, multiply, Config::GetDoubleAttribute(button, "Negative"), m_activeCollection);
				m_drive->AddControlOperate(tmp);
				tmp->SetOutprint(Config::GetBoolAttribute(button, "Print"));
				xml_attribute bindings = button.attribute("bindings");
				if(bindings){
					string bind_string = bindings.as_string();
					vector<string> bind_vector = getBindingStringList(bind_string);
					setBindingsToControl(bind_vector, tmp);
				}
				else{
					Log::Error("Control bindings not found for " + name + ". Did you intend to bind this control to anything?");
				}
				xml_attribute bind_event_xml = button.attribute("bindEvent");
				bool bind_event = bind_event_xml.as_bool(); 
				if(!bind_event_xml || bind_event){
					m_activeCollection->AddEvent(&(tmp->ValueChanged));
				}
			}
			else{
				Log::Error("Failed to load ToggleButtonControl " + name + ". This may cause a fatal runtime error!");
			}
		}
	}
	else{
		Log::Error("Toggle Button Control Driver definitions not found! Skipping...");
	}	

	#pragma endregion ToggleButtonControl

	#pragma region ServoControl

	xml_node ServoControls = _operator.child("ServoControls");
	if(ServoControls){
		for(xml_node button = ServoControls.first_child(); button; button = button.next_sibling()){
			string name = button.name();
			if(Config::HasAttribute(button, "Type", "Failed to load Servo Control " + name + ". This may cause a fatal runtime error!"))
			{

				istringstream ss(Config::GetStringAttribute(button, "Buttons"));
    			string word;
				vector<double> Buttons;
				while (ss >> word) 
    			{
					Buttons.push_back(stod(word));	
    			}

				istringstream sA(Config::GetStringAttribute(button, "Angles"));
    			string Aword;
				vector<double> Angles;
    			while (sA >> Aword) 
    			{
					Angles.push_back(stod(Aword));
    			}

				istringstream sP(Config::GetStringAttribute(button, "POVs"));
    			string Pword;
				vector<double> POV;
    			while (sP >> Pword) 
    			{
					POV.push_back(stod(Pword));
    			}

				ServoControl *tmp = new ServoControl(m_operatorJoy, name, m_activeCollection, (ServoControl::ServoControlPart)Config::GetIntAttribute(button, "Type"), Config::GetBoolAttribute(button, "cycle"), Angles, Buttons, Config::GetDoubleAttribute(button, "Axis"), POV, Config::GetDoubleAttribute(button, "Mult"), Config::GetBoolAttribute(button, "Inc"), Config::GetDoubleAttribute(button, "Mode"));
				m_drive->AddControlOperate(tmp);
				tmp->SetOutprint(Config::GetBoolAttribute(button, "Print"));
				xml_attribute bindings = button.attribute("bindings");
				if(bindings){
					string bind_string = bindings.as_string();
					vector<string> bind_vector = getBindingStringList(bind_string);
					setBindingsToControl(bind_vector, tmp);
				}
				else{
					Log::Error("Control bindings not found for " + name + ". Did you intend to bind this control to anything?");
				}
				xml_attribute bind_event_xml = button.attribute("bindEvent");
				bool bind_event = bind_event_xml.as_bool(); 
				if(!bind_event_xml || bind_event){
					m_activeCollection->AddEvent(&(tmp->ValueChanged));
				}
			}
		}
	}
	else{
		Log::Error("Servo Control Operator definitions not found! Skipping...");
	}	

	#pragma endregion ServoControl

	#pragma region GoalButtonControl
	
	xml_node GoalButtonControls = _operator.child("GoalButtonControls");
	if(GoalButtonControls){
		for(xml_node button = GoalButtonControls.first_child(); button; button = button.next_sibling()){
			string name = button.name();
				xml_attribute goal = button.attribute("goal");
				xml_attribute repeat = button.attribute("repeat");
				xml_attribute ID = button.attribute("ID");
				xml_attribute IndexStart = button.attribute("StartIndex");
				xml_attribute OtherKeys = button.attribute("RemoveKeys");
				xml_attribute AxisNumbers = button.attribute("button");
				xml_attribute StringsData = button.attribute("StringData");
				xml_attribute State = button.attribute("State");
				xml_attribute Mult = button.attribute("mult");
				if(goal && repeat && ID && OtherKeys && AxisNumbers && StringsData && State && Mult){
					TeleOpGoal goalToAdd = getTeleOpGoal(goal.as_string());
					if(goalToAdd != TeleOpGoal::eNone){

						vector<int> RemKeys;
						vector<string> bind_vector_Key = getBindingStringList(OtherKeys.as_string());
						for(int i = 0; i < bind_vector_Key.size(); i++)
							RemKeys.push_back(stoi(bind_vector_Key.at(i)));

						vector<int> Axis;
						vector<string> bind_vector_Axis = getBindingStringList(AxisNumbers.as_string());
						for(int i = 0; i < bind_vector_Axis.size(); i++)
							Axis.push_back(stoi(bind_vector_Axis.at(i)));

						vector<string> StringDataVector = getBindingStringList(StringsData.as_string());

						GoalButtonControl* tmp = new GoalButtonControl(m_operatorJoy, name, Axis, m_activeCollection, goalToAdd, StringDataVector, IndexStart.as_int(), ID.as_int(), RemKeys, repeat.as_bool(), (GoalButtonControl::ControlButtonState)GetActivationState(State.as_string()), Mult.as_double());
						m_drive->AddControlOperate(tmp);
						tmp->SetOutprint(Config::GetBoolAttribute(button, "Print"));
						Log::General("Added GoalButtonControl " + name + ", Goal: " + goal.as_string());
						xml_attribute bind_event_xml = button.attribute("bindEvent");
						bool bind_event = bind_event_xml.as_bool(); 
						if(!bind_event_xml || bind_event){
							m_activeCollection->AddEvent(&(tmp->ValueChanged));
						}
					}
					else {
						Log::Error("Failed to load GoalButtonControl " + name + ". This may cause a fatal runtime eror!");
					}
				}
				else{
					Log::Error("Failed to load GoalButtonControl " + name + ". This may cause a fatal runtime eror!");
				}
		}
	}
	else{
		Log::Error("GoalButtonControl Operator definitions not found! Skipping...");
	}

	#pragma endregion

	#pragma region GoalAxisControl
	
	xml_node GoalAxisControls = _operator.child("GoalAxisControls");
	if(GoalAxisControls){
		for(xml_node button = GoalAxisControls.first_child(); button; button = button.next_sibling()){
			string name = button.name();
				xml_attribute goal = button.attribute("goal");
				xml_attribute repeat = button.attribute("repeat");
				xml_attribute ID = button.attribute("ID");
				xml_attribute IndexStart = button.attribute("StartIndex");
				xml_attribute OtherKeys = button.attribute("RemoveKeys");
				xml_attribute AxisNumbers = button.attribute("Axis");
				xml_attribute StringsData = button.attribute("StringData");
				xml_attribute DeadZ = button.attribute("DeadZone");
				xml_attribute Mult = button.attribute("mult");
				if(goal && repeat && ID && OtherKeys && AxisNumbers && StringsData && DeadZ && Mult){
					TeleOpGoal goalToAdd = getTeleOpGoal(goal.as_string());
					if(goalToAdd != TeleOpGoal::eNone){

						vector<int> RemKeys;
						vector<string> bind_vector_Key = getBindingStringList(OtherKeys.as_string());
						for(int i = 0; i < bind_vector_Key.size(); i++)
							RemKeys.push_back(stoi(bind_vector_Key.at(i)));

						vector<int> Axis;
						vector<string> bind_vector_Axis = getBindingStringList(AxisNumbers.as_string());
						for(int i = 0; i < bind_vector_Axis.size(); i++)
							Axis.push_back(stoi(bind_vector_Axis.at(i)));

						vector<string> StringDataVector = getBindingStringList(StringsData.as_string());

						GoalAxisControl* tmp = new GoalAxisControl(m_operatorJoy, name, Axis, m_activeCollection, goalToAdd, StringDataVector, IndexStart.as_int(), ID.as_int(), RemKeys, repeat.as_bool(), DeadZ.as_double(), Mult.as_double());
						m_drive->AddControlOperate(tmp);
						tmp->SetOutprint(Config::GetBoolAttribute(button, "Print"));
						Log::General("Added GoalAxisControl " + name + ", Goal: " + goal.as_string());
						xml_attribute bind_event_xml = button.attribute("bindEvent");
						bool bind_event = bind_event_xml.as_bool(); 
						if(!bind_event_xml || bind_event){
							m_activeCollection->AddEvent(&(tmp->ValueChanged));
						}
					}
					else {
						Log::Error("Failed to load GoalAxisControl " + name + ". This may cause a fatal runtime eror! - Missing goal");
					}
				}
				else{
					Log::Error("Failed to load GoalAxisControl " + name + ". This may cause a fatal runtime eror! - Missing params: goal:" + (goal ? "Here " : "Missing ") + "repeat:" + (repeat ? "Here " : "Missing ") + "ID:" + (ID ? "Here " : "Missing ") + "RemoveKeys:" + (OtherKeys ? "Here " : "Missing ") + "AxisNumbers:" + (AxisNumbers ? "Here " : "Missing ") + "StringsData:" + (StringsData ? "Here " : "Missing ") + "DeadZone:" + (DeadZ ? "Here " : "Missing ") + "Mult:" + (Mult ? "Here " : "Missing "));
				}
		}
	}
	else{
		Log::Error("GoalAxisControl Operator definitions not found! Skipping...");
	}

	#pragma endregion

#pragma region POVGoalControl

	xml_node GoalPOVControls = _operator.child("POVGoalControls");
	if(GoalPOVControls){
		for(xml_node button = GoalPOVControls.first_child(); button; button = button.next_sibling()){
			string name = button.name();
				xml_attribute goal = button.attribute("goal");
				xml_attribute repeat = button.attribute("repeat");
				xml_attribute ID = button.attribute("ID");
				xml_attribute IndexStart = button.attribute("StartIndex");
				xml_attribute OtherKeys = button.attribute("RemoveKeys");
				xml_attribute Dir = button.attribute("Direction");
				xml_attribute StringsData = button.attribute("StringData");
				xml_attribute Mode = button.attribute("Mode");
				xml_attribute State = button.attribute("State");
				xml_attribute Mult = button.attribute("mult");
				if(goal && repeat && ID && OtherKeys && Dir && StringsData && Mode && Mult){
					TeleOpGoal goalToAdd = getTeleOpGoal(goal.as_string());
					if(goalToAdd != TeleOpGoal::eNone){

						vector<int> RemKeys;
						vector<string> bind_vector_Key = getBindingStringList(OtherKeys.as_string());
						for(int i = 0; i < bind_vector_Key.size(); i++)
							RemKeys.push_back(stoi(bind_vector_Key.at(i)));

						vector<string> StringDataVector = getBindingStringList(StringsData.as_string());

						POVGoalControl *tmp = new POVGoalControl(m_operatorJoy, name, Dir.as_double(), Mult.as_double(), m_activeCollection, goalToAdd, StringDataVector, IndexStart.as_int(), ID.as_int(), RemKeys, repeat.as_bool(),  (Controls::POVGoalControl::POVGoalControlMode)Mode.as_int(), (Controls::POVGoalControl::POVGoalControlState) State.as_int());
						m_drive->AddControlOperate(tmp);
						tmp->SetOutprint(Config::GetBoolAttribute(button, "Print"));
						Log::General("Added POVGoalControls " + name + ", Goal: " + goal.as_string());
						xml_attribute bind_event_xml = button.attribute("bindEvent");
						bool bind_event = bind_event_xml.as_bool(); 
						if(!bind_event_xml || bind_event){
							m_activeCollection->AddEvent(&(tmp->ValueChanged));
						}
					}
					else {
						Log::Error("Failed to load POVGoalControls " + name + ". This may cause a fatal runtime eror!");
					}
				}
				else{
					Log::Error("Failed to load POVGoalControls " + name + ". This may cause a fatal runtime eror!");
				}
		}
	}
	else{
		Log::Error("POVGoalControls Operator definitions not found! Skipping...");
	}

	#pragma endregion POVGoalControl

	#pragma region Combo

	xml_node ComboControls = _operator.child("ComboControls");
	if(ComboControls)
	{
		for(xml_node Ccontrol = ComboControls.first_child(); Ccontrol; Ccontrol = Ccontrol.next_sibling())
		{
			string name = Ccontrol.name();
			if(Config::HasAttribute(Ccontrol, "isGoal", "Cannot tell if control " + name + " is a goal or not...skipping"))
			{
				bool AllHere = true;
				double Output = 0;
				bool useOut = false;

				#pragma region ControlInput
				istringstream ss(Config::GetStringAttribute(Ccontrol, "ControlInput"));
    			string word;
				vector<ControlItem*> Icontrols;
				vector<bool> Negate;
				vector<bool> ABS;
				vector<double> LorG;
				vector<bool> AO;
    			while (ss >> word) 
    			{
					Negate.push_back(false);
					ABS.push_back(false);
					AO.push_back(false);
					LorG.push_back(0);
					for (int i = 0; i < word.length(); i++)
					{
						if(word[i] == '-')
						{
							Negate.back() = !Negate.back();
						}
						else if(word[i] == '&')
						{
							AO.back() = true;
						}
						else if(word[i] == '_')
						{
							AO.back() = false;
						}
						else if(word[i] == '|')
						{
							ABS.back() = !ABS.back();
						}
						else if(word[i] == '>')
						{
							LorG.back() = 1;
						}
						else if(word[i] == '<')
						{
							LorG.back() = -1;
						}
						else if(word[i] == '^')
						{
							LorG.back() = 0;
						}
						else
						{
							word = word.substr(i);
							break;
						}
					}
					
					if(m_activeCollection->GetOperatorControl(word) != nullptr)
					{
						Icontrols.push_back(m_activeCollection->GetOperatorControl(word));
					}
					else
					{
						AllHere = false;
					}
    			}
				#pragma endregion ControlInput

				#pragma region RobotInput
				istringstream Rs(Config::GetStringAttribute(Ccontrol, "RobotInput"));
    			string Rword;
				vector<bool> RAO;
				vector<NativeComponent*> Rcontrols;
				while (Rs >> Rword)
    			{
					RAO.push_back(false);

					for (int i = 0; i < Rword.length(); i++)
					{
						
						if(word[i] == '*')
						{
							RAO.back() = !RAO.back();
						}
						else if(word[i] == '_')
						{
							RAO.back() = !RAO.back();
						}
						
						else
						{
							Rword = Rword.substr(i);
							break;
						}
					}

					if(m_activeCollection->Get(Rword) != nullptr)
					{
						Rcontrols.push_back(m_activeCollection->Get(Rword));
					}
					else
					{
						AllHere = false;
					}
    			}
				#pragma endregion RobotInput

				#pragma region Input
				istringstream Is(Config::GetStringAttribute(Ccontrol, "UseInput"));
    			string Iword;
				vector<ControlItem*> Incontrols;
				vector<bool> INegate;
				vector<bool> IABS;
				vector<double> ILorG;
    			while (Is >> Iword) 
    			{
					INegate.push_back(false);
					IABS.push_back(false);
					ILorG.push_back(0);
					if(!isDouble(Iword))
					{
					for (int i = 0; i < Iword.length(); i++)
					{
						if(Iword[i] == '-')
						{
							INegate.back() = !INegate.back();
						}
						else if(Iword[i] == '|')
						{
							IABS.back() = !IABS.back();
						}
						else if(Iword[i] == '>')
						{
							ILorG.back() = 1;
						}
						else if(Iword[i] == '<')
						{
							ILorG.back() = -1;
						}
						else if(Iword[i] == '^')
						{
							ILorG.back() = 0;
						}
						else
						{
							Iword = Iword.substr(i);
							break;
						}
					}
					}
					else
					{
						Output = stod(Iword);
						useOut = true;
						continue;
					}

					if(m_activeCollection->GetOperatorControl(Iword) != nullptr)
					{
						Incontrols.push_back(m_activeCollection->GetOperatorControl(Iword));
					}
					else
					{
						AllHere = false;
					}
    			}
				#pragma endregion Input

				#pragma region ValInput
				istringstream IVs(Config::GetStringAttribute(Ccontrol, "InputValues"));
    			string IVword;
				vector<double> IValues;
				vector<bool> NotVal;
				vector<int> GreaterVal;
    			while (IVs >> IVword) 
    			{
					NotVal.push_back(false);
					GreaterVal.push_back(0);
					for (int i = 0; i < IVword.length(); i++)
					{
						if(IVword[i] == '!')
						{
							NotVal.back() = !NotVal.back();
						}
						else if(IVword[i] == '>')
						{
							GreaterVal.back() = 1;
						}
						else if(IVword[i] == '<')
						{
							GreaterVal.back() = -1;
						}
						else if(IVword[i] == '=')
						{
							GreaterVal.back() = 0;
						}
						else
						{
							IVword = IVword.substr(i);
							break;
						}
					}
					IValues.push_back(stod(IVword));
    			}
				#pragma endregion ValInput

				#pragma region RobotInput
				istringstream IRs(Config::GetStringAttribute(Ccontrol, "RobotValues"));
    			string IRword;
				vector<double> RValues;
				vector<bool> NotRVal;
				vector<int> GreaterRVal;
    			while (IRs >> IRword) 
    			{
					NotRVal.push_back(false);
					GreaterRVal.push_back(0);
					for (int i = 0; i < IRword.length(); i++)
					{
						if(IRword[i] == '!')
						{
							NotRVal.back() = !NotRVal.back();
						}
						else if(IRword[i] == '>')
						{
							GreaterRVal.back() = 1;
						}
						else if(IRword[i] == '<')
						{
							GreaterRVal.back() = -1;
						}
						else if(IRword[i] == '=')
						{
							GreaterRVal.back() = 0;
						}
						else
						{
							IRword = IRword.substr(i);
							break;
						}
					}
					RValues.push_back(stod(IRword));
    			}
				#pragma endregion RobotInput

				if(AllHere)
				{
					bool Fireone = Config::GetBoolAttribute(Ccontrol, "FireOnce");
					int Mode = Config::GetIntAttribute(Ccontrol, "Mode");
					ComboControl* tmp;
					if(Config::GetBoolAttribute(Ccontrol, "isGoal"))
					{
						vector<int> RemKeys;
						vector<string> bind_vector_Key = getBindingStringList(Config::GetStringAttribute(Ccontrol, "RemoveKeys"));
						for(int i = 0; i < bind_vector_Key.size(); i++)
							RemKeys.push_back(stoi(bind_vector_Key.at(i)));
						
						vector<string> StringDataVector = getBindingStringList(Config::GetStringAttribute(Ccontrol, "StringData"));

						tmp = new ComboControl(m_operatorJoy, name, m_activeCollection, Config::GetBoolAttribute(Ccontrol, "primary"), Fireone, true, getTeleOpGoal(Config::GetStringAttribute(Ccontrol, "Goal", "N/A")), StringDataVector, Config::GetIntAttribute(Ccontrol, "StartIndex"), Config::GetIntAttribute(Ccontrol, "ID"), RemKeys, Config::GetBoolAttribute(Ccontrol, "Repeat"), Mode, Icontrols, Rcontrols, Incontrols, IValues, RValues, LorG, ABS, Negate, AO, RAO, ILorG, IABS, INegate, NotRVal, NotVal, GreaterVal, GreaterRVal, useOut, Output);
					}
					else
					{
						tmp = new ComboControl(m_operatorJoy, name, m_activeCollection, Config::GetBoolAttribute(Ccontrol, "primary"), Fireone, false, TeleOpGoal::eNone, vector<string> {}, 0, 0, vector<int> {}, false, Mode, Icontrols, Rcontrols, Incontrols, IValues, RValues, LorG, ABS, Negate, AO, RAO, ILorG, IABS, INegate, NotRVal, NotVal, GreaterVal, GreaterRVal, useOut, Output);
					}
					m_drive->AddControlOperate(tmp);
					tmp->SetOutprint(Config::GetBoolAttribute(Ccontrol, "Print"));
					xml_attribute bindings = Ccontrol.attribute("bindings");
					if(bindings){
						string bind_string = bindings.as_string();
						vector<string> bind_vector = getBindingStringList(bind_string);
						setBindingsToControl(bind_vector, tmp);
					}
					else{
						Log::Error("Control bindings not found for " + name + ". Did you intend to bind this control to anything?");
					}
					xml_attribute bind_event_xml = Ccontrol.attribute("bindEvent");
					bool bind_event = bind_event_xml.as_bool(); 
					if(!bind_event_xml || bind_event){
						m_activeCollection->AddEvent(&(tmp->ValueChanged));
					}
				}
				else
				{
					Log::Error("Combo control failed to find some components");
				}
			}
		}
	}

	#pragma endregion Combo

}

vector<string> Config::getBindingStringList(string bindings){
	vector<char*> tmp;
	vector<string> ret;
	//char * pch;   //unreferenced
	char * bindings_char = new char[bindings.length() + 1];
	strcpy(bindings_char, bindings.c_str());
	tmp.push_back(strtok(bindings_char, " ,"));
	while(tmp.back() != NULL){
		tmp.push_back(strtok(NULL, " ,"));				
	}
	tmp.pop_back();
	for(int i = 0; i<(int)tmp.size(); i++){
		string add(tmp[i]);
		ret.push_back(add);
	}
	return ret;
}

bool Config::setBindingsToControl(vector<string> bindings, ControlItem *control){
	bool success = true;
	for(int i = 0; i<(int)bindings.size(); i++){
		string binding = bindings[i];
		OutputComponent *component;
		try{
			component = (OutputComponent*)(m_activeCollection->Get(binding));
			control->AddComponent(component);
			Log::General("Allocated Component " + binding + " to Control " + control->name);
		}
		catch(...){
			Log::Error("Failed to bind component " + binding + " to the control " + control->name + ". This can cause fatal runtime errors!");
			success = false;
		}
	}
	if(!success)
		Log::Error("One or more bindings failed to allocate for control " + control->name + ". Please check the Config!");
	return success;
}

TeleOpGoal Config::getTeleOpGoal(string goalString){
	if(goalString.compare("ElevatorControl") == 0){
		return TeleOpGoal::eElevatorControl;
	}
	else if (goalString.compare("Timer") == 0) {
		return TeleOpGoal::eTimer;
	}
	else if(goalString.compare("DriveWithTimer") == 0)
	{
		return TeleOpGoal::eDriveWithTimer;
	}
	else if(goalString.compare("RelativeElevatorControl") == 0)
	{
		return TeleOpGoal::eRelativeElevatorControl;
	}
	else if(goalString.compare("ShooterGoal") == 0)
	{
		return TeleOpGoal::eShooter;
	}
	else if(goalString.compare("MotorPosition") == 0)
	{
		return TeleOpGoal::eMotorPosition;
	}
	else if(goalString.compare("ResetSwerve") == 0)
	{
		return TeleOpGoal::eResetSwerve;
	}
	else if(goalString.compare("Drive") == 0)
	{
		return TeleOpGoal::eMove;
	}
	else if(goalString.compare("Abort") == 0)
	{
		return TeleOpGoal::eAbort;
	}
	else{
		Log::Error("Error registering teleop goal " + goalString + ". Skipping this control...");
		return TeleOpGoal::eNone;
	}
}

int Config::GetLocation(string Loc)
{
	if(Loc.compare("Front_Left") == 0){
		return 0;
	}
	else if(Loc.compare("Front_Right") == 0)
	{
		return 1;
	}
	else if(Loc.compare("Back_Left") == 0)
	{
		return 2;
	}
	else if(Loc.compare("Back_Right") == 0)
	{
		return 3;
	}
	else{
		Log::Error("Defaulting location to SwerveModule::Location::Front_Left");
		return 0;
	}
}

int Config::GetActivationState(string Type)
{
	if(Type.compare("onPress") == 0){
		return 1;
	}
	else if(Type.compare("onRelease") == 0)
	{
		return 0;
	}
	else{
		Log::Error("Defaulting Activation state to both onPress and onRelease");
		return 3;
	}
}

double Config::GetDoubleAttribute(xml_node Node, string Name, double Default, string errorMessage)
{
	const pugi::char_t *name = Name.c_str();
	if(Node.attribute(name))
	{
		return Node.attribute(name).as_double();
	}
	else
	{
		Log::Error(errorMessage + "--Returning default for attribute " + Name + " in node " + Node.parent().name() + "/" + Node.name() + ": " + to_string(Default));
		return Default;
	}
}

int Config::GetIntAttribute(xml_node Node, string Name, int Default, string errorMessage)
{
	const pugi::char_t *name = Name.c_str();
	if(Node.attribute(name))
	{
		return Node.attribute(name).as_int();
	}
	else
	{
		Log::Error(errorMessage + "--Returning default for attribute " + Name + " in node " + Node.parent().name() + "/" + Node.name() + ": " + to_string(Default));
		return Default;
	}
}

bool Config::GetBoolAttribute(xml_node Node, string Name, bool Default, string errorMessage)
{
	const pugi::char_t *name = Name.c_str();
	if(Node.attribute(name))
	{
		return Node.attribute(name).as_bool();
	}
	else
	{
		Log::Error(errorMessage + "--Returning default for attribute " + Name + " in node " + Node.parent().name() + "/" + Node.name() + ": " + to_string(Default));
		return Default;
	}
}

string Config::GetStringAttribute(xml_node Node, string Name, string Default, string errorMessage)
{
	const pugi::char_t *name = Name.c_str();
	if(Node.attribute(name))
	{
		return Node.attribute(name).as_string();
	}
	else
	{
		Log::Error(errorMessage + "--Returning default for attribute " + Name + " in node " + Node.parent().name() + "/" + Node.name() + ": " + (Default));
		return Default;
	}
}

bool Config::HasAttribute(xml_node Node, string Name, string errorMessage)
{
	const pugi::char_t *name = Name.c_str();
	if(Node.attribute(name))
	{
		return true;
	}
	else
	{
		Log::Error(errorMessage);
		return false;
	}
}

void Config::SetMotorGroup(Motor* Subj, xml_node Node)
{
	int pdbChannel = Node.attribute("pdbChannel") ? Node.attribute("pdbChannel").as_int() : -1;
	if(pdbChannel != -1)
	{
		Log::General("Allocated PDBChannel " + to_string(pdbChannel) + " for Motor " + Subj->name);
		Subj->SetPDBChannel(pdbChannel);
	}

	int MotorGroup = Node.attribute("Group") ? Node.attribute("Group").as_int() : -1;
	if (MotorGroup != -1)
	{
		m_activeCollection->GetPDBManager()->SetMotorGroup(Subj, MotorGroup);
	}

	double PersonalLowerRate = Node.attribute("LowerRate") ? Node.attribute("LowerRate").as_double() : -1;
	if (PersonalLowerRate != -1)
	{
		Subj->SetLowerRate(PersonalLowerRate);
	}

	double PersonalRegenRate = Node.attribute("RegenRate") ? Node.attribute("RegenRate").as_double() : -1;
	if (PersonalRegenRate != -1)
	{
		Subj->SetRegenRate(PersonalRegenRate);
	}

	bool PrintOutB = Node.attribute("Print") ? Node.attribute("Print").as_bool() : false;
	Subj->SetOutprint(PrintOutB);
}

bool Config::GetBoolAttDic(xml_node Node, string Name, map<string, bool> Map, bool Default)
{
	string nameVal = Config::GetStringAttribute(Node, Name, "N/A");
	if(Map.find(nameVal) != Map.end())
    {
        return Map.find(nameVal)->second;
    }
    else
    {
        Log::Error("Cannot find " + nameVal + " in Map that comes from value of " + Name + " RETURNING DEFAULT!");
        return Default;
    }
}

int Config::GetIntAttDic(xml_node Node, string Name, map<string, int> Map, int Default)
{
	string nameVal = Config::GetStringAttribute(Node, Name, "N/A");
	if(Map.find(nameVal) != Map.end())
    {
        return Map.find(nameVal)->second;
    }
    else
    {
        Log::Error("Cannot find " + nameVal + " in Map that comes from value of " + Name + " RETURNING DEFAULT!");
        return Default;
    }
}

double Config::GetDoubleAttDic(xml_node Node, string Name, map<string, double> Map, double Default)
{
	string nameVal = Config::GetStringAttribute(Node, Name, "N/A");
	if(Map.find(nameVal) != Map.end())
    {
        return Map.find(nameVal)->second;
    }
    else
    {
        Log::Error("Cannot find " + nameVal + " in Map that comes from value of " + Name + " RETURNING DEFAULT!");
        return Default;
    }
}

string Config::GetStringAttDic(xml_node Node, string Name, map<string, string> Map, string Default)
{
	string nameVal = Config::GetStringAttribute(Node, Name, "N/A");
	if(Map.find(nameVal) != Map.end())
    {
        return Map.find(nameVal)->second;
    }
    else
    {
        Log::Error("Cannot find " + nameVal + " in Map that comes from value of " + Name + " RETURNING DEFAULT!");
        return Default;
    }
}

Config::~Config(){}