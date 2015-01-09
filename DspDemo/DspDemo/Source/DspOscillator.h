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
