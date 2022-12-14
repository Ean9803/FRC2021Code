/****************************** Header ******************************\
Class Name: -
File Name: LinePaths.h
Summary: File of paths for auto use.
Project: FRC2020
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ian Poll (Mostly Zain lets be honest)
Email: irobot9803@gmail.com
\********************************************************************/

#pragma once

#ifndef SRC_UTIL_LINEPATHS_H_
#define SRC_UTIL_LINEPATHS_H_

#include <map>

using namespace std;
using namespace Util;
using namespace Logger;

#define NumPoints 3

struct Point
{
    double Act;
    double Angle;
    double Radius;
};

struct Auto
{
    Auto(int MaxPoints, vector<double> Points, bool Offset = false)
    {
        double Xoff = 0;
        double Yoff = 0;
        if (Points.size() % 3 == 0)
        {
            if (Offset)
            {
                Xoff = Points[0];
                Yoff = Points[1];
            }
            for(int i = 0; i < MaxPoints * NumPoints; i+=NumPoints)
            {
                Point* CheckPoint = new Point();
                CheckPoint->Angle = Points[i] - Xoff;
                CheckPoint->Radius = Points[i + 1] - Yoff;
                CheckPoint->Act = Points[i + 2];
                Waypoints.push_back(CheckPoint);
            }
            Num = MaxPoints;
        }
        else
        {
            Point* CheckPoint = new Point();
            Waypoints.push_back(CheckPoint);
        }
    }
    int Num = 0;
    vector <Point*> Waypoints;
};

