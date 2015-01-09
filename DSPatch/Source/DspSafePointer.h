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

#ifndef DSPSAFEPOINTER_H
#define DSPSAFEPOINTER_H

//-------------------------------------------------------------------------------------------------

#include "DspThread.h"

//=================================================================================================

template< class PointerType >
class DspSafePointer
{
public:
	DspSafePointer()
	: _pointer( NULL ),
		_pointerLocked( NULL ),
		_pointerLockedMutex( NULL ),
		_referenceCount( NULL ),
		_referenceCountMutex( NULL ) {}

	DspSafePointer( const PointerType& other )
	: _pointer( NULL ),
		_pointerLocked( NULL ),
		_pointerLockedMutex( NULL ),
		_referenceCount( NULL ),
		_referenceCountMutex( NULL )
	{
		_pointer = new PointerType( other );
		_AttachToPointer();
	}

	DspSafePointer( const DspSafePointer& other )
	: _pointer( NULL ),
		_pointerLocked( NULL ),
		_pointerLockedMutex( NULL ),
		_referenceCount( NULL ),
		_referenceCountMutex( NULL )
	{
		if( other.IsPointerValid() )
		{
			_pointer = other._pointer;
			_pointerLocked = other._pointerLocked;
			_pointerLockedMutex = other._pointerLockedMutex;
			_referenceCount = other._referenceCount;
			_referenceCountMutex = other._referenceCountMutex;
			_AttachToPointer();
		}
	}

	template< class OtherType >
	DspSafePointer< PointerType >( const DspSafePointer< OtherType >& other )
	: _pointer( NULL ),
		_pointerLocked( NULL ),
		_pointerLockedMutex( NULL ),
		_referenceCount( NULL ),
		_referenceCountMutex( NULL )
	{
		if( other.IsPointerValid() )
		{
			_pointer = ( ( DspSafePointer< PointerType >& ) other )._pointer;
			_pointerLocked = ( ( DspSafePointer< PointerType >& ) other )._pointerLocked;
			_pointerLockedMutex = ( ( DspSafePointer< PointerType >& ) other )._pointerLockedMutex;
			_referenceCount = ( ( DspSafePointer< PointerType >& ) other )._referenceCount;
			_referenceCountMutex = ( ( DspSafePointer< PointerType >& ) other )._referenceCountMutex;
			_AttachToPointer();
		}
	}

	virtual ~DspSafePointer()
	{
		_DetachFromPointer();
	}

public:
	bool New()
	{
		if( _pointer == NULL || *_pointerLocked == false )
		{
			_DetachFromPointer();
			_pointer = new PointerType();
			_AttachToPointer();
			return true;
		}
		return false;
	}

	template< class T1 >
	bool New( T1& p1 )
	{
		if( _pointer == NULL || *_pointerLocked == false )
		{
			_DetachFromPointer();
			_pointer = new PointerType( p1 );
			_AttachToPointer();
			return true;
		}
		return false;
	}

	template< class T1, class T2 >
	bool New( T1& p1, T2& p2 )
	{
		if( _pointer == NULL || *_pointerLocked == false )
		{
			_DetachFromPointer();
			_pointer = new PointerType( p1, p2 );
			_AttachToPointer();
			return true;
		}
		return false;
	}

	template< class T1, class T2, class T3 >
	bool New( T1& p1, T2& p2, T3& p3 )
	{
		if( _pointer == NULL || *_pointerLocked == false )
		{
			_DetachFromPointer();
			_pointer = new PointerType( p1, p2, p3 );
			_AttachToPointer();
			return true;
		}
		return false;
	}

	template< class T1, class T2, class T3, class T4 >
	bool New( T1& p1, T2& p2, T3& p3, T4& p4 )
	{
		if( _pointer == NULL || *_pointerLocked == false )
		{
			_DetachFromPointer();
			_pointer = new PointerType( p1, p2, p3, p4 );
			_AttachToPointer();
			return true;
		}
		return false;
	}

	template< class T1, class T2, class T3, class T4, class T5 >
	bool New( T1& p1, T2& p2, T3& p3, T4& p4, T5& p5 )
	{
		if( _pointer == NULL || *_pointerLocked == false )
		{
			_DetachFromPointer();
			_pointer = new PointerType( p1, p2, p3, p4, p5 );
			_AttachToPointer();
			return true;
		}
		return false;
	}

	template< class T1, class T2, class T3, class T4, class T5, class T6 >
	bool New( T1& p1, T2& p2, T3& p3, T4& p4, T5& p5, T6& p6 )
	{
		if( _pointer == NULL || *_pointerLocked == false )
		{
			_DetachFromPointer();
			_pointer = new PointerType( p1, p2, p3, p4, p5, p6 );
			_AttachToPointer();
			return true;
		}
		return false;
	}

	template< class T1, class T2, class T3, class T4, class T5, class T6, class T7 >
	bool New( T1& p1, T2& p2, T3& p3, T4& p4, T5& p5, T6& p6, T7& p7 )
	{
		if( _pointer == NULL || *_pointerLocked == false )
		{
			_DetachFromPointer();
			_pointer = new PointerType( p1, p2, p3, p4, p5, p6, p7 );
			_AttachToPointer();
			return true;
		}
		return false;
	}

	template< class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8 >
	bool New( T1& p1, T2& p2, T3& p3, T4& p4, T5& p5, T6& p6, T7& p7, T8& p8 )
	{
		if( _pointer == NULL || *_pointerLocked == false )
		{
			_DetachFromPointer();
			_pointer = new PointerType( p1, p2, p3, p4, p5, p6, p7, p8 );
			_AttachToPointer();
			return true;
		}
		return false;
	}

