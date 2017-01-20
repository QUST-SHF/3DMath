// HandleObject.h

#pragma once

#include "Defines.h"

namespace _3DMath
{
	class HandleObject;
	class Renderer;

	typedef std::map< int, HandleObject* > HandleObjectMap;
	typedef std::list< int > ObjectHandleList;
}

class _3DMATH_API _3DMath::HandleObject
{
public:

	HandleObject( void );
	virtual ~HandleObject( void );

	virtual void Render( Renderer& renderer ) const;
	virtual HandleObject* Clone( void ) const;

	int GetHandle( void ) const { return handle; }

	static HandleObject* Dereference( int handle );

private:

	int handle;

	static int newHandle;
	static HandleObjectMap* handleObjectMap;
};

// HandleObject.h
