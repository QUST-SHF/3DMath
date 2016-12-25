// HandleObject.cpp

#include "HandleObject.h"

namespace _3DMath
{
	int HandleObject::newHandle = 1;
	static HandleObjectMap handleObjectMapStorage;
	HandleObjectMap* HandleObject::handleObjectMap = &handleObjectMapStorage;
}

using namespace _3DMath;

HandleObject::HandleObject( void )
{
	handle = newHandle++;
	if( handleObjectMap )
		handleObjectMap->insert( std::pair< int, HandleObject* >( handle, this ) );
}

/*virtual*/ HandleObject::~HandleObject( void )
{
	if( handleObjectMap )
	{
		HandleObjectMap::iterator iter = handleObjectMap->find( handle );
		if( iter != handleObjectMap->end() )		// This should always happen.
			handleObjectMap->erase( iter );
	}
}

// Of course, we can't gurantee that the returned pointer can't somehow become
// stale before the caller is finished using it.  A better handle system might
// be built on top of a reference counting scheme.
/*static*/ HandleObject* HandleObject::Dereference( int handle )
{
	if( handleObjectMap )
	{
		HandleObjectMap::iterator iter = handleObjectMap->find( handle );
		if( iter != handleObjectMap->end() )
			return iter->second;
	}

	return nullptr;
}

/*virtual*/ void HandleObject::Render( Renderer& renderer ) const
{
}

/*virtual*/ HandleObject* HandleObject::Clone( void ) const
{
	return nullptr;
}

// HandleObject.cpp