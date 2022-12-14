/****************************** Header ******************************\
Class Name: TEventArgs inherits EventArgs
File Name: EventArgs.h
Summary: Argument template class to be used with events
Project: BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ryan Cooper, Dylan Watson
Email: cooper.ryan@centaurisoftware.co, dylantrwatson@gmail.com
\********************************************************************/

#pragma once 

class EventArgs
{
    public:
        double Type;
};

template<class T1, class T2>
class TEventArgs : public EventArgs
{
public:
    enum ControlPowerMode
    {
        Raw = 0,
        Ramp = 1,
        Encoder = 2,
        Position = 3
    };
    T1 GetValue() { return value; }
    T2 GetSender() { return sender; }
    ControlPowerMode GetMode() { return Mode; }
    TEventArgs(T1 value, T2 sender, ControlPowerMode Mode = ControlPowerMode::Ramp) { this->value = value; this->sender = sender; this->Type = 0; this->Mode = Mode; }
    
private:
    T1 value;
    T2 sender;
    ControlPowerMode Mode;
};

template<class T1, class T2, class T3, class T4>
class IEventArgs : public EventArgs
{
private:
    T1 vertical;
    T2 horizontal;
    T3 spin;
    T4 sender;

public:
    T1 GetVValue() { return vertical; }
    T2 GetHValue() { return horizontal; }
    T3 GetSValue() { return spin; }
    T4 GetSender() { return sender; }
    IEventArgs(T1 vertical, T2 horizontal, T3 spin, T4 sender) { this->Type = 1; this->vertical = vertical; this->horizontal = horizontal; this->spin = spin; this->sender = sender; }
};

template<class T1, class T2, class T3>
class LEventArgs : public EventArgs
{
private:
    T1 Vals;
    T2 Strings;
    T3 sender;

public:
    T1 GetValues() { return Vals; }
    T2 GetStrings() { return Strings; }
    T3 GetSender() { return sender; }
    LEventArgs(T1 Vals, T2 Strings, T3 sender) { this->Type = 2; this->Vals = Vals; this->Strings = Strings; this->sender = sender; }
};

template<class T1>
class SenderEventArgs : public EventArgs
{
private:
    T1 sender;

public:
    T1 GetSender() { return sender; }
    SenderEventArgs(T1 sender) { this->sender = sender; }
};