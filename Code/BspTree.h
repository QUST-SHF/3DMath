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
	class AffineTransform;
	class IndexTriangle;
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

	void Render( Renderer& renderer, RenderMode renderMode, const Vector& eye, const AffineTransform* transform = nullptr, int vertexFlags = 0 ) const;
	void Transform( const AffineTransform& transform );

	class _3DMATH_API Node
	{
	public:

		Node( void );
		virtual ~Node( void );

		Plane partitioningPlane;
		Node* frontNode;
		Node* backNode;
		IndexTriangleList* triangleList;

		void Generate( IndexTriangleList& givenTriangleList, std::vector< Vertex >& vertexArray );
		void Render( Renderer& renderer, RenderMode renderMode, const Vector& eye, const BspTree* bspTree, const AffineTransform& transform, const LinearTransform& normalTransform, int vertexFlags ) const;
		void Transform( const AffineTransform& transform );

		IndexTriangleList::iterator ChooseBestPartitioningTriangle( IndexTriangleList& givenTriangleList, std::vector< Vertex >& vertexArray );

		void AddSubTriangles( IndexTriangleList& triangleList, std::vector< Vertex >& vertexArray, const IndexTriangle& indexTriangle, const TriangleList& subTriangleList );
	};

	virtual bool FrontSpaceVisible( const Node* node ) const;
	virtual bool BackSpaceVisible( const Node* node ) const;

private:

	Node* rootNode;

	std::vector< Vertex >* vertexArray;
};

// BspTree.h
