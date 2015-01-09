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
