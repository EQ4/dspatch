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
	bool AddComponent( DspSafePointer< ComponentType > component, std::string componentName = "" );

	template< class ComponentType >
	DspSafePointer< ComponentType > GetComponent( std::string componentName );

	void RemoveComponent( std::string componentName );
	void RemoveAllComponents();

	// component output to component input
	template< class FromComponentType, class FromOutputType, class ToComponentType, class ToInputType >
	bool ConnectOutToIn( FromComponentType fromComponent, FromOutputType fromOutput, ToComponentType toComponent, ToInputType toInput );

	// circuit input to component input
	template< class FromInputType, class ToComponentType, class ToInputType >
	bool ConnectInToIn( FromInputType fromInput, ToComponentType toComponent, ToInputType toInput );

	// component output to circuit output
	template< class FromComponentType, class FromOutputType, class ToOutputType >
	bool ConnectOutToOut( FromComponentType fromComponent, FromOutputType fromOutput, ToOutputType toOutput );

	// component output to component input
	template< class FromComponentType, class FromOutputType, class ToComponentType, class ToInputType >
	void DisconnectOutToIn( FromComponentType fromComponent, FromOutputType fromOutput, ToComponentType toComponent, ToInputType toInput );

	// circuit input to component input
	template< class FromInputType, class ToComponentType, class ToInputType >
	bool DisconnectInToIn( FromInputType fromInput, ToComponentType toComponent, ToInputType toInput );

	// component output to circuit output
	template< class FromComponentType, class FromOutputType, class ToOutputType >
	bool DisconnectOutToOut( FromComponentType fromComponent, FromOutputType fromOutput, ToOutputType toOutput );

	void DisconnectComponent( std::string component );

	bool AddInput( std::string inputName = "" );
	bool AddOutput( std::string outputName = "" );

	void ClearInputs();
	void ClearOutputs();

protected:
	virtual void Process_( DspSignalBus& inputs, DspSignalBus& outputs );

private:
	std::vector< DspSafePointer< DspComponent > > _components;

	std::vector< DspSafePointer< DspCircuitThread > > _circuitThreads;
	unsigned long _currentThreadIndex;

	DspWireBus* _inToInWires;
	DspWireBus* _outToOutWires;

	bool _FindComponent( DspSafePointer< DspComponent > component, unsigned long& returnIndex );
	bool _FindComponent( std::string componentName, unsigned long& returnIndex );

	void _DisconnectComponent( unsigned long componentIndex );
	void _RemoveComponent( unsigned long componentIndex );
};

//=================================================================================================

