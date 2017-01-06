// IndexTriangle.h

#pragma once

#include "Defines.h"
#include "Vertex.h"

namespace _3DMath
{
	class IndexTriangle;
	class Triangle;
	class Plane;
}

class _3DMATH_API _3DMath::IndexTriangle
{
public:

	IndexTriangle( void );
	IndexTriangle( int vertex0, int vertex1, int vertex2 );
	~IndexTriangle( void );

	bool GetTriangle( Triangle& triangle, const std::vector< Vector >* vertexArray ) const;
	bool GetTriangle( Triangle& triangle, const std::vector< Vertex >* vertexArray ) const;
	bool GetPlane( Plane& plane, const std::vector< Vertex >* vertexArray ) const;
	bool HasVertex( int index ) const;
	bool CoincidentWith( const IndexTriangle& indexTriangle ) const;
	bool AdjacentTo( const IndexTriangle& indexTriangle ) const;
	int SharedVertexCount( const IndexTriangle& indexTriangle ) const;

	template< typename Type >
	static bool BoundsCheck( int index, const std::vector< Type >* vertexArray )
	{
		if( index < 0 || index >= ( signed )vertexArray->size() )
			return false;
		return true;
	}

	int vertex[3];
};

namespace _3DMath
{
	typedef std::list< IndexTriangle > IndexTriangleList;
}

// IndexTriangle.h
