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

#ifndef DSPTHREADWIN_H
#define DSPTHREADWIN_H

//-------------------------------------------------------------------------------------------------

#include <windows.h>

//=================================================================================================

class DspThread
{
private:

	static DWORD WINAPI _ThreadFunc( LPVOID pv );

	virtual void _Run() = 0;

	HANDLE _threadHandle;

public:
	DspThread();
	DspThread( const DspThread& );
	virtual ~DspThread();

	enum Priority
	{
		IdlePriority = -15,

		LowestPriority = -2,
		LowPriority = -1,
		NormalPriority = 0,
		HighPriority = 1,
		HighestPriority = 2,

		TimeCriticalPriority = 15,
	};

	virtual void Start( Priority priority = NormalPriority );

	static void SetPriority( Priority priority );

	static void MsSleep( unsigned long milliseconds );
};

//=================================================================================================

class DspMutex
{
private:

	CRITICAL_SECTION _cs;

public:

	DspMutex();
	DspMutex( const DspMutex& );
	virtual ~DspMutex();

	void Lock();

	void Unlock();
};

//=================================================================================================

class DspWaitCondition
{
private:

	HANDLE _hEvent;

public:

	DspWaitCondition();
	DspWaitCondition( const DspWaitCondition& );
	virtual ~DspWaitCondition();

	void Wait( DspMutex& mutex );

	void WakeAll();
};

//=================================================================================================

#endif // DSPTHREADWIN_H
