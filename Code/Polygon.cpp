// Polygon.cpp

#include "Polygon.h"
#include "Triangle.h"
#include "IndexTriangle.h"

using namespace _3DMath;

Polygon::Polygon( void )
{
	vertexArray = new VectorArray();
}

/*virtual*/ Polygon::~Polygon( void )
{
	delete vertexArray;
}

bool Polygon::SplitAgainstSurface( const Surface* surface, PolygonList& polygonList ) const
{
	// TODO: Write this.  Think of this like the paint-can algorithm.
	return false;
}

void Polygon::Tessellate( IndexTriangleList& indexTriangleList ) const
{
	indexTriangleList.clear();

	VectorArray pointArray;
	for( int i = 0; i < ( signed )vertexArray->size(); i++ )
		pointArray.push_back( ( *vertexArray )[i] );

	while( pointArray.size() > 2 )
	{
		for( int i = 0; i < ( signed )vertexArray->size(); i++ )
		{
			IndexTriangle indexTriangle( i, ( i + 1 ) % vertexArray->size(), ( i + 2 ) % vertexArray->size() );

			Triangle triangle;
			indexTriangle.GetTriangle( triangle, &pointArray );

			int j;
			for( j = 0; j < ( signed )vertexArray->size(); j++ )
			{
				if( j == i )
					continue;

				if( triangle.ProperlyContainsPoint( ( *vertexArray )[j] ) )
					break;
			}

			if( j < ( signed )vertexArray->size() )
				continue;

			indexTriangleList.push_back( indexTriangle );
			pointArray.erase( pointArray.begin() + ( i + 1 ) % vertexArray->size() );
			break;
		}
	}
}

// Polygon.cpp