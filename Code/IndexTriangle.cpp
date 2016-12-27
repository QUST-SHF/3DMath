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

void IndexTriangle::GetTriangle( Triangle& triangle, const std::vector< Vector >* vertexArray ) const
{
	// TODO: Bounds check for safety?
	for( int i = 0; i < 3; i++ )
		triangle.vertex[i] = ( *vertexArray )[ vertex[i] ];
}

void IndexTriangle::GetTriangle( Triangle& triangle, const std::vector< Vertex >* vertexArray ) const
{
	// TODO: Bounds check for safety?
	for( int i = 0; i < 3; i++ )
		triangle.vertex[i] = ( *vertexArray )[ vertex[i] ].position;
}

void IndexTriangle::GetPlane( Plane& plane, const std::vector< Vertex >* vertexArray ) const
{
	Triangle triangle;
	GetTriangle( triangle, vertexArray );
	triangle.GetPlane( plane );
}

bool IndexTriangle::CoincidentWith( const IndexTriangle& indexTriangle ) const
{
	for( int i = 0; i < 3; i++ )
		if( !indexTriangle.HasVertex( vertex[i] ) )
			return false;
	return true;
}

bool IndexTriangle::HasVertex( int index ) const
{
	for( int i = 0; i < 3; i++ )
		if( vertex[i] == index )
			return true;
	return false;
}

// IndexTriangle.cpp