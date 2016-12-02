// TriangleMesh.h

#pragma once

#include "Defines.h"
#include "Vector.h"
#include "Renderer.h"

namespace _3DMath
{
	class TriangleMesh;
	class Plane;
	class Triangle;
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

	bool SetVertexPosition( int index, const Vector& position );
	bool GetVertexPosition( int index, Vector& position ) const;

	bool SetVertex( int index, const Vertex& vertex );
	bool GetVertex( int index, Vertex& vertex ) const;

	bool ValidIndex( int index ) const;

	typedef std::vector< Vertex > VertexArray;

	VertexArray* vertexArray;
	IndexTriangleList* triangleList;
};

// TriangleMesh.h
