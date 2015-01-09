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

#ifndef DSPCIRCUITTHREAD_H
#define DSPCIRCUITTHREAD_H

//-------------------------------------------------------------------------------------------------

#include <vector>

#include "DspThread.h"
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
