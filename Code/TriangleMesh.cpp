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

	VertexArray* newVertexArray = nullptr;

	bool tetrahedronFound = false;

	int i0, i1, i2, i3;

	for( i0 = 0; i0 < ( signed )vertexArray->size(); i0++ )
	{
		for( i1 = 0; i1 < ( signed )vertexArray->size(); i1++ )
		{
			if( i1 == i0 )
				continue;

			for( i2 = 0; i2 < ( signed )vertexArray->size(); i2++ )
			{
				if( i2 == i0 || i2 == i1 )
					continue;

				for( i3 = 0; i3 < ( signed )vertexArray->size(); i3++ )
				{
					if( i3 == i0 || i3 == i1 || i3 == i2 )
						continue;

					LinearTransform linearTransform;

					linearTransform.xAxis.Subtract( ( *vertexArray )[i1].position, ( *vertexArray )[i0].position );
					linearTransform.yAxis.Subtract( ( *vertexArray )[i2].position, ( *vertexArray )[i0].position );
					linearTransform.zAxis.Subtract( ( *vertexArray )[i3].position, ( *vertexArray )[i0].position );

					// In the strictest sense, it need only be greater than zero,
					// but I want a tetrahedron that is no where near degenerate.
					double det = linearTransform.Determinant();
					if( det > EPSILON )
					{
						newVertexArray = new VertexArray();

						newVertexArray->push_back( ( *vertexArray )[i0] );
						newVertexArray->push_back( ( *vertexArray )[i1] );
						newVertexArray->push_back( ( *vertexArray )[i2] );
						newVertexArray->push_back( ( *vertexArray )[i3] );

						AddOrRemoveTriangle( IndexTriangle( 0, 1, 3 ) );
						AddOrRemoveTriangle( IndexTriangle( 0, 3, 2 ) );
						AddOrRemoveTriangle( IndexTriangle( 0, 2, 1 ) );
						AddOrRemoveTriangle( IndexTriangle( 1, 2, 3 ) );

						tetrahedronFound = true;
						break;
					}
				}

				if( tetrahedronFound )
					break;
			}

			if( tetrahedronFound )
				break;
		}

		if( tetrahedronFound )
			break;
	}

	if( !tetrahedronFound )
		return false;

	VertexArray pointCloud;

	for( int i = 0; i < ( signed )vertexArray->size(); i++ )
	{
		if( i == i0 || i == i1 || i == i2 || i == i3 )
			continue;

		pointCloud.push_back( ( *vertexArray )[i] );
	}

	delete vertexArray;
	vertexArray = nullptr;

	while( pointCloud.size() > 0 )
	{
		Vertex point = pointCloud.back();
		pointCloud.pop_back();

		newVertexArray->push_back( point );

		int index = ( int )newVertexArray->size() - 1;

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
					indexTriangle.GetPlane( plane, newVertexArray );

					if( plane.GetSide( point.position ) == Plane::SIDE_FRONT )
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

	vertexArray = newVertexArray;

	return true;
}

void TriangleMesh::AddOrRemoveTriangle( const IndexTriangle& givenIndexTriangle )
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

void TriangleMesh::AddSymmetricVertices( const Vector& vector )
{
	for( int i = 0; i < 8; i++ )
	{
		if( ( ( i & 1 ) && vector.x == 0.0 ) ||
			( ( i & 2 ) && vector.y == 0.0 ) ||
			( ( i & 4 ) && vector.z == 0.0 ) )
		{
			continue;
		}

		Vertex vertex;
		vertex.position = vector;

		vertex.position.x = ( i & 1 ) ? -vector.x : vector.x;
		vertex.position.y = ( i & 2 ) ? -vector.y : vector.y;
		vertex.position.z = ( i & 4 ) ? -vector.z : vector.z;

		vertexArray->push_back( vertex );
	}
}

void TriangleMesh::Compress( void )
{
	VertexArray* compressedVertexArray = new VertexArray();

	for( int i = 0; i < ( signed )vertexArray->size(); i++ )
	{
		const Vertex& vertex = ( *vertexArray )[i];

		int j;
		for( j = 0; j < ( signed )compressedVertexArray->size(); j++ )
		{
			const Vertex& compressedVertex = ( *compressedVertexArray )[j];
			if( compressedVertex.position.IsEqualTo( vertex.position ) )
				break;
		}

		if( j < ( signed )compressedVertexArray->size() )
			continue;

		compressedVertexArray->push_back( vertex );

		for( IndexTriangleList::iterator iter = triangleList->begin(); iter != triangleList->end(); iter++ )
		{
			IndexTriangle& indexTriangle = *iter;
			for( int j = 0; j < 3; j++ )
				if( ( *vertexArray )[ indexTriangle.vertex[j] ].position.IsEqualTo( vertex.position ) )
					indexTriangle.vertex[j] = compressedVertexArray->size() - 1;
		}
	}

	delete vertexArray;
	vertexArray = compressedVertexArray;
}

