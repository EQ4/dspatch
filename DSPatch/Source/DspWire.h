/********************************************************************
DSPatch - Real-Time, Multi-Purpose Circuit Builder / Simulator Engine
Copyright (c) 2012 Marcus Tomlinson / Adapt Audio

This file is part of DSPatch.

DSPatch is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

DSPatch is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with DSPatch.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

#ifndef DSPWIRE_H
#define DSPWIRE_H

//-------------------------------------------------------------------------------------------------

#include "DspSafePointer.h"
#include "DspComponent.h"

//=================================================================================================

struct DspWire
{
	DspSafePointer< DspComponent > linkedComponent;
	unsigned long fromSignalIndex;
	unsigned long toSignalIndex;
};

//=================================================================================================

#endif // DSPWIRE_H
