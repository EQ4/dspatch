/************************************************************************
DSPatch - Cross-Platform, Object-Oriented, Flow-Based Programming Library
Copyright (c) 2012-2013 Marcus Tomlinson

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
/// Dynamically typed variable

/**
DspRunType holds a variable that can be dynamically typed at run-time (hence the name). The
DspRunType class makes use of an internal template class and public template methods to allow
users to get and set the contained variable as any type they wish. A DspRunType object also has the
ability to change type at any point during program execution. Built-in typecasting and error
checking (via the RunTypeCast() method) prevents critical runtime errors from occurring when signal
types are mismatched.
*/

class DspRunType
{
public:
  DspRunType()
  : _valueHolder( NULL ) {}

  template< typename ValueType >
  DspRunType( const ValueType& value )
  {
    _valueHolder = new _DspRtValue< ValueType >( value );
  }

  DspRunType( const DspRunType& other )
  {
    if( other._valueHolder != NULL )
    {
      _valueHolder = other._valueHolder->Copy();
    }
    else
    {
      _valueHolder = NULL;
    }
  }

  virtual ~DspRunType()
  {
    delete _valueHolder;
  }

public:
  DspRunType& Swap( DspRunType& rhs )
  {
    std::swap( _valueHolder, rhs._valueHolder );
    return *this;
  }

  template< typename ValueType >
  DspRunType& operator=( const ValueType& rhs )
  {
    if( typeid( rhs ) == GetType() )
    {
      ( ( _DspRtValue< ValueType >* ) _valueHolder )->value = rhs;
    }
    else
    {
      DspRunType( rhs ).Swap( *this );
    }
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
    return !_valueHolder;
  }

  const std::type_info& GetType() const
  {
    if( _valueHolder != NULL )
    {
      return _valueHolder->GetType();
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
      return &static_cast< DspRunType::_DspRtValue< ValueType >* >( operand->_valueHolder )->value;
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
    virtual _DspRtValueHolder* Copy() const = 0;
  };

  template< typename ValueType >
  class _DspRtValue : public _DspRtValueHolder
  {
  public:
    _DspRtValue( const ValueType& value )
    : value( value ) {}

  public:
    virtual const std::type_info& GetType() const
    {
      return typeid( ValueType );
    }

    virtual _DspRtValueHolder* Copy() const
    {
      return new _DspRtValue( value );
    }

  public:
    ValueType value;

  private:
    _DspRtValue& operator=( const _DspRtValue& ); // disable copy-assignment
  };

private:
  _DspRtValueHolder* _valueHolder;
};

//=================================================================================================

#endif // DSPRUNTYPE_H