bool TriangleMesh::GeneratePolygonFaceList( PolygonList& polygonFaceList, double eps /*= EPSILON*/ ) const
{
	// Our algorithm's correctness depends upon the mesh being fully compressed.
	const_cast< TriangleMesh* >( this )->Compress();

	IndexTriangleList triangleQueue;
	for( IndexTriangleList::const_iterator iter = triangleList->cbegin(); iter != triangleList->cend(); iter++ )
		triangleQueue.push_back( *iter );

	while( triangleQueue.size() > 0 )
	{
		IndexTriangleList::iterator iter = triangleQueue.begin();
		IndexTriangle indexTriangle = *iter;
		triangleQueue.erase( iter );

		Plane plane;
		if( !indexTriangle.GetPlane( plane, vertexArray ) )
			return false;

		IndexTriangleList coplanarList, breadthFirstSearchQueue;
		breadthFirstSearchQueue.push_back( indexTriangle );

		while( breadthFirstSearchQueue.size() > 0 )
		{
			iter = breadthFirstSearchQueue.begin();
			IndexTriangle coplanarTriangle = *iter;
			breadthFirstSearchQueue.erase( iter );

			coplanarList.push_back( coplanarTriangle );

			iter = triangleQueue.begin();
			while( iter != triangleQueue.end() )
			{
				IndexTriangleList::iterator nextIter = iter;
				nextIter++;

				IndexTriangle adjacentTriangle = *iter;
				if( adjacentTriangle.AdjacentTo( coplanarTriangle ) )
				{
					Plane otherPlane;
					if( !adjacentTriangle.GetPlane( otherPlane, vertexArray ) )
						return false;

					double dot = otherPlane.normal.Dot( plane.normal );
					if( fabs( dot - 1.0 ) < eps )
					{
						triangleQueue.erase( iter );
						breadthFirstSearchQueue.push_back( adjacentTriangle );
					}
				}

				iter = nextIter;			
			}
		}

		iter = coplanarList.begin();
		indexTriangle = *iter;
		coplanarList.erase( iter );

		std::vector< int > loopArray;
		for( int i = 0; i < 3; i++ )
			loopArray.push_back( indexTriangle.vertex[i] );

		while( coplanarList.size() > 0 )
		{
			unsigned int listSize = coplanarList.size();

			for( iter = coplanarList.begin(); iter != coplanarList.end(); iter++ )
			{
				indexTriangle = *iter;

				struct EdgeShare
				{
					int i, j;
					int s, t;
				};

				std::list< EdgeShare > edgeShareList;
				int vertexShareCount = 0;

				int i, j, s, t;

				for( i = 0; i < ( signed )loopArray.size(); i++ )
				{
					j = ( i + 1 ) % loopArray.size();
					
					for( s = 0; s < 3; s++ )
					{
						t = ( s + 1 ) % 3;

						if( loopArray[i] == indexTriangle.vertex[t] &&
							loopArray[j] == indexTriangle.vertex[s] )
						{
							EdgeShare edgeShare;
							edgeShare.i = i;
							edgeShare.j = j;
							edgeShare.s = s;
							edgeShare.t = t;
							edgeShareList.push_back( edgeShare );
						}

						if( loopArray[i] == indexTriangle.vertex[s] )
							vertexShareCount++;
					}
				}

				if( edgeShareList.size() == 1 && vertexShareCount < 3 )
				{
					EdgeShare edgeShare = edgeShareList.front();
					std::vector< int >::iterator loopIter( loopArray.begin() + edgeShare.j );
					loopArray.insert( loopIter, indexTriangle.vertex[ ( edgeShare.s + 2 ) % 3 ] );
					coplanarList.erase( iter );
					break;
				}
				else if( edgeShareList.size() == 2 )
				{
					EdgeShare edgeShareA = edgeShareList.front();
					EdgeShare edgeShareB = edgeShareList.back();
					if( edgeShareA.j == edgeShareB.i )
					{
						std::vector< int >::iterator loopIter( loopArray.begin() + edgeShareA.j );
						loopArray.erase( loopIter );
						coplanarList.erase( iter );
						break;
					}
					else if( edgeShareB.j == edgeShareA.i )
					{
						std::vector< int >::iterator loopIter( loopArray.begin() + edgeShareB.j );
						loopArray.erase( loopIter );
						coplanarList.erase( iter );
						break;
					}
				}

				iter++;
			}

			if( listSize == coplanarList.size() )
				return false;
		}

		Polygon* polygon = new Polygon();
		polygonFaceList.push_back( polygon );

		for( int i = 0; i < ( signed )loopArray.size(); i++ )
			polygon->vertexArray->push_back( ( *vertexArray )[ loopArray[i] ].position );
	}

	return true;
}

// TriangleMesh.cpp