static vector<double> backupPath(string Path)
{
    vector<double> InputBackupPoints;
    std::map<string, string> BackUpPathsDic = {
        {"Dud", "6 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "},
        {"BackFourth_XYAct", "63 -3.523352 -1.277473 0 -3.47916 -1.124078 0 -3.437703 -0.9620392 0 -3.398297 -0.7935166 0 -3.360259 -0.6206717 0 -3.322904 -0.4456657 0 -3.28555 -0.2706596 0 -3.247511 -0.09781468 0 -3.208106 0.07070804 0 -3.166649 0.2327471 0 -3.122457 0.3861413 0 -3.074846 0.5287298 0 -3.023132 0.6583513 0 -2.966632 0.7728446 0 -2.904662 0.8700483 0 -2.836539 0.9478021 0 -2.707391 1.040225 0 -2.56072 1.095509 0 -2.400539 1.117216 0 -2.230861 1.108907 0 -2.0557 1.074142 0 -1.87907 1.016484 0 -1.704983 0.9394926 0 -1.537453 0.8467305 0 -1.380494 0.7417583 0 -1.285091 0.6594409 0 -1.191951 0.5561387 0 -1.100743 0.4353958 0 -1.011137 0.300756 0 -0.9228024 0.1557643 0 -0.835409 0.003964663 0 -0.7486264 -0.1510988 0 -0.662124 -0.3058821 0 -0.5755717 -0.4568408 0 -0.4886388 -0.6004307 0 -0.4009948 -0.7331082 0 -0.3123097 -0.851328 0 -0.2222532 -0.9515467 0 -0.1304947 -1.03022 0 0.03505145 -1.138111 0 0.2057219 -1.225854 0 0.3793435 -1.289827 0 0.5537431 -1.326408 0 0.7267476 -1.331975 0 0.896184 -1.302906 0 1.059879 -1.23558 0 1.215659 -1.126374 0 1.290169 -1.052016 0 1.362734 -0.9614179 0 1.433555 -0.8562579 0 1.502833 -0.7382169 0 1.57077 -0.608975 0 1.637567 -0.4702122 0 1.703424 -0.3236087 0 1.768544 -0.1708447 0 1.833127 -0.01360035 0 1.897375 0.1464442 0 1.961489 0.307609 0 2.02567 0.4682136 0 2.090119 0.6265781 0 2.155037 0.7810222 0 2.220626 0.9298658 0 2.287088 1.071429 0 "},
        {"Race", "52 -1.959081 -0.1160527 0 -1.775442 -0.06153639 0 -1.615179 -0.01364158 0 -1.462889 0.01737721 0 -1.296204 0.04378125 0 -1.051493 0.06372704 0 -0.8607883 0.05498168 0 -0.737367 0.05498168 0 -0.6208407 0.04737665 0 -0.5052401 -0.01592243 0 -0.3680451 -0.09885165 0 -0.2218065 -0.2488514 0 -0.1955564 -0.3448712 0 -0.1674958 -0.5247334 0 -0.1764209 -0.6755556 0 -0.344631 -0.7778233 0 -0.5604298 -0.8022193 0 -0.7318845 -0.6652123 0 -0.7301361 -0.5046088 0 -0.7077992 -0.4172174 0 -0.6886888 -0.3332926 0 -0.5321094 -0.2117134 0 -0.351466 -0.1043779 0 -0.2459213 -0.0414474 0 0.1289591 -0.04174381 0 0.3705385 -0.03937536 0 0.8968157 0.1192135 0 1.13764 0.2353161 0 1.380848 0.4711389 0 1.234745 0.7113696 0 0.9434287 0.7445276 0 0.7160598 0.6805801 0 0.5715858 0.4484744 0 0.661164 0.1403627 0 0.836032 -0.1704695 0 1.210996 -0.5537525 0 1.420978 -0.6530255 0 1.517331 -0.7102696 0 1.811823 -0.7459969 0 2.042175 -0.636192 0 2.104031 -0.5518396 0 2.153353 -0.4865256 0 2.15892 -0.2713635 0 2.063629 -0.1416465 0 1.72341 -0.02322525 0 1.280514 -0.03980422 0 0.6493356 0.02634515 0 0.211854 0.124046 0 -0.4270182 0.2718854 0 -1.023862 0.2718854 0 -1.410705 0.2534643 0 -2.041975 0.2542107 0 "},
        {"Bounce", "29 -2.015079 -0.0306989 0 -1.641658 0.05324852 0 -1.473763 0.2558803 0 -1.412974 0.5424594 0 -1.33771 0.9419335 0 -1.268236 0.3166697 0 -1.082973 -0.2043832 0 -0.7877097 -0.708068 0 -0.5329726 -0.9164892 0 -0.1450773 -0.8412259 0 -0.03507729 -0.1349096 0 -0.05823521 0.3861436 0 -0.02928774 0.9477229 0 0.06913321 0.3687751 0 0.1038702 0.0214064 0 0.1791335 -0.4938573 0 0.3122914 -0.7891207 0 0.6249232 -0.8412259 0 0.9143973 -0.8064891 0 1.088082 -0.7312258 0 1.22124 -0.4880677 0 1.283476 -0.1739885 0 1.305187 0.1690381 0 1.314234 0.5609136 0 1.348608 0.9303547 0 1.429299 0.5703214 0 1.597556 0.2703539 0 1.910187 0.01561686 0 2.188082 -0.05964631 0 "},
        {"Slalom", "60 -1.960528 -0.9789484 0 -1.75066 -0.9687995 0 -1.522369 -0.8624035 0 -1.395132 -0.6787145 0 -1.295563 -0.4839301 0 -1.119799 -0.3271994 0 -1.106519 -0.2728011 0 -1.018422 -0.1526317 0 -0.9031249 -0.03781135 0 -0.8236849 0.06315786 0 -0.6733769 0.1410154 0 -0.5389733 0.2022113 0 -0.4031804 0.24223 0 -0.2657898 0.257895 0 0.0392755 0.2784408 0 0.2492774 0.2492506 0 0.4502494 0.1979648 0 0.6561046 0.127244 0 0.8562407 -0.003839299 0 1.060527 -0.2368424 0 1.209879 -0.5119002 0 1.347281 -0.6649784 0 1.456442 -0.7692217 0 1.679799 -0.8585752 0 1.952068 -0.9300495 0 2.065284 -0.9245177 0 2.407702 -0.7588701 0 2.451846 -0.5597389 0 2.441247 -0.4427067 0 2.37295 -0.212758 0 2.280739 -0.09332636 0 2.028949 0.08947381 0 1.818423 0.09633702 0 1.639475 0.08947381 0 1.540718 0.01744677 0 1.454144 -0.108332 0 1.371744 -0.2685745 0 1.337206 -0.3971299 0 1.252475 -0.5222733 0 1.134537 -0.6701788 0 0.923685 -0.8684219 0 0.8020955 -0.9200377 0 0.6663376 -0.9657693 0 0.5188738 -1.004575 0 0.3621686 -1.035412 0 0.1986844 -1.057238 0 0.03088444 -1.069012 0 -0.1387682 -1.06969 0 -0.307811 -1.058233 0 -0.4737793 -1.033596 0 -0.6342111 -0.9947377 0 -0.7650533 -0.9482951 0 -1.123685 -0.6736849 0 -1.255642 -0.5330424 0 -1.463017 -0.272929 0 -1.631044 -0.0440548 0 -1.693409 0.02557677 0 -1.881755 0.06369826 0 -2.011043 0.07862043 0 -2.246856 0.1003272 0 "}
    };
    Log::General("************************************Using Path: " + Path + "********************************");
    string Scanner = "";
    if(BackUpPathsDic.find(Path) != BackUpPathsDic.end())
    {
        Scanner = BackUpPathsDic.find(Path)->second;
    }
    else
    {
        Log::Error("Cannot find Backup Path " + Path + " using path labled \"Dud\"!");
        Scanner = BackUpPathsDic.find("Dud")->second;
    }

    istringstream ss(Scanner);
    string word;
    int index = 0;
    int Pointsnum = 0;
    while (ss >> word) 
    {
        if (index == 0)
        {
            Pointsnum = stod(word);
        }
        else
        {
            InputBackupPoints.push_back(stod(word));
        }
        index++;
    }
    
    return InputBackupPoints;
}

