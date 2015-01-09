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

#ifndef DSPCOMPONENTTHREAD_H
#define DSPCOMPONENTTHREAD_H

//-------------------------------------------------------------------------------------------------

#include "DspThread.h"

class DspComponent;

//=================================================================================================

class DspComponentThread : public DspThread
{
public:
	DspComponentThread( DspComponent& component );
	~DspComponentThread();

	void Start( Priority priority = TimeCriticalPriority );
	void Stop();
	void Pause();
	void Resume();

private:
	DspComponent& _component;
	bool _stop, _pause;
	bool _stopped;
	DspMutex _resumeMutex, _pauseMutex;
	DspWaitCondition _resumeCondt, _pauseCondt;

	void _Run();
};

//=================================================================================================

#endif // DSPCOMPONENTTHREAD_H
