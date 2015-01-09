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

#ifndef DSPRUNTYPE_H
#define DSPRUNTYPE_H

//-------------------------------------------------------------------------------------------------

#include <utility>
#include <typeinfo>

//=================================================================================================

class DspRunType
{
public:
	DspRunType()
	: _content( NULL ) {}

	template< typename ValueType >
	DspRunType( const ValueType& value )
	{
		_content = new _DspRtValue< ValueType >( value );
	}

	DspRunType( const DspRunType& other )
	{
		if( other._content != NULL )
		{
			_content = other._content->Clone();
		}
		else
		{
			_content = NULL;
		}
	}

	virtual ~DspRunType()
	{
		delete _content;
	}

public:
	DspRunType& Swap( DspRunType& rhs )
	{
		std::swap( _content, rhs._content );
		return *this;
	}

	template< typename ValueType >
	DspRunType& operator=( const ValueType& rhs )
	{
		DspRunType( rhs ).Swap( *this );
		return *this;
	}

	DspRunType& operator=( DspRunType rhs )
	{
		rhs.Swap( *this );
		return *this;
	}

public:
	bool IsEmpty() const
	{
		return !_content;
	}

	const std::type_info& GetType() const
	{
		if( _content != NULL )
		{
			return _content->GetType();
		}
		else
		{
			return typeid( void );
		}
	}

	template< typename ValueType >
	static ValueType* RunTypeCast( DspRunType* operand )
	{
		if( operand != NULL && operand->GetType() == typeid( ValueType ) )
		{
			return &static_cast< DspRunType::_DspRtValue< ValueType >* >( operand->_content )->held;
		}
		else
		{
			return NULL;
		}
	}

	template< typename ValueType >
	static inline const ValueType* RunTypeCast( const DspRunType* operand )
	{
		return RunTypeCast< ValueType >( const_cast< DspRunType* >( operand ) );
	}

private:
	class _DspRtValueHolder
	{
	public:
		virtual ~_DspRtValueHolder() {}

	public:
		virtual const std::type_info& GetType() const = 0;
		virtual _DspRtValueHolder* Clone() const = 0;
	};

	template< typename ValueType >
	class _DspRtValue : public _DspRtValueHolder
	{
	public:
		_DspRtValue( const ValueType& value )
		: held( value ) {}

	public:
		virtual const std::type_info& GetType() const
		{
			return typeid( ValueType );
		}

		virtual _DspRtValueHolder* Clone() const
		{
			return new _DspRtValue( held );
		}

	public:
		ValueType held;

	private:
		_DspRtValue& operator=( const _DspRtValue& );	// disable copy-assignment
	};

private:
	_DspRtValueHolder* _content;
};

//=================================================================================================

#endif // DSPRUNTYPE_H
