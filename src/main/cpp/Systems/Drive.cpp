/****************************** Header ******************************\
Class Name:	Drive
File Name:	Drive.cpp
Summary: 	Drive loop
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Ryan Cooper, Dylan Watson
Email:	cooper.ryan@centaurisoftware.co, dylantrwatson@gmail.com
\*********************************************************************/

#include "Drive.h"
#include <iostream>

using namespace std;
using namespace System;

Drive::Drive(ActiveCollection* activeCollection) { 
	m_activeCollection = activeCollection;
	m_teleOpGoal = new MultitaskGoal(m_activeCollection);
}

void Drive::AddControlDrive(ControlItem *control)
{
	m_activeCollection->AddDriverControl(control);
}

void Drive::AddControlOperate(ControlItem *control)
{
	m_activeCollection->AddOperatorControl(control);
}

//Start teleop by enabling all controls except for dead controls stated in config
void Drive::StartUp(vector<string> DeadControls)
{
	m_activeCollection->EnableAllControlsExcept(DeadControls);
}

//Start auto by disabling all controls except for the names in the list 
void Drive::StartAuto(vector<string> EnableControls)
{
	m_activeCollection->DisableAllControlsExcept(EnableControls);
}

//Update components, PDB manager, and controls
void Drive::Update(double deltaTime)
{
	if (m_activeCollection != nullptr)
	{
		m_activeCollection->ProcessSuperior_Goal(deltaTime);
		m_activeCollection->UpdateComponents(deltaTime);
		if (m_activeCollection->GetPDBManager() != nullptr)
			m_activeCollection->GetPDBManager()->UpdatePDB();
	}
	if (!m_DisableDrive)
	{
		m_activeCollection->UpdateDriver(deltaTime);
	}
	if (!m_DisableOperator)
	{
		m_activeCollection->UpdateOperator(deltaTime);
	}
}

//Clean up PDB object and check if any other components are active
void Drive::DeleteAll()
{
	if (m_activeCollection->GetPDBManager() != nullptr)
		m_activeCollection->GetPDBManager()->DeleteComponent();
	m_activeCollection->UpdateComponents(0);
}