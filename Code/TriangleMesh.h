// TriangleMesh.h

#pragma once

#include "Defines.h"
#include "Vector.h"
#include "Renderer.h"
#include "Triangle.h"

namespace _3DMath
{
	class TriangleMesh;
	class Plane;
	class Triangle;
	class AffineTransform;
	class AxisAlignedBox;
	class Vertex;
}

class _3DMATH_API _3DMath::TriangleMesh
{
public:

	TriangleMesh( void );
	virtual ~TriangleMesh( void );

	class IndexTriangle
	{
	public:

		IndexTriangle( int vertex0, int vertex1, int vertex2, TriangleMesh* mesh );
		~IndexTriangle( void );

		void GetTriangle( Triangle& triangle ) const;
		void GetPlane( Plane& plane ) const;
		bool HasVertex( int index ) const;
		bool CoincidentWith( const IndexTriangle& indexTriangle ) const;

		int vertex[3];
		TriangleMesh* mesh;
	};

	typedef std::list< IndexTriangle > IndexTriangleList;

	void Clear( void );
	bool FindConvexHull( void );
	void AddOrRemoveTriangle( IndexTriangle& givenIndexTriangle );
	void CalculateNormals( void );
	void CalculateSphericalUVs( void );
	void SubdivideAllTriangles( double radius );
	void Transform( const AffineTransform& affineTransform );
	bool GenerateBoundingBox( AxisAlignedBox& boundingBox ) const;
	void GenerateTriangleList( TriangleList& triangleList ) const;
	//void GenerateMeshUsingString( const std::string& string, double fontSize );

	typedef std::set< uint64_t > EdgeSet;

	static void SetEdgePair( uint64_t& edgePair, int index0, int index1 );
	static void GetEdgePair( uint64_t edgePair, int& index0, int& index1 );

	void GenerateEdgeSet( EdgeSet& edgeSet ) const;

	int FindIndex( const Vector& position, double eps = EPSILON, bool addIfNotFound = false ) const;

	bool SetVertexPosition( int index, const Vector& position );
	bool GetVertexPosition( int index, Vector& position ) const;

	bool SetVertex( int index, const Vertex& vertex );
	bool GetVertex( int index, Vertex& vertex ) const;
	bool GetVertex( int index, const Vertex*& vertex ) const;

	bool ValidIndex( int index ) const;

	// TODO: May want to write a tri-stripper one day.

	std::vector< Vertex >* vertexArray;
	IndexTriangleList* triangleList;
};

// TriangleMesh.h
