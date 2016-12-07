// Exception.cpp

#include "Exception.h"

using namespace _3DMath;

//-----------------------------------------------------------------------
//                              Exception
//-----------------------------------------------------------------------

Exception::Exception( void )
{
    this->error = nullptr;
}

Exception::Exception( const std::string& error )
{
    this->error = new std::string( error );
}

/*virtual*/ Exception::~Exception( void )
{
	delete error;
}

/*virtual*/ void Exception::Handle( void )
{
    if( ExceptionCallback::exceptionCallback )
        ExceptionCallback::exceptionCallback->Call( this );
}

//-----------------------------------------------------------------------
//                           ExceptionCallback
//-----------------------------------------------------------------------

ExceptionCallback::ExceptionCallback( void )
{
}

/*virtual*/ ExceptionCallback::~ExceptionCallback( void )
{
}

/*static*/ void ExceptionCallback::Set( ExceptionCallback* callback )
{
    exceptionCallback = callback;
}

/*static*/ ExceptionCallback* ExceptionCallback::Get( void )
{
    return exceptionCallback;
}

ExceptionCallback* ExceptionCallback::exceptionCallback = nullptr;

// Exception.cpp