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

#ifndef DSPOSCILLATOR_H
#define DSPOSCILLATOR_H

#include "DSPatch.h"

//=================================================================================================

class DspOscillator : public DspComponent
{
public:
	DspOscillator( float startFreq = 1000.0 , float startAmpl = 1.0 );
	~DspOscillator();

	void SetBufferSize( unsigned long bufferSize );
	void SetSampleRate( unsigned long sampleRate );

	void SetAmpl( float ampl );
	void SetFreq( float freq );

	unsigned long GetBufferSize()
	{
		return _bufferSize;
	}

	unsigned long GetSampleRate()
	{
		return _sampleRate;
	}

	float GetAmpl()
	{
		return _ampl;
	}

	float GetFreq()
	{
		return _freq;
	}

protected:
	virtual void Process_( DspSignalBus& inputs, DspSignalBus& outputs );

private:
	float _freq;
	float _ampl;

	std::vector< float > _signalLookup;
	std::vector< float > _signal;

	unsigned long _lastPos;
	unsigned long _lookupLength;
	unsigned long _sampleRate;
	unsigned long _bufferSize;

	DspMutex _processMutex;

	void _BuildLookup();
};

//=================================================================================================

#endif /* DSPOSCILLATOR_H */