static Auto Map(string Path, bool Offset)
{
    ifstream Inputfile (Path);
    string NumberInput;
    if(Inputfile.is_open())
    {
        Log::General("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Found File%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
        vector<double> InputPoints;
        getline(Inputfile, NumberInput);
        double Length = (stod(NumberInput) - 1) / NumPoints;
        Log::General("Lenght: " + to_string(Length));
        for(int i = 0; i < (Length * NumPoints); i++)
        {
            getline(Inputfile, NumberInput);
            InputPoints.push_back(stod(NumberInput));
            Log::General("Line: " + to_string(i) + " = " + to_string(stod(NumberInput)));
        }
        return Auto(Length, InputPoints, Offset);
    }
    else
    {
        string PathF = "src/main/cpp/Autos/Paths/" + Path;
        ifstream SimFile(PathF);
        if (SimFile.is_open())
        {
            Log::General("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Found File%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
            vector<double> InputPoints;
            getline(SimFile, NumberInput);
            double Length = stod(NumberInput);
            for(int i = 0; i < (Length * NumPoints); i++)
            {
                getline(SimFile, NumberInput);
                InputPoints.push_back(stod(NumberInput));
                Log::General("Line: " + to_string(i) + " = " + to_string(stod(NumberInput)));
            }
            return Auto(Length, InputPoints, Offset);
        }
        else
        {
            ifstream DudFile("Dud.txt");
            if (DudFile.is_open())
            {
                Log::Error("File does not exist, using Dud.txt");
                vector<double> InputDudPoints;
                getline(DudFile, NumberInput);
                double Length = stod(NumberInput);
                for(int i = 0; i < (Length * NumPoints); i++)
                {
                    getline(DudFile, NumberInput);
                    InputDudPoints.push_back(stod(NumberInput));
                }
                return Auto(Length, InputDudPoints);
            }
        }
    }
    Log::Error("Files don't not exist, using backup Path");
    vector<double> InputBack = backupPath(Path.substr(0, Path.length() - 4));
    return Auto(InputBack.size() / NumPoints, InputBack, Offset);
}

#endif /* UTIL_LinePaths_H_ */