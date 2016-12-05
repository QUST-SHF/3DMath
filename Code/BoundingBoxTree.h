// BoundingBoxTree.h

#pragma once

#include "Defines.h"
#include "AxisAlignedBox.h"
#include "Triangle.h"

namespace _3DMath
{
	class BoundingBoxTree;
	class LineSegment;
}

class _3DMATH_API _3DMath::BoundingBoxTree
{
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
		~Node( void );

		bool InsertTriangle( const Triangle& triangle );

		bool FindIntersection( const LineSegment& lineSegment, const Triangle*& intersectedTriangle, Vector& intersectionPoint ) const;

		AxisAlignedBox boundingBox;
		TriangleList triangleList;

		Node* node[2];
	};

private:

	Node* CreateNode( const AxisAlignedBox& boundingBox, int depth );

	Node* rootNode;
};

// BoundingBoxTree.h