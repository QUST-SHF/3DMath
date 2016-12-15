// BspTree.h

#pragma once

#include "Defines.h"
#include "Plane.h"
#include "Triangle.h"
#include "TriangleMesh.h"

namespace _3DMath
{
	class BspTree;
	class Triangle;
	class TriangleMesh;
	class Renderer;
}

class _3DMATH_API _3DMath::BspTree
{
public:

	BspTree( void );
	virtual ~BspTree( void );

	bool Generate( const TriangleMesh& triangleMesh );
	void Clear( void );

	enum RenderMode
	{
		RENDER_BACK_TO_FRONT,
		RENDER_FRONT_TO_BACK,
	};

	void Render( Renderer& renderer, RenderMode renderMode, const Vector& eye ) const;

	class _3DMATH_API Node
	{
	public:

		Node( void );
		virtual ~Node( void );

		Plane partitioningPlane;
		Node* frontNode;
		Node* backNode;
		TriangleMesh::IndexTriangleList* triangleList;

		void Generate( TriangleMesh::IndexTriangleList& givenTriangleList, std::vector< Vertex >& vertexArray );
		void Render( Renderer& renderer, RenderMode renderMode, const Vector& eye, const BspTree* bspTree ) const;

		TriangleMesh::IndexTriangleList::iterator ChooseBestPartitioningTriangle( TriangleMesh::IndexTriangleList& givenTriangleList, std::vector< Vertex >& vertexArray );

		void AddSubTriangles( TriangleMesh::IndexTriangleList& triangleList, std::vector< Vertex >& vertexArray, const TriangleMesh::IndexTriangle& indexTriangle, const TriangleList& subTriangleList );
	};

	virtual bool FrontSpaceVisible( const Node* node ) const;
	virtual bool BackSpaceVisible( const Node* node ) const;

private:

	Node* rootNode;

	std::vector< Vertex >* vertexArray;
};

// BspTree.h
