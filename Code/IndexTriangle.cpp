// IndexTriangle.cpp

#include "IndexTriangle.h"
#include "Triangle.h"
#include "Renderer.h"

using namespace _3DMath;

IndexTriangle::IndexTriangle( void )
{
	vertex[0] = 0;
	vertex[1] = 0;
	vertex[2] = 0;
}

IndexTriangle::IndexTriangle( int vertex0, int vertex1, int vertex2 )
{
	vertex[0] = vertex0;
	vertex[1] = vertex1;
	vertex[2] = vertex2;
}

IndexTriangle::~IndexTriangle( void )
{
}

bool IndexTriangle::GetTriangle( Triangle& triangle, const std::vector< Vector >* vertexArray ) const
{
	for( int i = 0; i < 3; i++ )
	{
		int j = vertex[i];
		if( !BoundsCheck< Vector >( j, vertexArray ) )
			return false;
		triangle.vertex[i] = ( *vertexArray )[j];
	}
	return true;
}

bool IndexTriangle::GetTriangle( Triangle& triangle, const std::vector< Vertex >* vertexArray ) const
{
	for( int i = 0; i < 3; i++ )
	{
		int j = vertex[i];
		if( !BoundsCheck< Vertex >( j, vertexArray ) )
			return false;
		triangle.vertex[i] = ( *vertexArray )[j].position;
	}
	return true;
}

bool IndexTriangle::GetPlane( Plane& plane, const std::vector< Vertex >* vertexArray ) const
{
	Triangle triangle;
	if( !GetTriangle( triangle, vertexArray ) )
		return false;
	triangle.GetPlane( plane );
	return true;
}

bool IndexTriangle::CoincidentWith( const IndexTriangle& indexTriangle ) const
{
	return( SharedVertexCount( indexTriangle ) == 3 ? true : false );
}

bool IndexTriangle::AdjacentTo( const IndexTriangle& indexTriangle ) const
{
	return( SharedVertexCount( indexTriangle ) == 2 ? true : false );
}

int IndexTriangle::SharedVertexCount( const IndexTriangle& indexTriangle ) const
{
	int sharedVertexCount = 0;
	for( int i = 0; i < 3; i++ )
		if( indexTriangle.HasVertex( vertex[i] ) )
			sharedVertexCount++;
	return sharedVertexCount;
}

bool IndexTriangle::HasVertex( int index ) const
{
	for( int i = 0; i < 3; i++ )
		if( vertex[i] == index )
			return true;
	return false;
}

// IndexTriangle.cpp