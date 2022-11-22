/****************************** Header ******************************\
Class Name: CompositeGoal, MultitaskGoal
File Name: Goal.cpp
Summary: Standard goal files. These classes are usually extended from,
but it rare cases MultitaskGoal and CompositeGoal may not
Project: BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): James Killian, Chris Weeks
Email: chrisrweeks@aol.com
\********************************************************************/
#include <stddef.h>
#include <stdlib.h>
#include <list>
#include "Goal.h"

using namespace std;

/***************************************************************************************************************/
/*												Goal														   */
/***************************************************************************************************************/
//Set data at an index and if the idex doesnt exist add empty values till it does then set data at index
void Goal::Setdata(int index, double dataVal)
{
	if(index >= 0 && index < data.size())
	{
		data.at(index) = dataVal;
	}
	else
	{
		for(int i = data.size() - 1; i < index + 1; i++)
		{
			data.push_back(0);
		}
		data.at(index) = dataVal;
	}
}

//copy data to a goal from the start index, used for goal controls
void Goal::CopyFrom(vector<double> dataIn, int startIndex)
{
	for(int i = startIndex; i < dataIn.size() + startIndex; i++)
	{
		Setdata(i, dataIn.at(i - startIndex));
	}
}

//Set data at an index and if the idex doesnt exist add empty values till it does then set data at index
void Goal::SetStringdata(int index, string dataVal)
{
	if(index >= 0 && index < Stringdata.size())
	{
		Stringdata.at(index) = dataVal;
	}
	else
	{
		for(int i = Stringdata.size() - 1; i < index + 1; i++)
		{
			Stringdata.push_back("N/A");
		}
		Stringdata.at(index) = dataVal;
	}
}

//copy data to a goal from the start index, used for goal controls
void Goal::CopyStringFrom(vector<string> dataIn, int startIndex)
{
	for(int i = startIndex; i < dataIn.size() + startIndex; i++)
	{
		SetStringdata(i, dataIn.at(i - startIndex));
	}
}


/***************************************************************************************************************/
/*												CompositeGoal												   */
/***************************************************************************************************************/

void CompositeGoal::Activate()
{
	m_Status = eActive;
}
Goal::Goal_Status CompositeGoal::Process(double dTime)
{
	if (m_Status == eActive)
	{
		Goal_Status newStatus = eFailed;
		//remove finished goals
		while (!m_SubGoals.empty() && (m_SubGoals.front()->GetStatus() == eCompleted || m_SubGoals.front()->GetStatus() == eFailed))
		{
			data = m_SubGoals.front()->data;
			m_SubGoals.erase(m_SubGoals.begin());
		}
		if (!m_SubGoals.empty())
		{
			//create pointer toks next goal for simplicity
			Goal* currentGoal = m_SubGoals.front();
			//active the next goal if it isnt already
			if (currentGoal->GetStatus() == eInactive)
			{
				currentGoal->data = data;
				currentGoal->Activate();
			}
			newStatus = currentGoal->Process(dTime);

			if(newStatus == eCompleted && m_SubGoals.size() > 1)
			{
				newStatus = eActive;
			}
		}
		else
		{
			Terminate();
		}
		return newStatus;
	}
	else
	{
		return m_Status;
	}
}

//Each goal can have an ID, in the config goal controls have an attribute for that and the method grabs a goal with the same ID number
Goal* CompositeGoal::GetGoal(int IdentityKey)
{
	Goal* Selected = nullptr;
	for(int i = 0; i < m_SubGoals.size(); i++)
	{
		if(m_SubGoals.at(i)->IdentityKey == IdentityKey)
		{
			Selected = m_SubGoals.at(i);
			break;
		}
	}
	return Selected;
}

//Each goal can have an ID, in the config goal controls have an attribute for that and the method checks for a goal with the same ID number
bool CompositeGoal::HasGoal(int IdentityKey)
{
	bool Selected = false;
	for(int i = 0; i < m_SubGoals.size(); i++)
	{
		if(m_SubGoals.at(i)->IdentityKey == IdentityKey)
		{
			Selected = true;
			break;
		}
	}
	return Selected;
}

//Each goal can have an ID, in the config goal controls have an attribute for that and the method removes a goal with the same ID number
bool CompositeGoal::RemoveGoal(int IdentityKey)
{
	bool removed = false;
	for(int i = 0; i < m_SubGoals.size(); i++)
	{
		if(m_SubGoals.at(i)->IdentityKey == IdentityKey)
		{
			m_SubGoals.at(i)->Terminate();
			delete m_SubGoals.at(i);
			m_SubGoals.erase(m_SubGoals.begin() + i);
			removed = true;
		}
	}
	return removed;
}

