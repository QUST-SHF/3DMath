// ListFunctions.h

#pragma once

#include <list>

namespace _3DMath
{
	template< typename Type >
	void FreeList( std::list< Type* >& list )
	{
		while( list.size() > 0 )
		{
			typename std::list< Type* >::iterator iter = list.begin();
			Type* type = *iter;
			delete type;
			list.erase( iter );
		}
	}

	class Renderer;

	template< typename Type >
	void RenderList( std::list< Type* >& list, Renderer& renderer )
	{
		for( typename std::list< Type* >::iterator iter = list.begin(); iter != list.end(); iter++ )
		{
			Type* type = *iter;
			type->Render( renderer );
		}
	}
}

// ListFunctions.h
