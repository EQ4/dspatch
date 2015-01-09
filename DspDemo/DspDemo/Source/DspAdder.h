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

#ifndef DSPADDER_H
#define DSPADDER_H

//=================================================================================================

class DspAdder : public DspComponent
{
public:
	DspAdder()
	{
		AddInput_();
		AddInput_();
		AddOutput_();
	}
	~DspAdder() {}

protected:
	virtual void Process_( DspSignalBus& inputs, DspSignalBus& outputs )
	{
		// get inputs
		if( !inputs.GetValue( 0, _stream1 ) )
		{
			_stream1.assign( _stream1.size(), 0 );
		}
		if( !inputs.GetValue( 1, _stream2 ) )
		{
			_stream2.assign( _stream2.size(), 0 );
		}

		// if input sizes match
		if( _stream1.size() == _stream2.size() )
		{
			for( unsigned long i = 0; i < _stream1.size(); i++ )
			{
				_stream1[i] += _stream2[i];	// perform addition
			}
			outputs.SetValue( 0, _stream1 );
		}
			// if input sizes don't match
		else
		{
			outputs.ClearValue( 0 );	// clear the output
		}
	}

private:
	std::vector< float > _stream1, _stream2;
};

//=================================================================================================

#endif // DSPADDER_H
