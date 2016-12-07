// BoundingBoxTree.h

#pragma once

#include "Defines.h"
#include "AxisAlignedBox.h"
#include "Triangle.h"
#include "Plane.h"

namespace _3DMath
{
	class BoundingBoxTree;
	class LineSegment;
	class Renderer;
}

class _3DMATH_API _3DMath::BoundingBoxTree
{
	friend Renderer;

public:

	BoundingBoxTree( void );
	virtual ~BoundingBoxTree( void );

	void GenerateNodes( const AxisAlignedBox& rootBox, int depth );

	bool InsertTriangle( const Triangle& triangle );
	bool InsertTriangleList( const TriangleList& triangleList );

	bool FindIntersection( const LineSegment& lineSegment, const Triangle*& intersectedTriangle, Vector& intersectionPoint ) const;

	class _3DMATH_API Node
	{
	public:

		Node( void );
		virtual ~Node( void );

		virtual bool InsertTriangle( const Triangle& triangle ) = 0;
		virtual bool FindIntersection( const LineSegment& lineSegment, const Triangle*& intersectedTriangle, Vector& intersectionPoint ) const = 0;

		AxisAlignedBox boundingBox;
	};

	class _3DMATH_API BranchNode : public Node
	{
	public:

		BranchNode( void );
		virtual ~BranchNode( void );

		virtual bool InsertTriangle( const Triangle& triangle ) override;
		virtual bool FindIntersection( const LineSegment& lineSegment, const Triangle*& intersectedTriangle, Vector& intersectionPoint ) const override;

		Plane plane;
		Node* frontNode;
		Node* backNode;
	};

	class _3DMATH_API LeafNode : public Node
	{
	public:

		LeafNode( void );
		virtual ~LeafNode( void );

		virtual bool InsertTriangle( const Triangle& triangle ) override;
		virtual bool FindIntersection( const LineSegment& lineSegment, const Triangle*& intersectedTriangle, Vector& intersectionPoint ) const override;

		TriangleList* triangleList;
	};

private:

	Node* CreateNode( const AxisAlignedBox& boundingBox, int depth );

	Node* rootNode;
};

// BoundingBoxTree.h