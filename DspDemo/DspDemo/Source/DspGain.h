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

#ifndef DSPGAIN_H
#define DSPGAIN_H

#include "DSPatch.h"

//=================================================================================================

class DspGain : public DspComponent
{
public:
	DspGain()
	: _gain( 1.0 )
	{
		AddInput_();
		AddOutput_();
	}
	~DspGain() {}

	void SetGain( float gain )
	{
		if( gain < 0.0 )
		{
			_gain = 0;
		}
		else
		{
			_gain = gain;
		}
	}

	float GetGain()
	{
		return _gain;
	}

protected:
	virtual void Process_( DspSignalBus& inputs, DspSignalBus& outputs )
	{
		if( !inputs.GetValue( 0, _stream ) )
		{
			_stream.assign( _stream.size(), 0 );
		}

		for( unsigned long i = 0; i < _stream.size(); i++ )
		{
			_stream[i] *= _gain;
		}

		outputs.SetValue( 0, _stream );
	}

private:
	std::vector< float > _stream;
	float _gain;
};

//=================================================================================================

#endif // DSPGAIN_H
