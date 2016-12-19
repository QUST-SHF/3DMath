// BoundingBoxTree.cpp

#include "BoundingBoxTree.h"
#include "LineSegment.h"

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

bool BoundingBoxTree::InsertTriangleList( const TriangleList& triangleList, const Vector* normalFilter /*= nullptr*/, double angleFilter /*= 0.0*/ )
{
	for( TriangleList::const_iterator iter = triangleList.begin(); iter != triangleList.cend(); iter++ )
	{
		const Triangle& triangle = *iter;

		if( normalFilter )
		{
			Vector normal;
			triangle.GetNormal( normal );

			double angle = normal.AngleBetween( *normalFilter );
			if( angle >= angleFilter )
				continue;
		}

		if( !InsertTriangle( triangle ) )
			return false;
	}

	return true;
}

bool BoundingBoxTree::FindIntersection( const LineSegment& lineSegment, const Triangle*& intersectedTriangle, Vector& intersectionPoint ) const
{
	if( !rootNode )
		return false;

	return rootNode->FindIntersection( lineSegment, intersectedTriangle, intersectionPoint );
}

bool BoundingBoxTree::FindNearestTriangle( const Vector& point, const Triangle*& nearestTriangle, double maxDistance ) const
{
	if( !rootNode )
		return false;

	return rootNode->FindNearestTriangle( point, nearestTriangle, maxDistance );
}

//-----------------------------------------------------------------------------------------------------------
//                                                   Node
//-----------------------------------------------------------------------------------------------------------

BoundingBoxTree::Node::Node( void )
{
}

/*virtual*/ BoundingBoxTree::Node::~Node( void )
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

/*virtual*/ BoundingBoxTree::BranchNode::~BranchNode( void )
{
	delete frontNode;
	delete backNode;
}

/*virtual*/ bool BoundingBoxTree::BranchNode::InsertTriangle( const Triangle& triangle )
{
	if( !boundingBox.ContainsTriangle( triangle ) )
		return false;

	if( !backNode->InsertTriangle( triangle ) && !frontNode->InsertTriangle( triangle ) )
	{
		// At the expense of duplicating triangles in the tree, we might consider inserting
		// down multiple branches.  Forgoing the split, we may actually net fewer triangles
		// per leaf node, and therefore fewer checks.

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

/*virtual*/ bool BoundingBoxTree::BranchNode::FindIntersection( const LineSegment& lineSegment, const Triangle*& intersectedTriangle, Vector& intersectionPoint ) const
{
	if( boundingBox.IntersectsWithLineSegment( lineSegment ) )
	{
		if( backNode->FindIntersection( lineSegment, intersectedTriangle, intersectionPoint ) )
			return true;

		if( frontNode->FindIntersection( lineSegment, intersectedTriangle, intersectionPoint ) )
			return true;
	}

	return false;
}

/*virtual*/ bool BoundingBoxTree::BranchNode::FindNearestTriangle( const Vector& point, const Triangle*& nearestTriangle, double maxDistance ) const
{
	if( boundingBox.ContainsPoint( point ) )
	{
		if( backNode->FindNearestTriangle( point, nearestTriangle, maxDistance ) )
			return true;

		if( frontNode->FindNearestTriangle( point, nearestTriangle, maxDistance ) )
			return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------------------
//                                                    LeafNode
//-----------------------------------------------------------------------------------------------------------

BoundingBoxTree::LeafNode::LeafNode( void )
{
	triangleList = new TriangleList();
}

/*virtual*/ BoundingBoxTree::LeafNode::~LeafNode( void )
{
	delete triangleList;
}

/*virtual*/ bool BoundingBoxTree::LeafNode::InsertTriangle( const Triangle& triangle )
{
	if( boundingBox.ContainsTriangle( triangle ) )
	{
		triangleList->push_back( triangle );
		return true;
	}

	return false;
}

/*virtual*/ bool BoundingBoxTree::LeafNode::FindIntersection( const LineSegment& lineSegment, const Triangle*& intersectedTriangle, Vector& intersectionPoint ) const
{
	intersectedTriangle = nullptr;
	double smallestLambda = 2.0;

	for( TriangleList::const_iterator iter = triangleList->cbegin(); iter != triangleList->cend(); iter++ )
	{
		const Triangle& triangle = *iter;
		if( triangle.Intersect( lineSegment, intersectionPoint ) )
		{
			double lambda;
			if( lineSegment.LerpInverse( lambda, intersectionPoint ) )
			{
				if( lambda < smallestLambda )
				{
					smallestLambda = lambda;
					intersectedTriangle = &triangle;
				}
			}
		}
	}

	return( intersectedTriangle ? true : false );
}

/*virtual*/ bool BoundingBoxTree::LeafNode::FindNearestTriangle( const Vector& point, const Triangle*& nearestTriangle, double maxDistance ) const
{
	double smallestDistance = maxDistance;
	nearestTriangle = nullptr;

	for( TriangleList::const_iterator iter = triangleList->cbegin(); iter != triangleList->cend(); iter++ )
	{
		const Triangle& triangle = *iter;

		double distance = triangle.DistanceToPoint( point );
		if( distance <= smallestDistance )
		{
			smallestDistance = distance;
			nearestTriangle = &triangle;
		}
	}

	return( nearestTriangle ? true : false );
}

// BoundingBoxTree.cpp