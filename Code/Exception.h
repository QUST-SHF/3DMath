// Exception.h

#pragma once

#include "Defines.h"

namespace _3DMath
{
    class Exception;
	class ExceptionCallback;
}

class _3DMATH_API _3DMath::Exception
{
public:

    Exception( void );
    Exception( const std::string& error );
    virtual ~Exception( void );

    virtual void Handle( void );

    std::string* error;
};

class _3DMATH_API _3DMath::ExceptionCallback
{
public:

    ExceptionCallback( void );
    virtual ~ExceptionCallback( void );

    virtual void Call( Exception* exception ) = 0;

    static void Set( ExceptionCallback* callback );
    static ExceptionCallback* Get( void );

    static ExceptionCallback* exceptionCallback;
};

// Exception.h