//Each goal can have an ID, in the config goal controls have an attribute for that and the method replaces a goal with the same ID number with a new goal
bool CompositeGoal::ReplaceGoal(int IdentityKey, Goal* NewGoal)
{
	bool removed = false;
	for(int i = 0; i < m_SubGoals.size(); i++)
	{
		if(m_SubGoals.at(i)->IdentityKey == IdentityKey)
		{
			m_SubGoals.at(i)->Terminate();
			delete m_SubGoals.at(i);
			m_SubGoals.erase(m_SubGoals.begin() + i);
			m_SubGoals.insert(m_SubGoals.begin() + i, NewGoal);
			removed = true;
		}
	}
	return removed;
}

//Removes the next goal in the composite goal and can be called from a goal inside this composite goal
bool CompositeGoal::RemoveNextGoal()
{
	bool removed = false;
	if(m_SubGoals.size() > 1)
	{
		m_SubGoals.at(1)->Terminate();
		delete m_SubGoals.at(1);
		m_SubGoals.erase(m_SubGoals.begin() + 1);
		removed = true;
	}
	return removed;
}

//Adds a new goal as the next goal in the composite goal and can be called from a goal inside this composite goal
bool CompositeGoal::AddNextGoal(Goal* NewGoal)
{
	bool Added = false;
	if(m_SubGoals.size() > 1)
		m_SubGoals.insert(m_SubGoals.begin() + 1, NewGoal);
	else
		m_SubGoals.push_back(NewGoal);
	Added = true;
	return Added;
}

//Removes goals of type or not of type excluding current goal
template <typename T>
bool CompositeGoal::RemoveGoalOfType(Goal* Current, T Type, bool ExcludeType)
{
	int PrevId = Current->IdentityKey;
	Current->IdentityKey = -500;
	bool removed = false;
	for(int i = 0; i < m_SubGoals.size(); i++)
	{
		if(m_SubGoals.at(i)->IdentityKey >= 0)
		{
			if(ExcludeType ? !dynamic_cast<T*>(m_SubGoals.at(i)) : dynamic_cast<T*>(m_SubGoals.at(i)))
			{
				m_SubGoals.at(i)->Terminate();
				delete m_SubGoals.at(i);
				m_SubGoals.erase(m_SubGoals.begin() + i);
				removed = true;
			}
		}
	}
	Current->IdentityKey = PrevId;
	return removed;
}

void CompositeGoal::Terminate()
{
	m_Status = eInactive;
}
/***************************************************************************************************************/
/*												MultitaskGoal												   */
/***************************************************************************************************************/

MultitaskGoal::MultitaskGoal(ActiveCollection *activeCollection, bool WaitAll) : m_WaitAll(WaitAll)
{
	m_Status = eInactive;
}

void MultitaskGoal::RemoveAllGoals()
{
	for (GoalList::iterator it = m_GoalsToProcess.begin(); it != m_GoalsToProcess.end(); ++it)
	{
		(*it)->Terminate();
		delete *it;
	}
	m_GoalsToProcess.clear();
}

MultitaskGoal::~MultitaskGoal()
{
	RemoveAllGoals();
}

void MultitaskGoal::Activate()
{

	for (GoalList::iterator it = m_GoalsToProcess.begin(); it != m_GoalsToProcess.end(); ++it)
		(*it)->Activate();
	m_Status = eActive;
}
Goal::Goal_Status MultitaskGoal::Process(double dTime_s)
{
	ActivateIfInactive();
	Goal_Status status = eFailed;
	size_t NonActiveCount = 0;

	bool SuccessDetected = false;
	//To keep things simple we'll always run a complete iteration of all the goals for the given process cycle, and simply or in the success
	//detected... This way any success that happened will be reflected and dealt with below
	for (GoalList::iterator it = m_GoalsToProcess.begin(); it != m_GoalsToProcess.end(); ++it)
	{
		status = (*it)->Process(dTime_s);
		if (status == eFailed)
		{
			m_Status = eFailed;
			return eFailed;
		}

		if (status != eActive)
		{
			NonActiveCount++;
			SuccessDetected |= (status == eCompleted);
		}
	}

	//Either we wait until no more active goals exist, or if the wait all option is false, we can complete when the first detected successful completion
	//has occurred.  So for that... if no successful completion then it would fall back to the wait all logic and then evaluate if it failed
	const bool IsCompleted = ((NonActiveCount >= m_GoalsToProcess.size()) || ((!m_WaitAll) && (SuccessDetected)));
	if (!IsCompleted)
		m_Status = eActive;
	else
	{
		status = eCompleted;
		//Check the final status it is completed unless any goal failed
		for (GoalList::iterator it = m_GoalsToProcess.begin(); it != m_GoalsToProcess.end(); ++it)
		{
			if ((*it)->GetStatus() == eFailed)
				status = eFailed;
		}
		m_Status = status;
	}
	return status;
}