	template< class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9 >
	bool New( T1& p1, T2& p2, T3& p3, T4& p4, T5& p5, T6& p6, T7& p7, T8& p8, T9& p9 )
	{
		if( _pointer == NULL || *_pointerLocked == false )
		{
			_DetachFromPointer();
			_pointer = new PointerType( p1, p2, p3, p4, p5, p6, p7, p8, p9 );
			_AttachToPointer();
			return true;
		}
		return false;
	}

	template< class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10 >
	bool New( T1& p1, T2& p2, T3& p3, T4& p4, T5& p5, T6& p6, T7& p7, T8& p8, T9& p9, T10& p10 )
	{
		if( _pointer == NULL || *_pointerLocked == false )
		{
			_DetachFromPointer();
			_pointer = new PointerType( p1, p2, p3, p4, p5, p6, p7, p8, p9, p10 );
			_AttachToPointer();
			return true;
		}
		return false;
	}

	bool Detach()
	{
		if( *_pointerLocked == false )
		{
			_DetachFromPointer();
			return true;
		}
		return false;
	}

	bool IsPointerValid() const
	{
		return _pointer != NULL &&
			_pointerLocked != NULL &&
			_pointerLockedMutex != NULL &&
			_referenceCount != NULL &&
			_referenceCountMutex != NULL;
	}

	void LockPointer()
	{
		if( _pointerLocked != NULL )
		{
			_pointerLockedMutex->Lock();
			*_pointerLocked = true;
			_pointerLockedMutex->Unlock();
		}
	}

public:
	bool operator==( const PointerType& rhs )
	{
		if( IsPointerValid() )
		{
			return *_pointer == rhs;
		}
		else
		{
			return false;
		}
	}

	bool operator!=( const PointerType& rhs )
	{
		if( IsPointerValid() )
		{
			return *_pointer != rhs;
		}
		else
		{
			return true;
		}
	}

	bool operator==( const DspSafePointer& rhs )
	{
		return _pointer == rhs._pointer;
	}

	bool operator!=( const DspSafePointer& rhs )
	{
		return _pointer != rhs._pointer;
	}

	template< class OtherType >
	bool operator==( const DspSafePointer< OtherType >& rhs )
	{
		return _pointer == ( ( DspSafePointer< PointerType >& ) rhs )._pointer;
	}

	template< class OtherType >
	bool operator!=( const DspSafePointer< OtherType >& rhs )
	{
		return _pointer != ( ( DspSafePointer< PointerType >& ) rhs )._pointer;
	}

public:
	DspSafePointer& operator=( const PointerType& rhs )
	{
		if( IsPointerValid() )
		{
			*_pointer = rhs;
		}
		else
		{
			_pointer = new PointerType( rhs );
			_AttachToPointer();
		}
		return *this;
	}

	DspSafePointer& operator=( const DspSafePointer& rhs )
	{
		if( this != &rhs && rhs.IsPointerValid() )
		{
			_DetachFromPointer();
			_pointer = rhs._pointer;
			_pointerLocked = rhs._pointerLocked;
			_pointerLockedMutex = rhs._pointerLockedMutex;
			_referenceCount = rhs._referenceCount;
			_referenceCountMutex = rhs._referenceCountMutex;
			_AttachToPointer();
		}
		return *this;
	}

	template< class OtherType >
	DspSafePointer< PointerType >& operator=( const DspSafePointer< OtherType >& rhs )
	{
		if( this != &( ( DspSafePointer< PointerType >& ) rhs ) && rhs.IsPointerValid() )
		{
			_DetachFromPointer();
			_pointer = ( ( DspSafePointer< PointerType >& ) rhs )._pointer;
			_pointerLocked = ( ( DspSafePointer< PointerType >& ) rhs )._pointerLocked;
			_pointerLockedMutex = ( ( DspSafePointer< PointerType >& ) rhs )._pointerLockedMutex;
			_referenceCount = ( ( DspSafePointer< PointerType >& ) rhs )._referenceCount;
			_referenceCountMutex = ( ( DspSafePointer< PointerType >& ) rhs )._referenceCountMutex;
			_AttachToPointer();
		}
		return *this;
	}

public:
	PointerType* const operator->() const
	{
		if( !IsPointerValid() )
		{
			throw std::exception();
		}

		return _pointer;
	}

	// this can be dangerous as default copy constructed objects screw up internal pointers when destroyed
	//const PointerType operator*() const	
	//{
	//   if( !IsPointerValid() )
	//   {
	//     throw std::exception();
	//   }
	// 
	// 	return *_pointer;
	//}

	//-- Private Methods
private:
	void _DetachFromPointer()
	{
		if( _referenceCount != NULL )
		{
			_referenceCountMutex->Lock();
			( *_referenceCount )--;
			_referenceCountMutex->Unlock();

			if( *_referenceCount == 0 )
			{
				delete _pointer;
				delete _pointerLocked;
				delete _pointerLockedMutex;
				delete _referenceCount;
				delete _referenceCountMutex;
			}
		}
		_pointer = NULL;
		_referenceCount = NULL;
	}

	void _AttachToPointer()
	{
		if( _referenceCount != NULL )
		{
			_referenceCountMutex->Lock();
			( *_referenceCount )++;
			_referenceCountMutex->Unlock();
		}
		else if( _pointer != NULL )
		{
			_pointerLocked = new bool( false );
			_pointerLockedMutex = new DspMutex();
			_referenceCount = new unsigned long( 1 );
			_referenceCountMutex = new DspMutex();
		}
	}

	//-- Private Members
private:
	PointerType* _pointer;

	bool* _pointerLocked;
	DspMutex* _pointerLockedMutex;

	unsigned long* _referenceCount;
	DspMutex* _referenceCountMutex;
};

//=================================================================================================

#endif // DSPSAFEPOINTER_H
