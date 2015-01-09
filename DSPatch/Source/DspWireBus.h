/************************************************************************
DSPatch - Cross-Platform, Object-Oriented, Flow-Based Programming Library
Copyright (c) 2012 Marcus Tomlinson

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

#ifndef DSPWIREBUS_H
#define DSPWIREBUS_H

//-------------------------------------------------------------------------------------------------

#include <vector>

#include "DspSafePointer.h"
#include "DspWire.h"

class DspComponent;

//=================================================================================================

class DspWireBus
{
public:
	DspWireBus( bool isLinkedComponentReceivingSignals = false );
	virtual ~DspWireBus();

	bool AddWire( DspSafePointer< DspComponent > linkedComponent, unsigned long fromSignalIndex, unsigned long toSignalIndex );

	bool RemoveWire( unsigned long wireIndex );

	bool RemoveWire( DspSafePointer< DspComponent > linkedComponent, unsigned long fromSignalIndex, unsigned long toSignalIndex );

	void RemoveAllWires();

	DspSafePointer< DspWire > GetWire( unsigned long wireIndex );

	unsigned long GetWireCount();

private:
	bool _isLinkedComponentReceivingSignals;
	std::vector< DspSafePointer< DspWire > > _wires;
};

//=================================================================================================

#endif // DSPWIREBUS_H