//Each goal can have an ID, in the config goal controls have an attribute for that and the method grabs a goal with the same ID number
Goal* MultitaskGoal::GetGoal(int IdentityKey)
{
	Goal* Selected = nullptr;
	for(int i = 0; i < m_GoalsToProcess.size(); i++)
	{
		if(m_GoalsToProcess.at(i)->IdentityKey == IdentityKey)
		{
			Selected = m_GoalsToProcess.at(i);
			break;
		}
	}
	return Selected;
}

//Each goal can have an ID, in the config goal controls have an attribute for that and the method checks for a goal with the same ID number
bool MultitaskGoal::HasGoal(int IdentityKey)
{
	bool Selected = false;
	for(int i = 0; i < m_GoalsToProcess.size(); i++)
	{
		if(m_GoalsToProcess.at(i)->IdentityKey == IdentityKey)
		{
			Selected = true;
			break;
		}
	}
	return Selected;
}

//Each goal can have an ID, in the config goal controls have an attribute for that and the method removes a goal with the same ID number
bool MultitaskGoal::RemoveGoal(int IdentityKey)
{
	bool removed = false;
	for(int i = 0; i < m_GoalsToProcess.size(); i++)
	{
		if(m_GoalsToProcess.at(i)->IdentityKey == IdentityKey)
		{
			m_GoalsToProcess.at(i)->Terminate();
			delete m_GoalsToProcess.at(i);
			m_GoalsToProcess.erase(m_GoalsToProcess.begin() + i);
			removed = true;
		}
	}
	return removed;
}

//Each goal can have an ID, in the config goal controls have an attribute for that and the method replaces a goal with the same ID number with a new goal
bool MultitaskGoal::ReplaceGoal(int IdentityKey, Goal* NewGoal)
{
	bool removed = false;
	for(int i = 0; i < m_GoalsToProcess.size(); i++)
	{
		if(m_GoalsToProcess.at(i)->IdentityKey == IdentityKey)
		{
			m_GoalsToProcess.at(i)->Terminate();
			delete m_GoalsToProcess.at(i);
			m_GoalsToProcess.erase(m_GoalsToProcess.begin() + i);
			m_GoalsToProcess.insert(m_GoalsToProcess.begin() + i, NewGoal);
			removed = true;
		}
	}
	return removed;
}

//Cant remove next goal in multitask goal because there is no next goal in multitask goal
bool MultitaskGoal::RemoveNextGoal()
{
	bool removed = false;
	return removed;
}

//Adds a new goal to the multitask goal
bool MultitaskGoal::AddNextGoal(Goal* NewGoal)
{
	bool Added = false;
	m_GoalsToProcess.push_back(NewGoal);
	Added = true;
	return Added;
}

//Removes goals of type or not of type excluding current goal
template <typename T>
bool MultitaskGoal::RemoveGoalOfType(Goal* Current, T Type, bool ExcludeType)
{
	int PrevId = Current->IdentityKey;
	Current->IdentityKey = -500;
	bool removed = false;
	for(int i = 0; i < m_GoalsToProcess.size(); i++)
	{
		if(m_GoalsToProcess.at(i)->IdentityKey >= 0)
		{
			if(ExcludeType ? !dynamic_cast<T*>(m_GoalsToProcess.at(i)) : dynamic_cast<T*>(m_GoalsToProcess.at(i)))
			{
				m_GoalsToProcess.at(i)->Terminate();
				delete m_GoalsToProcess.at(i);
				m_GoalsToProcess.erase(m_GoalsToProcess.begin() + i);
				removed = true;
			}
		}
	}
	Current->IdentityKey = PrevId;
	return removed;
}

void MultitaskGoal::Terminate()
{
	//ensure its all clean
	RemoveAllGoals();
	m_Status = eInactive; //make this inactive
}

void MultitaskGoal::Reset(){
	Terminate();
}
