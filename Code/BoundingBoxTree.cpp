// BoundingBoxTree.cpp

#include "BoundingBoxTree.h"

using namespace _3DMath;

BoundingBoxTree::BoundingBoxTree( void )
{
	rootNode = nullptr;
}

/*virtual*/ BoundingBoxTree::~BoundingBoxTree( void )
{
	delete rootNode;
}

void BoundingBoxTree::GenerateNodes( const AxisAlignedBox& rootBox, int depth )
{
	rootNode = CreateNode( rootBox, depth );
}

BoundingBoxTree::Node* BoundingBoxTree::CreateNode( const AxisAlignedBox& boundingBox, int depth )
{
	Node* node = new Node();
	node->boundingBox = boundingBox;

	if( depth > 1 )
	{
		AxisAlignedBox boxA, boxB;
		boundingBox.SplitInTwo( boxA, boxB );

		node->node[0] = CreateNode( boxA, depth - 1 );
		node->node[1] = CreateNode( boxB, depth - 1 );
	}

	return node;
}

bool BoundingBoxTree::InsertTriangle( const Triangle& triangle )
{
	if( !rootNode )
		return false;

	return rootNode->InsertTriangle( triangle );
}

bool BoundingBoxTree::InsertTriangleList( const TriangleList& triangleList )
{
	for( TriangleList::const_iterator iter = triangleList.begin(); iter != triangleList.cend(); iter++ )
		if( !InsertTriangle( *iter ) )
			return false;

	return true;
}

bool BoundingBoxTree::FindIntersection( const LineSegment& lineSegment, const Triangle*& intersectedTriangle, Vector& intersectionPoint ) const
{
	if( !rootNode )
		return false;

	return rootNode->FindIntersection( lineSegment, intersectedTriangle, intersectionPoint );
}

BoundingBoxTree::Node::Node( void )
{
	node[0] = nullptr;
	node[1] = nullptr;
}

BoundingBoxTree::Node::~Node( void )
{
	delete node[0];
	delete node[1];
}

bool BoundingBoxTree::Node::InsertTriangle( const Triangle& triangle )
{
	if( !boundingBox.ContainsTriangle( triangle ) )
		return false;

	for( int i = 0; i < 2; i++ )
		if( node[i]->InsertTriangle( triangle ) )
			return true;

	triangleList.push_back( triangle );
	return true;
}

bool BoundingBoxTree::Node::FindIntersection( const LineSegment& lineSegment, const Triangle*& intersectedTriangle, Vector& intersectionPoint ) const
{
	if( !boundingBox.ContainsLineSegment( lineSegment ) )
		return false;

	for( TriangleList::const_iterator iter = triangleList.cbegin(); iter != triangleList.cend(); iter++ )
	{
		const Triangle& triangle = *iter;
		if( triangle.Intersect( lineSegment, intersectionPoint ) )
		{
			intersectedTriangle = &triangle;
			return true;
		}
	}

	for( int i = 0; i < 2; i++ )
		if( node[i]->FindIntersection( lineSegment, intersectedTriangle, intersectionPoint ) )
			return true;

	return false;
}

// BoundingBoxTree.cpp