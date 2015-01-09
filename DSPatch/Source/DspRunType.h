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