template< class ComponentType >
DspSafePointer< ComponentType > DspCircuit::AddComponent( std::string componentName )
{
	unsigned long componentIndex;

	if( _FindComponent( componentName, componentIndex ) )
	{
		return DspSafePointer< ComponentType >();	// if the component is already in the array
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
bool DspCircuit::AddComponent( DspSafePointer< ComponentType > component, std::string componentName )
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
DspSafePointer< ComponentType > DspCircuit::GetComponent( std::string componentName )
{
	unsigned long componentIndex;

	if( _FindComponent( componentName, componentIndex ) )
	{
		return _components[componentIndex];
	}
	else
	{
		return DspSafePointer< ComponentType >();
	}
}

//-------------------------------------------------------------------------------------------------

template< class FromComponentType, class FromOutputType, class ToComponentType, class ToInputType >
bool DspCircuit::ConnectOutToIn( FromComponentType fromComponent, FromOutputType fromOutput, ToComponentType toComponent, ToInputType toInput )
{
	unsigned long fromComponentIndex;
	unsigned long toComponentIndex;

	//only interconnect components that have been added to this system
	if( !_FindComponent( fromComponent, fromComponentIndex ) ||
			!_FindComponent( toComponent, toComponentIndex ) )
	{
		return false;
	}

	PauseAutoTick();

	bool result = _components[toComponentIndex]->ConnectInput( _components[fromComponentIndex], fromOutput, toInput );

	ResumeAutoTick();

	return result;
}

//-------------------------------------------------------------------------------------------------

template< class FromInputType, class ToComponentType, class ToInputType >
bool DspCircuit::ConnectInToIn( FromInputType fromInput, ToComponentType toComponent, ToInputType toInput )
{
	unsigned long fromInputIndex;
	unsigned long toComponentIndex;
	unsigned long toInputIndex;

	//only interconnect components that have been added to this system
	if( !FindInput( fromInput, fromInputIndex ) ||
			!_FindComponent( toComponent, toComponentIndex ) ||
			!_components[toComponentIndex]->FindInput( toInput, toInputIndex ) )
	{
		return false;
	}

	PauseAutoTick();

	bool result = _inToInWires->AddWire( _components[toComponentIndex], fromInputIndex, toInputIndex );

	ResumeAutoTick();

	return result;
}

//-------------------------------------------------------------------------------------------------

template< class FromComponentType, class FromOutputType, class ToOutputType >
bool DspCircuit::ConnectOutToOut( FromComponentType fromComponent, FromOutputType fromOutput, ToOutputType toOutput )
{
	unsigned long fromComponentIndex;
	unsigned long fromOutputIndex;
	unsigned long toOutputIndex;

	//only interconnect components that have been added to this system
	if( !_FindComponent( fromComponent, fromComponentIndex ) ||
			!_components[fromComponentIndex]->FindOutput( fromOutput, fromOutputIndex ) ||
			!FindOutput( toOutput, toOutputIndex ) )
	{
		return false;
	}

	PauseAutoTick();

	bool result = _outToOutWires->AddWire( _components[fromComponentIndex], fromOutputIndex, toOutputIndex );

	ResumeAutoTick();

	return result;
}

//-------------------------------------------------------------------------------------------------

template< class FromComponentType, class FromOutputType, class ToComponentType, class ToInputType >
void DspCircuit::DisconnectOutToIn( FromComponentType fromComponent, FromOutputType fromOutput, ToComponentType toComponent, ToInputType toInput )
{
	unsigned long fromComponentIndex;
	unsigned long toComponentIndex;

	//only interconnect components that have been added to this system
	if( !_FindComponent( fromComponent, fromComponentIndex ) ||
			!_FindComponent( toComponent, toComponentIndex ) )
	{
		return;
	}

	PauseAutoTick();

	_components[toComponentIndex]->DisconnectInput( _components[fromComponentIndex], fromOutput, toInput );

	ResumeAutoTick();
}

//-------------------------------------------------------------------------------------------------

template< class FromInputType, class ToComponentType, class ToInputType >
bool DspCircuit::DisconnectInToIn( FromInputType fromInput, ToComponentType toComponent, ToInputType toInput )
{
	unsigned long fromInputIndex;
	unsigned long toComponentIndex;
	unsigned long toInputIndex;

	//only interconnect components that have been added to this system
	if( !FindInput( fromInput, fromInputIndex ) ||
			!_FindComponent( toComponent, toComponentIndex ) ||
			!_components[toComponentIndex]->FindInput( toInput, toInputIndex ) )
	{
		return false;
	}

	PauseAutoTick();

	bool result = _inToInWires->RemoveWire( _components[toComponentIndex], fromInputIndex, toInputIndex );

	ResumeAutoTick();

	return result;
}

//-------------------------------------------------------------------------------------------------

template< class FromComponentType, class FromOutputType, class ToOutputType >
bool DspCircuit::DisconnectOutToOut( FromComponentType fromComponent, FromOutputType fromOutput, ToOutputType toOutput )
{
	unsigned long fromComponentIndex;
	unsigned long fromOutputIndex;
	unsigned long toOutputIndex;

	//only interconnect components that have been added to this system
	if( !_FindComponent( fromComponent, fromComponentIndex ) ||
			!_components[fromComponentIndex]->FindOutput( fromOutput, fromOutputIndex ) ||
			!FindOutput( toOutput, toOutputIndex ) )
	{
		return false;
	}

	PauseAutoTick();

	bool result = _outToOutWires->RemoveWire( _components[fromComponentIndex], fromOutputIndex, toOutputIndex );

	ResumeAutoTick();

	return result;
}

//=================================================================================================

#endif // DSPCIRCUIT_H
