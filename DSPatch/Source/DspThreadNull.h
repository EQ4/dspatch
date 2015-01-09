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

#ifndef DSPTHREADNULL_H
#define DSPTHREADNULL_H

//=================================================================================================

class DspThread
{
public:
	DspThread() {}

	virtual ~DspThread() {}

	enum Priority
	{
		IdlePriority,

		LowestPriority,
		LowPriority,
		NormalPriority,
		HighPriority,
		HighestPriority,

		TimeCriticalPriority,
	};

	virtual void Start( Priority priority ) {}
	static void SetPriority( Priority priority ) {}
	static void MsSleep( unsigned long milliseconds ) {}
};

//=================================================================================================

class DspMutex
{
public:
	DspMutex() {}

	virtual ~DspMutex() {}

	void Lock() {}
	void Unlock() {}
};

//=================================================================================================

class DspWaitCondition
{
public:
	DspWaitCondition() {}

	virtual ~DspWaitCondition() {}

	void Wait( DspMutex& mutex ) {}
	void WakeAll() {}
};

//=================================================================================================

#endif // DSPTHREADNULL_H
