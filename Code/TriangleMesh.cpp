// TriangleMesh.cpp

#include "TriangleMesh.h"
#include "Triangle.h"
#include "Plane.h"
#include "LinearTransform.h"
#include "AffineTransform.h"
#include "Renderer.h"
#include "AxisAlignedBox.h"

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

void TriangleMesh::Clone( const TriangleMesh& triangleMesh )
{
	Clear();

	for( int i = 0; i < ( signed )triangleMesh.vertexArray->size(); i++ )
		vertexArray->push_back( ( *triangleMesh.vertexArray )[i] );

	for( IndexTriangleList::const_iterator iter = triangleMesh.triangleList->cbegin(); iter != triangleMesh.triangleList->cend(); iter++ )
		triangleList->push_back( *iter );
}

bool TriangleMesh::GenerateBoundingBox( AxisAlignedBox& boundingBox ) const
{
	if( vertexArray->size() == 0 )
		return false;

	boundingBox.negCorner = ( *vertexArray )[0].position;
	boundingBox.posCorner = boundingBox.negCorner;

	for( int i = 1; i < ( signed )vertexArray->size(); i++ )
		boundingBox.GrowToIncludePoint( ( *vertexArray )[i].position );

	return true;
}

void TriangleMesh::GenerateTriangleList( TriangleList& triangleList, bool skipDegenerates /*= true*/ ) const
{
	for( IndexTriangleList::const_iterator iter = this->triangleList->cbegin(); iter != this->triangleList->cend(); iter++ )
	{
		const IndexTriangle& indexTriangle = *iter;
		Triangle triangle;
		indexTriangle.GetTriangle( triangle, vertexArray );
		bool isDegenerate = triangle.IsDegenerate();
		if( !isDegenerate || !skipDegenerates )
			triangleList.push_back( triangle );
	}
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
		AddOrRemoveTriangle( IndexTriangle( 0, 1, 3 ) );
		AddOrRemoveTriangle( IndexTriangle( 0, 3, 2 ) );
		AddOrRemoveTriangle( IndexTriangle( 0, 2, 1 ) );
		AddOrRemoveTriangle( IndexTriangle( 1, 2, 3 ) );
	}
	else
	{
		AddOrRemoveTriangle( IndexTriangle( 0, 3, 1 ) );
		AddOrRemoveTriangle( IndexTriangle( 0, 2, 3 ) );
		AddOrRemoveTriangle( IndexTriangle( 0, 1, 2 ) );
		AddOrRemoveTriangle( IndexTriangle( 3, 2, 1 ) );
	}

	while( pointCloud.size() > 0 )
	{
		Vector point = pointCloud.back();
		pointCloud.pop_back();

		Vertex vertex;
		vertex.position = point;
		vertexArray->push_back( vertex );

		int index = ( int )vertexArray->size() - 1;

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
					indexTriangle.GetPlane( plane, vertexArray );

					if( plane.GetSide( point ) == Plane::SIDE_FRONT )
					{
						AddOrRemoveTriangle( IndexTriangle( index, indexTriangle.vertex[0], indexTriangle.vertex[1] ) );
						AddOrRemoveTriangle( IndexTriangle( index, indexTriangle.vertex[1], indexTriangle.vertex[2] ) );
						AddOrRemoveTriangle( IndexTriangle( index, indexTriangle.vertex[2], indexTriangle.vertex[0] ) );
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

void TriangleMesh::CalculateCenter( Vector& center ) const
{
	center.Set( 0.0, 0.0, 0.0 );

	if( vertexArray->size() > 0 )
	{
		for( int i = 0; i < ( int )vertexArray->size(); i++ )
			center.Add( ( *vertexArray )[i].position );

		center.Scale( 1.0 / double( vertexArray->size() ) );
	}
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
		indexTriangle.GetPlane( plane, vertexArray );

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

void TriangleMesh::CalculateSphericalUVs( void )
{
	for( int i = 0; i < ( int )vertexArray->size(); i++ )
	{
		Vertex* vertex = &( *vertexArray )[i];

		Vector unitSpherePoint;
		vertex->position.GetNormalized( unitSpherePoint );

		double lattitudeAngle = acos( unitSpherePoint.y );
		double longitudeAngle = atan2( unitSpherePoint.z, unitSpherePoint.x );
		if( longitudeAngle < 0.0 )
			longitudeAngle += 2.0 * M_PI;

		vertex->texCoords.x = 1.0 - longitudeAngle / ( 2.0 * M_PI );
		vertex->texCoords.y = lattitudeAngle / M_PI;
	}
}

void TriangleMesh::SubdivideAllTriangles( double radius )
{
	IndexTriangleList::iterator iter = triangleList->begin();
	while( iter != triangleList->end() )
	{
		IndexTriangleList::iterator nextIter = iter;
		nextIter++;

		IndexTriangle indexTriangle = *iter;
		triangleList->erase( iter );

		Triangle triangle;
		indexTriangle.GetTriangle( triangle, vertexArray );

		Vector point[3];
		for( int i = 0; i < 3; i++ )
		{
			point[i].Lerp( triangle.vertex[i], triangle.vertex[ ( i + 1 ) % 3 ], 0.5 );
			point[i].Scale( radius / point[i].Length() );
		}

		int index[3];
		for( int i = 0; i < 3; i++ )
			index[i] = FindIndex( point[i], EPSILON, true );

		for( int i = 0; i < 3; i++ )
			triangleList->push_front( IndexTriangle( indexTriangle.vertex[i], index[i], index[ ( i + 2 ) % 3 ] ) );

		triangleList->push_front( IndexTriangle( index[0], index[1], index[2] ) );

		iter = nextIter;
	}
}

void TriangleMesh::Transform( const AffineTransform& affineTransform )
{
	affineTransform.Transform( *vertexArray );
}

bool TriangleMesh::SetVertexPosition( int index, const Vector& position )
{
	if( !ValidIndex( index ) )
		return false;

	( *vertexArray )[ index ].position = position;
	return true;
}

bool TriangleMesh::GetVertexPosition( int index, Vector& position ) const
{
	if( !ValidIndex( index ) )
		return false;

	position = ( *vertexArray )[ index ].position;
	return true;
}

bool TriangleMesh::SetVertex( int index, const Vertex& vertex )
{
	if( !ValidIndex( index ) )
		return false;

	( *vertexArray )[ index ] = vertex;
	return true;
}

bool TriangleMesh::GetVertex( int index, Vertex& vertex ) const
{
	if( !ValidIndex( index ) )
		return false;

	vertex = ( *vertexArray )[ index ];
	return true;
}

bool TriangleMesh::GetVertex( int index, const Vertex*& vertex ) const
{
	if( !ValidIndex( index ) )
		return false;

	vertex = &( *vertexArray )[ index ];
	return true;
}

bool TriangleMesh::ValidIndex( int index ) const
{
	if( index < 0 || index >= ( signed )vertexArray->size() )
		return false;
	return true;
}

int TriangleMesh::FindIndex( const Vector& position, double eps /*= EPSILON*/, bool addIfNotFound /*= false*/ ) const
{
	for( int i = 0; i < ( signed )vertexArray->size(); i++ )
	{
		const Vertex& vertex = ( *vertexArray )[i];
		if( vertex.position.IsEqualTo( position, eps ) )
			return i;
	}

	if( addIfNotFound )
	{
		Vertex vertex;
		vertex.position = position;
		vertexArray->push_back( vertex );
		return ( int )vertexArray->size() - 1;
	}

	return -1;
}

/*static*/ void TriangleMesh::SetEdgePair( uint64_t& edgePair, int index0, int index1 )
{
	if( index0 <= index1 )
		edgePair = uint64_t( index0 ) | ( uint64_t( index1 ) << 32 );
	else
		edgePair = uint64_t( index1 ) | ( uint64_t( index0 ) << 32 );
}

/*static*/ void TriangleMesh::GetEdgePair( uint64_t edgePair, int& index0, int& index1 )
{
	index0 = edgePair & 0xFFFFFFFF;
	index1 = edgePair >> 32;
}

void TriangleMesh::GenerateEdgeSet( EdgeSet& edgeSet ) const
{
	edgeSet.clear();

	IndexTriangleList::const_iterator iter = triangleList->cbegin();
	while( iter != triangleList->cend() )
	{
		const IndexTriangle& indexTriangle = *iter;
		
		for( int i = 0; i < 3; i++ )
		{
			int j = ( i + 1 ) % 3;

			uint64_t edgePair;
			SetEdgePair( edgePair, indexTriangle.vertex[i], indexTriangle.vertex[j] );

			edgeSet.insert( edgePair );
		}

		iter++;
	}
}

TriangleMesh::IndexTriangle::IndexTriangle( void )
{
	vertex[0] = 0;
	vertex[1] = 0;
	vertex[2] = 0;
}

TriangleMesh::IndexTriangle::IndexTriangle( int vertex0, int vertex1, int vertex2 )
{
	vertex[0] = vertex0;
	vertex[1] = vertex1;
	vertex[2] = vertex2;
}

TriangleMesh::IndexTriangle::~IndexTriangle( void )
{
}

void TriangleMesh::IndexTriangle::GetTriangle( Triangle& triangle, const std::vector< Vertex >* vertexArray ) const
{
	// TODO: Bounds check for safety?
	for( int i = 0; i < 3; i++ )
		triangle.vertex[i] = ( *vertexArray )[ vertex[i] ].position;
}

void TriangleMesh::IndexTriangle::GetPlane( Plane& plane, const std::vector< Vertex >* vertexArray ) const
{
	Triangle triangle;
	GetTriangle( triangle, vertexArray );
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