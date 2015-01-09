/********************************************************************
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

#ifndef DSPCIRCUITTHREAD_H
#define DSPCIRCUITTHREAD_H

//-------------------------------------------------------------------------------------------------

#include <vector>

#include "DspThreadWin.h"
#include "DspSafePointer.h"

class DspComponent;

//=================================================================================================

class DspCircuitThread : public DspThread
{
public:
	DspCircuitThread( std::vector< DspSafePointer< DspComponent > >& components, unsigned long threadNo );
	~DspCircuitThread();

	void Start( Priority priority = TimeCriticalPriority );
	void Stop();
	void Sync();
	void Resume();

private:
	std::vector< DspSafePointer< DspComponent > >& _components;
	unsigned long _threadNo;
	bool _stop;
	bool _stopped;
	bool _gotResume, _gotSync;
	DspMutex _resumeMutex;
	DspWaitCondition _resumeCondt, _syncCondt;

	void _Run();
};

//=================================================================================================

#endif // DSPCIRCUITTHREAD_H
