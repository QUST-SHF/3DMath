// Exception.cpp

#include "Exception.h"

using namespace _3DMath;

//-----------------------------------------------------------------------
//                              Exception
//-----------------------------------------------------------------------

Exception::Exception( void )
{
    this->error = "unknown";
}

Exception::Exception( const std::string& error )
{
    this->error = error;
}

/*virtual*/ Exception::~Exception( void )
{
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

/*static*/ ExceptionCallback::ExceptionCallback* Get( void )
{
    return exceptionCallback;
}

ExceptionCallback* ExceptionCallback::exceptionCallback = nullptr;

// Exception.cpp