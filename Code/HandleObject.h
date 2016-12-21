// HandleObject.h

#pragma once

#include "Defines.h"

namespace _3DMath
{
	class HandleObject;
	class Renderer;

	typedef std::map< int, HandleObject* > HandleObjectMap;
}

class _3DMATH_API _3DMath::HandleObject
{
public:

	HandleObject( void );
	virtual ~HandleObject( void );

	virtual void Render( Renderer& renderer ) const;

	int GetHandle( void ) const { return handle; }

	static HandleObject* Dereference( int handle );

private:

	int handle;

	static int newHandle;
	static HandleObjectMap* handleObjectMap;
};

// HandleObject.h
