/************************************************************************
DSPatch - Cross-Platform, Object-Oriented, Flow-Based Programming Library
Copyright (c) 2012-2015 Marcus Tomlinson

This file is part of DSPatch.

GNU Lesser General Public License Usage
This file may be used under the terms of the GNU Lesser General Public
License version 3.0 as published by the Free Software Foundation and
appearing in the file LGPLv3.txt included in the packaging of this
file. Please review the following information to ensure the GNU Lesser
General Public License version 3.0 requirements will be met:
http://www.gnu.org/copyleft/lgpl.html.

Other Usage
Alternatively, this file may be used in accordance with the terms and
conditions contained in a signed written agreement between you and
Marcus Tomlinson.

DSPatch is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
************************************************************************/

#include <dspatch/DspWireBus.h>
#include <dspatch/DspComponent.h>
#include <dspatch/DspWire.h>

//=================================================================================================

DspWireBus::DspWireBus(bool isLinkedComponentReceivingSignals)
    : _isLinkedComponentReceivingSignals(isLinkedComponentReceivingSignals)
{
}

//-------------------------------------------------------------------------------------------------

DspWireBus::~DspWireBus()
{
    RemoveAllWires();
}

//=================================================================================================

bool DspWireBus::AddWire(DspComponent* linkedComponent, int fromSignalIndex, int toSignalIndex)
{
    for (size_t i = 0; i < _wires.size(); i++)
    {
        if (_wires[i].linkedComponent == linkedComponent && _wires[i].fromSignalIndex == fromSignalIndex &&
            _wires[i].toSignalIndex == toSignalIndex)
        {
            return false;  // wire already exists
        }
    }

    for (size_t i = 0; i < _wires.size(); i++)
    {
        if (_isLinkedComponentReceivingSignals && _wires[i].linkedComponent == linkedComponent &&
            _wires[i].toSignalIndex == toSignalIndex)  // if there's a wire to the receiving component's input already
        {
            RemoveWire(i);  // remove the wire (only one wire can connect to an input at a time)
            break;
        }
        else if (!_isLinkedComponentReceivingSignals && _wires[i].toSignalIndex == toSignalIndex)
        {
            RemoveWire(i);  // remove the wire (only one wire can connect to an input at a time)
            break;
        }
    }

    _wires.push_back(DspWire(linkedComponent, fromSignalIndex, toSignalIndex));

    return true;
}

//-------------------------------------------------------------------------------------------------

bool DspWireBus::RemoveWire(DspComponent const* linkedComponent, int fromSignalIndex, int toSignalIndex)
{
    for (size_t i = 0; i < _wires.size(); i++)
    {
        if (_wires[i].linkedComponent == linkedComponent && _wires[i].fromSignalIndex == fromSignalIndex &&
            _wires[i].toSignalIndex == toSignalIndex)
        {
            RemoveWire(i);
            return true;
        }
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

bool DspWireBus::RemoveWire(int wireIndex)
{
    if ((size_t)wireIndex > _wires.size())
    {
        return false;
    }

    _wires.erase(_wires.begin() + wireIndex);

    return true;
}

//-------------------------------------------------------------------------------------------------

void DspWireBus::RemoveAllWires()
{
    for (size_t i = 0; i < _wires.size(); i++)
    {
        RemoveWire(i);
    }
}

//-------------------------------------------------------------------------------------------------

DspWire* DspWireBus::GetWire(int wireIndex)
{
    if ((size_t)wireIndex < _wires.size())
    {
        return &_wires[wireIndex];
    }
    else
    {
        return NULL;
    }
}

//-------------------------------------------------------------------------------------------------

int DspWireBus::GetWireCount() const
{
    return _wires.size();
}

//=================================================================================================
