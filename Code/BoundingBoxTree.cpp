// BoundingBoxTree.cpp

#include "BoundingBoxTree.h"

using namespace _3DMath;

//-----------------------------------------------------------------------------------------------------------
//                                           BoundingBoxTree
//-----------------------------------------------------------------------------------------------------------

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
	if( rootNode )
		delete rootNode;

	rootNode = CreateNode( rootBox, depth );
}

BoundingBoxTree::Node* BoundingBoxTree::CreateNode( const AxisAlignedBox& boundingBox, int depth )
{
	Node* node = nullptr;
	
	if( depth == 1 )
		node = new LeafNode();
	else
	{
		BranchNode* branchNode = new BranchNode();

		node = branchNode;
	
		AxisAlignedBox boxA, boxB;
		boundingBox.SplitInTwo( boxA, boxB, &branchNode->plane );

		branchNode->backNode = CreateNode( boxA, depth - 1 );
		branchNode->frontNode = CreateNode( boxB, depth - 1 );
	}

	node->boundingBox = boundingBox;

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

//-----------------------------------------------------------------------------------------------------------
//                                                   Node
//-----------------------------------------------------------------------------------------------------------

BoundingBoxTree::Node::Node( void )
{
}

BoundingBoxTree::Node::~Node( void )
{
}

//-----------------------------------------------------------------------------------------------------------
//                                                   BranchNode
//-----------------------------------------------------------------------------------------------------------

BoundingBoxTree::BranchNode::BranchNode( void )
{
	frontNode = nullptr;
	backNode = nullptr;
}

BoundingBoxTree::BranchNode::~BranchNode( void )
{
	delete frontNode;
	delete backNode;
}

bool BoundingBoxTree::BranchNode::InsertTriangle( const Triangle& triangle )
{
	if( !boundingBox.ContainsTriangle( triangle ) )
		return false;

	if( !backNode->InsertTriangle( triangle ) && !frontNode->InsertTriangle( triangle ) )
	{
		TriangleList frontList, backList;
		if( !plane.SplitTriangle( triangle, frontList, backList ) )
			return false;

		for( TriangleList::iterator iter = backList.begin(); iter != backList.end(); iter++ )
			if( !backNode->InsertTriangle( *iter ) )
				return false;

		for( TriangleList::iterator iter = frontList.begin(); iter != frontList.end(); iter++ )
			if( !frontNode->InsertTriangle( *iter ) )
				return false;
	}

	return true;
}

bool BoundingBoxTree::BranchNode::FindIntersection( const LineSegment& lineSegment, const Triangle*& intersectedTriangle, Vector& intersectionPoint ) const
{
	if( !boundingBox.ContainsLineSegment( lineSegment ) )
		return false;

	if( backNode->FindIntersection( lineSegment, intersectedTriangle, intersectionPoint ) )
		return true;

	if( frontNode->FindIntersection( lineSegment, intersectedTriangle, intersectionPoint ) )
		return true;

	return false;
}

//-----------------------------------------------------------------------------------------------------------
//                                                    LeafNode
//-----------------------------------------------------------------------------------------------------------

BoundingBoxTree::LeafNode::LeafNode( void )
{
	triangleList = new TriangleList();
}

BoundingBoxTree::LeafNode::~LeafNode( void )
{
	delete triangleList;
}

bool BoundingBoxTree::LeafNode::InsertTriangle( const Triangle& triangle )
{
	if( boundingBox.ContainsTriangle( triangle ) )
	{
		triangleList->push_back( triangle );
		return true;
	}

	return false;
}

bool BoundingBoxTree::LeafNode::FindIntersection( const LineSegment& lineSegment, const Triangle*& intersectedTriangle, Vector& intersectionPoint ) const
{
	// There might be more than one triangle in our list intersecting with the given line-segment, but let's start with this.

	for( TriangleList::const_iterator iter = triangleList->cbegin(); iter != triangleList->cend(); iter++ )
	{
		const Triangle& triangle = *iter;
		if( triangle.Intersect( lineSegment, intersectionPoint ) )
		{
			intersectedTriangle = &triangle;
			return true;
		}
	}

	return true;
}

// BoundingBoxTree.cpp