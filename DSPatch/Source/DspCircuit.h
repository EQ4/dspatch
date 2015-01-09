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

#ifndef DSPCIRCUIT_H
#define DSPCIRCUIT_H

//-------------------------------------------------------------------------------------------------

#include "DspComponent.h"

class DspCircuitThread;

//=================================================================================================

class DspCircuit : public DspComponent
{
public:
	DspCircuit( unsigned long threadCount = 0 );
	~DspCircuit();

	void PauseAutoTick();

	virtual void SetThreadCount( unsigned long threadCount );

	template< class ComponentType >
	DspSafePointer< ComponentType > AddComponent( std::string componentName );

	template< class ComponentType >
	bool AddComponent( DspSafePointer< ComponentType >& component, std::string componentName = "" );

	template< class ComponentType >
	bool GetComponent( std::string componentName, DspSafePointer< ComponentType >& returnComponent );

	void RemoveComponent( std::string componentName );
	void RemoveAllComponents();

	// component output to component input
	bool ConnectOutToIn( std::string fromComponent, unsigned long fromOutput, std::string toComponent, unsigned long toInput );
	bool ConnectOutToIn( std::string fromComponent, unsigned long fromOutput, std::string toComponent, std::string toInput );
	bool ConnectOutToIn( std::string fromComponent, std::string fromOutput, std::string toComponent, unsigned long toInput );
	bool ConnectOutToIn( std::string fromComponent, std::string fromOutput, std::string toComponent, std::string toInput );

	// circuit input to component input (currently only works with non-threaded ticking (_threadCount = 0))
	bool ConnectInToIn( unsigned long fromInput, std::string toComponent, unsigned long toInput );
	bool ConnectInToIn( unsigned long fromInput, std::string toComponent, std::string toInput );
	bool ConnectInToIn( std::string fromInput, std::string toComponent, unsigned long toInput );
	bool ConnectInToIn( std::string fromInput, std::string toComponent, std::string toInput );

	// component output to circuit output (currently only works with non-threaded ticking (_threadCount = 0))
	bool ConnectOutToOut( std::string fromComponent, unsigned long fromOutput, unsigned long toOutput );
	bool ConnectOutToOut( std::string fromComponent, unsigned long fromOutput, std::string toOutput );
	bool ConnectOutToOut( std::string fromComponent, std::string fromOutput, unsigned long toOutput );
	bool ConnectOutToOut( std::string fromComponent, std::string fromOutput, std::string toOutput );

	// component output to component input
	void DisconnectOutToIn( std::string fromComponent, unsigned long fromOutput, std::string toComponent, unsigned long toInput );
	void DisconnectOutToIn( std::string fromComponent, unsigned long fromOutput, std::string toComponent, std::string toInput );
	void DisconnectOutToIn( std::string fromComponent, std::string fromOutput, std::string toComponent, unsigned long toInput );
	void DisconnectOutToIn( std::string fromComponent, std::string fromOutput, std::string toComponent, std::string toInput );

	// circuit input to component input (currently only works with non-threaded ticking (_threadCount = 0))
	void DisconnectInToIn( unsigned long fromInput, std::string toComponent, unsigned long toInput );
	void DisconnectInToIn( unsigned long fromInput, std::string toComponent, std::string toInput );
	void DisconnectInToIn( std::string fromInput, std::string toComponent, unsigned long toInput );
	void DisconnectInToIn( std::string fromInput, std::string toComponent, std::string toInput );

	// component output to circuit output (currently only works with non-threaded ticking (_threadCount = 0))
	void DisconnectOutToOut( std::string fromComponent, unsigned long fromOutput, unsigned long toOutput );
	void DisconnectOutToOut( std::string fromComponent, unsigned long fromOutput, std::string toOutput );
	void DisconnectOutToOut( std::string fromComponent, std::string fromOutput, unsigned long toOutput );
	void DisconnectOutToOut( std::string fromComponent, std::string fromOutput, std::string toOutput );

	void DisconnectComponent( std::string component );

	bool AddInput( std::string inputName = "" );
	bool AddOutput( std::string outputName = "" );

	void ClearInputs();
	void ClearOutputs();

protected:
	virtual void Process_( DspSignalBus& inputs, DspSignalBus& outputs );

private:
	std::vector< DspSafePointer< DspComponent > > _components;

	DspWireBus* _inToInWires;
	DspWireBus* _outToOutWires;

	bool _FindComponent( DspSafePointer< DspComponent > component, unsigned long& returnIndex );
	bool _FindComponent( std::string componentName, unsigned long& returnIndex );
	void _DisconnectComponent( unsigned long componentIndex );
	void _RemoveComponent( unsigned long componentIndex );

	std::vector< DspSafePointer< DspCircuitThread > > _circuitThreads;

	unsigned long _currentThreadIndex;
};

//=================================================================================================

template< class ComponentType >
DspSafePointer< ComponentType > DspCircuit::AddComponent( std::string componentName )
{
	unsigned long componentIndex;

	if( _FindComponent( componentName, componentIndex ) )
	{
		return ( DspSafePointer< ComponentType > ) _components[componentIndex];	// if the component is already in the array
	}

	DspSafePointer< ComponentType > newComponent;
	newComponent.New();
	newComponent.LockPointer();

	newComponent->SetComponentName( componentName );
	newComponent->SetThreadCount( GetThreadCount() );

	PauseAutoTick();

	_components.push_back( newComponent );

	ResumeAutoTick();

	return _components[_components.size() - 1];
}

//-------------------------------------------------------------------------------------------------

template< class ComponentType >
bool DspCircuit::AddComponent( DspSafePointer< ComponentType >& component, std::string componentName )
{
	if( componentName == "" && component->GetComponentName() == "" )
	{
		return false;	// a component can't be added to the circuit without a name
	}

	unsigned long componentIndex;

	if( _FindComponent( component, componentIndex ) )
	{
		return false;	// if the component is already in the array
	}
	if( _FindComponent( component->GetComponentName(), componentIndex ) )
	{
		return false;	// if the component name is already in the array
	}

	component->SetComponentName( componentName );
	component->SetThreadCount( GetThreadCount() );

	component.LockPointer();	// lock the pointer so that it can't be re-newed

	PauseAutoTick();

	_components.push_back( component );

	ResumeAutoTick();

	return true;
}

//-------------------------------------------------------------------------------------------------

template< class ComponentType >
bool DspCircuit::GetComponent( std::string componentName, DspSafePointer< ComponentType >& returnComponent )
{
	unsigned long componentIndex;

	if( _FindComponent( componentName, componentIndex ) )
	{
		returnComponent = _components[componentIndex];
		return true;
	}
	else
	{
		return false;
	}
}

//=================================================================================================

#endif // DSPCIRCUIT_H
