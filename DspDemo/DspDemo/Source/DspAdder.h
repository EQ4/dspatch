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
