// TriangleMesh.h

#pragma once

#include "Defines.h"
#include "Vector.h"
#include "Renderer.h"
#include "Triangle.h"
#include "IndexTriangle.h"
#include "Polygon.h"

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

	void Clear( void );
	void Clone( const TriangleMesh& triangleMesh );
	bool FindConvexHull( void );
	void AddOrRemoveTriangle( const IndexTriangle& givenIndexTriangle );
	void CalculateNormals( void );
	void CalculateSphericalUVs( void );
	void SubdivideAllTriangles( double radius );	// TODO: A better version of this could smooth any ridged mesh.  This one only knows convex meshes at origin.
	void Transform( const AffineTransform& affineTransform );
	bool GenerateBoundingBox( AxisAlignedBox& boundingBox ) const;
	void GenerateTriangleList( TriangleList& triangleList, bool skipDegenerates = true ) const;
	//void GenerateStringMesh( const std::string& string, double fontSize, void* font );
	void Compress( void );
	//void GenerateFromSurface( const Surface* surface, const AxisAlignedBox& boundingBox );	// TODO: Use a gift-wrapping-type algorithm?  Utilize tangent spaces.
	void AddSymmetricVertices( const Vector& vector );
	bool GeneratePolygonFaceList( PolygonList& polygonFaceList, double eps = EPSILON ) const;

	typedef std::set< uint64_t > EdgeSet;

	static void SetEdgePair( uint64_t& edgePair, int index0, int index1 );
	static void GetEdgePair( uint64_t edgePair, int& index0, int& index1 );

	void GenerateEdgeSet( EdgeSet& edgeSet ) const;

	int FindIndex( const Vector& position, double eps = EPSILON, bool addIfNotFound = false ) const;

	void CalculateCenter( Vector& center ) const;

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
