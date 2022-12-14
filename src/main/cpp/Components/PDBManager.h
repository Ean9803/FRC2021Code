/****************************** Header ******************************\
Class Name: PDBManager
File Name:	PDBManager.h
Summary: class to manage current on the PDB
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ian Poll
Email: irobot9803@gmail.com
\********************************************************************/

#ifndef SRC_PDB_Manager_H_
#define SRC_PDB_Manager_H_

#include <frc/WPILib.h>
#include <vector>
#include <iostream>


using namespace frc;
using namespace Util;

namespace Components
{
	class PDBManager
	{
        private:
            double Timeout = 0;
            double PDBCurentThres = 0;
            double Lower = 0;
            bool Run = true;
            //list of lists of motors with the motors in the same group being in the same list
            std::vector<std::vector<Motor*>> MotorGroups;
            PowerDistributionPanel PDB{0};

		public:
			
			PDBManager(){}
            //Constructor used in config
            PDBManager(double TimeOut, double CurrentThres, double LowerAmount, bool run){ Timeout = TimeOut; PDBCurentThres = CurrentThres; Lower = LowerAmount; Run = run; }

            //Add motor to a motor group in the first list of list of motors
            void SetMotorGroup (Motor *MotorPtr, int Group)
            {
                if (Group < MotorGroups.size() && Group >= 0)
                {
                    MotorGroups.at(Group).push_back(MotorPtr);
                }
                else if (Group >= MotorGroups.size() && Group >= 0)
                {
                    int DiffSize = Group - MotorGroups.size();
                    for (int i = 0; i < DiffSize; i++)
                    {
                        vector<Motor*> NewVector = {};
                        MotorGroups.push_back(NewVector);
                    }
                    MotorGroups.at(Group).push_back(MotorPtr);
                }
            }

            //Method to go through each group and take the average current and from there determine if it needs to be lowered or not
            void UpdatePDB()
            {
                if (PDB.GetTotalCurrent() > PDBCurentThres)
                {
                    vector<double> AverageInGroups;
                    for(int i = 0; i < MotorGroups.size(); i++)
                    {
                        double Average = 0;
                        for(int j = 0; j < MotorGroups.at(i).size(); j++)
                        {
                            Average += PDB.GetCurrent(MotorGroups.at(i).at(j)->GetPDBChannel());
                        }
                        Average /= MotorGroups.at(i).size();
                        AverageInGroups.push_back(Average);
                    }

                    int MaxGroup = -1;
                    for(int i = 1; i < AverageInGroups.size(); i++)
                    {
                        if (AverageInGroups.at(i - 1) < AverageInGroups.at(i))
                        {
                            MaxGroup = i;
                        }
                    }

                    for(int i = 0; i < MotorGroups.at(MaxGroup).size(); i++)
                    {
                        MotorGroups.at(MaxGroup).at(i)->SetTimeOut(Timeout, Lower);
                    }
                }
            }

            /*
            The delete component is just a way to clean up space (it was used for quickload so we dont get dups of objects)
            */
            virtual void DeleteComponent() {delete this;};

			virtual ~PDBManager(){}
	};
}

#endif /* SRC_PDB_Manager_H_ */