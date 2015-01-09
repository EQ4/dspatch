#include "../include/DSPatch.h"

//=================================================================================================

DspCircuit* DSPatch::_globalCircuit = new DspCircuit();

//=================================================================================================

bool DSPatch::IsThisGlobalCircuit( DspComponent* thisComponent )
{
	if( _globalCircuit != NULL )
	{
		return _globalCircuit == thisComponent;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------

bool DSPatch::AddGlobalComponent( DspComponent* component, std::string componentName )
{
	if( _globalCircuit != NULL )
	{
		return _globalCircuit->AddComponent( component, componentName );
	}

	return false;
}

//-------------------------------------------------------------------------------------------------

void DSPatch::RemoveGlobalComponent( DspComponent* component, bool deleteComponent )
{
	if( _globalCircuit != NULL )
	{
		return _globalCircuit->RemoveComponent( component );
	}
}

//-------------------------------------------------------------------------------------------------

unsigned long DSPatch::GetGlobalComponentCount()
{
	if( _globalCircuit != NULL )
	{
		return _globalCircuit->GetComponentCount();
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------

void DSPatch::StartGlobalAutoTick()
{
	if( _globalCircuit != NULL )
	{
		_globalCircuit->StartAutoTick();
	}
}

//-------------------------------------------------------------------------------------------------

void DSPatch::StopGlobalAutoTick()
{
	if( _globalCircuit != NULL )
	{
		_globalCircuit->StopAutoTick();
	}
}

//-------------------------------------------------------------------------------------------------

void DSPatch::SetGlobalThreadCount( unsigned long threadCount )
{
	if( _globalCircuit != NULL )
	{
		_globalCircuit->SetThreadCount( threadCount );
	}
}

//-------------------------------------------------------------------------------------------------

void DSPatch::Finalize()
{
	delete _globalCircuit;
	_globalCircuit = NULL;
}

//=================================================================================================