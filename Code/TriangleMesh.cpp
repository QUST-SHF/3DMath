// TriangleMesh.cpp

#include "TriangleMesh.h"
#include "Triangle.h"
#include "Plane.h"
#include "LinearTransform.h"

using namespace _3DMath;

TriangleMesh::TriangleMesh( void )
{
	vertexArray = new VertexArray();
	triangleList = new IndexTriangleList();
}

/*virtual*/ TriangleMesh::~TriangleMesh( void )
{
	delete vertexArray;
	delete triangleList;
}

void TriangleMesh::Clear( void )
{
	vertexArray->clear();
	triangleList->clear();
}

bool TriangleMesh::FindConvexHull( void )
{
	if( vertexArray->size() < 4 )
		return false;

	triangleList->clear();

	VectorArray pointCloud;
	while( vertexArray->size() > 4 )
	{
		pointCloud.push_back( vertexArray->back().position );
		vertexArray->pop_back();
	}

	LinearTransform linearTransform;

	linearTransform.xAxis.Subtract( ( *vertexArray )[1].position, ( *vertexArray )[0].position );
	linearTransform.yAxis.Subtract( ( *vertexArray )[2].position, ( *vertexArray )[0].position );
	linearTransform.zAxis.Subtract( ( *vertexArray )[3].position, ( *vertexArray )[0].position );

	if( linearTransform.Determinant() > 0.0 )
	{
		AddOrRemoveTriangle( IndexTriangle( 0, 1, 3, this ) );
		AddOrRemoveTriangle( IndexTriangle( 0, 3, 2, this ) );
		AddOrRemoveTriangle( IndexTriangle( 0, 2, 1, this ) );
		AddOrRemoveTriangle( IndexTriangle( 1, 2, 3, this ) );
	}
	else
	{
		AddOrRemoveTriangle( IndexTriangle( 0, 3, 1, this ) );
		AddOrRemoveTriangle( IndexTriangle( 0, 2, 3, this ) );
		AddOrRemoveTriangle( IndexTriangle( 0, 1, 2, this ) );
		AddOrRemoveTriangle( IndexTriangle( 3, 2, 1, this ) );
	}

	while( pointCloud.size() > 0 )
	{
		Vector point = pointCloud.back();
		pointCloud.pop_back();

		Vertex vertex;
		vertex.position = point;
		vertexArray->push_back( vertex );

		int index = vertexArray->size() - 1;

		bool keepGoing = true;
		while( keepGoing )
		{
			keepGoing = false;

			for( IndexTriangleList::iterator iter = triangleList->begin(); iter != triangleList->end(); iter++ )
			{
				IndexTriangle& indexTriangle = *iter;

				if( !indexTriangle.HasVertex( index ) )
				{
					Plane plane;
					indexTriangle.GetPlane( plane );

					if( plane.GetSide( point ) == Plane::SIDE_FRONT )
					{
						AddOrRemoveTriangle( IndexTriangle( index, indexTriangle.vertex[0], indexTriangle.vertex[1], this ) );
						AddOrRemoveTriangle( IndexTriangle( index, indexTriangle.vertex[1], indexTriangle.vertex[2], this ) );
						AddOrRemoveTriangle( IndexTriangle( index, indexTriangle.vertex[2], indexTriangle.vertex[0], this ) );
						AddOrRemoveTriangle( indexTriangle );

						keepGoing = true;
						break;
					}
				}
			}
		}
	}

	return true;
}

void TriangleMesh::AddOrRemoveTriangle( IndexTriangle& givenIndexTriangle )
{
	for( IndexTriangleList::iterator iter = triangleList->begin(); iter != triangleList->end(); iter++ )
	{
		IndexTriangle& indexTriangle = *iter;
		if( givenIndexTriangle.CoincidentWith( indexTriangle ) )
		{
			triangleList->erase( iter );
			return;
		}
	}

	triangleList->push_back( givenIndexTriangle );
}

void TriangleMesh::CalculateNormals( void )
{
	for( int i = 0; i < ( int )vertexArray->size(); i++ )
	{
		Vertex* vertex = &( *vertexArray )[i];
		vertex->normal.Set( 0.0, 0.0, 0.0 );
	}

	for( IndexTriangleList::iterator iter = triangleList->begin(); iter != triangleList->end(); iter++ )
	{
		IndexTriangle& indexTriangle = *iter;

		Plane plane;
		indexTriangle.GetPlane( plane );

		for( int i = 0; i < 3; i++ )
		{
			Vertex* vertex = &( *vertexArray )[ indexTriangle.vertex[i] ];
			vertex->normal.Add( plane.normal );
		}
	}

	for( int i = 0; i < ( int )vertexArray->size(); i++ )
	{
		Vertex* vertex = &( *vertexArray )[i];
		vertex->normal.Normalize();
	}
}

bool TriangleMesh::SetVertexPosition( int index, const Vector& position )
{
	if( index < 0 || index >= ( signed )vertexArray->size() )
		return false;

	( *vertexArray )[ index ].position = position;
	return true;
}

bool TriangleMesh::GetVertexPosition( int index, Vector& position ) const
{
	if( index < 0 || index >= ( signed )vertexArray->size() )
		return false;

	position = ( *vertexArray )[ index ].position;
	return true;
}

TriangleMesh::IndexTriangle::IndexTriangle( int vertex0, int vertex1, int vertex2, TriangleMesh* mesh )
{
	this->mesh = mesh;

	vertex[0] = vertex0;
	vertex[1] = vertex1;
	vertex[2] = vertex2;
}

TriangleMesh::IndexTriangle::~IndexTriangle( void )
{
}

void TriangleMesh::IndexTriangle::GetTriangle( Triangle& triangle ) const
{
	for( int i = 0; i < 3; i++ )
		triangle.vertex[i] = ( *mesh->vertexArray )[ vertex[i] ].position;
}

void TriangleMesh::IndexTriangle::GetPlane( Plane& plane ) const
{
	Triangle triangle;
	GetTriangle( triangle );
	triangle.GetPlane( plane );
}

bool TriangleMesh::IndexTriangle::CoincidentWith( const IndexTriangle& indexTriangle ) const
{
	for( int i = 0; i < 3; i++ )
		if( !indexTriangle.HasVertex( vertex[i] ) )
			return false;
	return true;
}

bool TriangleMesh::IndexTriangle::HasVertex( int index ) const
{
	for( int i = 0; i < 3; i++ )
		if( vertex[i] == index )
			return true;
	return false;
}

// TriangleMesh.cpp