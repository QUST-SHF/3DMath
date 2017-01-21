// Polygon.cpp

#include "Polygon.h"
#include "Triangle.h"
#include "IndexTriangle.h"
#include "LineSegment.h"
#include "Surface.h"
#include "AffineTransform.h"
#include "Graph.h"
#include "Exception.h"

using namespace _3DMath;

Polygon::Polygon( void )
{
	vertexArray = new VectorArray();
	indexTriangleList = new IndexTriangleList();
}

/*virtual*/ Polygon::~Polygon( void )
{
	delete vertexArray;
	delete indexTriangleList;
}

void Polygon::Transform( const AffineTransform& transform )
{
	transform.Transform( *vertexArray );
}

bool Polygon::GetPlane( Plane& plane ) const
{
	if( vertexArray->size() < 3 )
		return false;

	Vector center;
	GetCenter( center );

	Vector normal;
	normal.Set( 0.0, 0.0, 0.0 );
	
	for( int i = 0; i < ( signed )vertexArray->size(); i++ )
	{
		// This is the Newel method.
		int j = ( i + 1 ) % vertexArray->size();
		const Vector& pointA = ( *vertexArray )[i];
		const Vector& pointB = ( *vertexArray )[j];
		normal.x += ( pointA.y - pointB.y ) * ( pointA.z + pointB.z );
		normal.y += ( pointA.z - pointB.z ) * ( pointA.x + pointB.x );
		normal.z += ( pointA.x - pointB.x ) * ( pointA.y + pointB.y );
	}

	plane.SetCenterAndNormal( center, normal );
	return true;
}

void Polygon::GetCenter( Vector& center ) const
{
	center.Set( 0.0, 0.0, 0.0 );
	for( int i = 0; i < ( signed )vertexArray->size(); i++ )
		center.Add( ( *vertexArray )[i] );
	center.Scale( 1.0 / double( vertexArray->size() ) );
}

bool Polygon::GetTriangleAverageCenter( Vector& center ) const
{
	if( indexTriangleList->size() == 0 )
		return false;

	center.Set( 0.0, 0.0, 0.0 );

	for( IndexTriangleList::const_iterator iter = indexTriangleList->cbegin(); iter != indexTriangleList->cend(); iter++ )
	{
		const IndexTriangle& indexTriangle = *iter;

		Triangle triangle;
		indexTriangle.GetTriangle( triangle, vertexArray );

		Vector triangleCenter;
		triangle.GetCenter( triangleCenter );

		center.Add( triangleCenter );
	}

	center.Scale( 1.0 / double( indexTriangleList->size() ) );
	return true;
}

void Polygon::GetIntegratedCenter( Vector& center, double delta ) const
{
	center.Set( 0.0, 0.0, 0.0 );
	int count = 0;

	for( int i = 0; i < ( signed )vertexArray->size(); i++ )
	{
		int j = ( i + 1 ) % vertexArray->size();
		LineSegment lineSegment( ( *vertexArray )[i], ( *vertexArray )[j] );

		int stepCount = ( int )floor( lineSegment.Length() / delta );

		for( int k = 0; k < stepCount; k++ )
		{
			double lambda = double(k) / double( stepCount );

			Vector point;
			lineSegment.Lerp( lambda, point );

			center.Add( point );
			count++;
		}
	}

	center.Scale( 1.0 / double( count ) );
}

// This assumes that we're properly tessellated.
bool Polygon::ContainsPoint( const Vector& point, double eps /*= EPSILON*/ ) const
{
	for( IndexTriangleList::const_iterator iter = indexTriangleList->cbegin(); iter != indexTriangleList->cend(); iter++ )
	{
		const IndexTriangle& indexTriangle = *iter;

		Triangle triangle;
		indexTriangle.GetTriangle( triangle, vertexArray );

		if( triangle.ContainsPoint( point, eps ) )
			return true;
	}

	return false;
}

// The set of returned polygons should be pair-wise disjoint and its union should equal this polygon.
// Note that we require this polygon to be tessellated for our algorithm here to be correct.
bool Polygon::SplitAgainstSurface( const Surface* surface, PolygonList& insidePolygonList, PolygonList& outsidePolygonList, double maxDistance ) const
{
	bool success = true;

	Plane plane;
	GetPlane( plane );

	struct PointData
	{
		Vector point;
		Surface::Side side;
		bool processed;
	};

	typedef TemplateGraphNode< PointData > Node;

	Node* anchorNode = nullptr;

	try
	{
		Node* node = nullptr;

		for( int i = 0; i < ( signed )vertexArray->size(); i++ )
		{
			Node* newNode = new Node();
			newNode->data.processed = false;
			newNode->data.point = ( *vertexArray )[i];
			newNode->data.side = surface->GetSide( newNode->data.point );

			if( !node )
				anchorNode = newNode;
			else
			{
				node->SetAdjacency( "ccw", newNode );
				newNode->SetAdjacency( "cw", node );
			}

			node = newNode;
		}

		if( !node )
			throw new Exception( "The polygon is empty." );
		else
		{
			node->SetAdjacency( "ccw", anchorNode );
			anchorNode->SetAdjacency( "cw", node );
		}

		node = nullptr;

		GraphNodeList graphNodeList;
		GraphNode* graphNode = nullptr;

		GraphTraversor traversor( anchorNode );
		while( traversor.Traverse( graphNode ) )
		{
			Node* cwNode = ( Node* )graphNode;
			Node* ccwNode = ( Node* )cwNode->GetAdjacency( "ccw" );

			if( ( cwNode->data.side == Surface::INSIDE && ccwNode->data.side == Surface::OUTSIDE ) ||
				( cwNode->data.side == Surface::OUTSIDE && ccwNode->data.side == Surface::INSIDE ) )
			{
				graphNodeList.push_back( cwNode );
			}
		}

		for( GraphNodeList::iterator iter = graphNodeList.begin(); iter != graphNodeList.end(); iter++ )
		{
			Node* cwNode = ( Node* )*iter;
			Node* ccwNode = ( Node* )cwNode->GetAdjacency( "ccw" );

			LineSegment lineSegment;
			lineSegment.vertex[0] = cwNode->data.point;
			lineSegment.vertex[1] = ccwNode->data.point;

			SurfacePoint* surfacePoint = surface->FindSingleIntersection( lineSegment );
			if( !surfacePoint )
				throw new Exception( "Failed to intersect polygon edge with surface." );
			else
			{
				Node* newNode = new Node();
				newNode->data.processed = false;
				newNode->data.side = Surface::NEITHER_SIDE;
				surfacePoint->GetLocation( newNode->data.point );
				delete surfacePoint;
				cwNode->SetAdjacency( "ccw", newNode );
				ccwNode->SetAdjacency( "cw", newNode );
				newNode->SetAdjacency( "cw", cwNode );
				newNode->SetAdjacency( "ccw", ccwNode );
			}
		}

		GraphNodeArray graphNodeArray;

		// Note that if the polygon didn't have a high-enough density of vertices,
		// (we can, without changing the polygon, add vertices on edges), then we
		// can miss nodes here that need to be processed.
		NamedAdjacencyGraphTraversor namedTraversor( "ccw", anchorNode );
		while( namedTraversor.Traverse( graphNode ) )
		{
			node = ( Node* )graphNode;

			Node* cwNode = ( Node* )node->GetAdjacency( "cw" );
			Node* ccwNode = ( Node* )node->GetAdjacency( "ccw" );

			if( node->data.side == Surface::NEITHER_SIDE )
			{
				if( ( cwNode->data.side == Surface::INSIDE && ccwNode->data.side == Surface::OUTSIDE ) ||
					( cwNode->data.side == Surface::OUTSIDE && ccwNode->data.side == Surface::INSIDE ) )
				{
					graphNodeArray.push_back( node );
				}
			}
		}

		int count = ( signed )graphNodeArray.size();
		if( count == 2 )
			count--;

		for( int i = 0; i < count; i++ )
		{
			int j = ( i + 1 ) % graphNodeArray.size();

			Node* nodeA = ( Node* )graphNodeArray[i];
			Node* nodeB = ( Node* )graphNodeArray[j];

			bool pathFound = false;

			SurfacePoint* surfacePointA = surface->GetNearestSurfacePoint( nodeA->data.point );
			SurfacePoint* surfacePointB = surface->GetNearestSurfacePoint( nodeB->data.point );
			SurfacePoint* surfacePointC = nullptr;

			VectorArray pointArray;
			
			if( surfacePointA && surfacePointB )
			{
				pathFound = surface->FindDirectPath( surfacePointA, surfacePointB, pointArray, maxDistance, &plane );
				if( !pathFound )
				{
					// This is a bit of a hack.
					Vector center;
					GetCenter( center );

					surfacePointC = surface->GetNearestSurfacePoint( center );
					pathFound = surface->FindDirectPath( surfacePointA, surfacePointC, pointArray, maxDistance, &plane );
					if( pathFound )
					{
						pointArray.pop_back();
						pathFound = surface->FindDirectPath( surfacePointC, surfacePointA, pointArray, maxDistance, &plane );
					}
				}
			}

			delete surfacePointA;
			delete surfacePointB;
			delete surfacePointC;

			if( !pathFound )
				throw new Exception( "Failed to find path along surface in plane." );

			for( j = 0; j < ( signed )pointArray.size(); j++ )
				if( !ContainsPoint( pointArray[j], 1e-3 ) )
					break;

			if( j == ( signed )pointArray.size() )
			{
				for( j = 1; j < ( signed )pointArray.size() - 1; j++ )
				{
					Node* newNode = new Node();
					newNode->data.processed = false;
					newNode->data.side = Surface::NEITHER_SIDE;
					newNode->data.point = pointArray[j];

					nodeA->SetAdjacency( "s_d0", newNode );
					newNode->SetAdjacency( "s_d1", nodeA );

					nodeA = newNode;
				}

				nodeA->SetAdjacency( "s_d0", nodeB );
				nodeB->SetAdjacency( "s_d1", nodeA );
			}
		}

		while( true )
		{
			Node* foundNode = nullptr;

			traversor.Reset( anchorNode );
			while( traversor.Traverse( graphNode ) )
			{
				node = ( Node* )graphNode;
				if( node->data.side != Surface::NEITHER_SIDE && !node->data.processed )
				{
					foundNode = node;
					break;
				}
			}

			if( !foundNode )
				break;

			Surface::Side side = foundNode->data.side;

			Polygon* polygon = new Polygon;
			if( side == Surface::INSIDE )
				insidePolygonList.push_back( polygon );
			else if( side == Surface::OUTSIDE )
				outsidePolygonList.push_back( polygon );
			else
			{
				delete polygon;
				throw new Exception( "Encountered unexpected surface side." );
			}

			node = foundNode;

			std::string surfaceDir;

			do
			{
				if( node->data.side != side && node->data.side != Surface::NEITHER_SIDE )
					throw new Exception( "Encountered node on wrong side." );

				polygon->vertexArray->push_back( node->data.point );
				node->data.processed = true;

				Node* adjacentNode = nullptr;

				if( node->data.side == Surface::NEITHER_SIDE )
				{
					if( !surfaceDir.empty() )
						adjacentNode = ( Node* )node->GetAdjacency( surfaceDir );
					else
					{
						Node* nodeDir0 = ( Node* )node->GetAdjacency( "s_d0" );
						Node* nodeDir1 = ( Node* )node->GetAdjacency( "s_d1" );

						if( nodeDir0 && nodeDir1 )
							throw new Exception( "Ambiguous case encountered." );
						else if( nodeDir0 )
						{
							adjacentNode = nodeDir0;
							surfaceDir = "s_d0";
						}
						else if( nodeDir1 )
						{
							adjacentNode = nodeDir1;
							surfaceDir = "s_d1";
						}
						else
							throw new Exception( "No where to go." );
					}
				}

				if( !adjacentNode )
				{
					adjacentNode = ( Node* )node->GetAdjacency( "ccw" );
					surfaceDir = "";
				}
				
				if( !adjacentNode )
					throw new Exception( "Encountered unexpected boundary." );

				node = adjacentNode;
			}
			while( node != foundNode );
		}
	}
	catch( Exception* exception )
	{
		exception->Handle();
		delete exception;
		success = false;
	}

	DeleteGraph( anchorNode );

	return success;
}

double Polygon::GetArea( void ) const
{
	double totalArea = 0.0;

	for( IndexTriangleList::const_iterator iter = indexTriangleList->cbegin(); iter != indexTriangleList->end(); iter++ )
	{
		const IndexTriangle& indexTriangle = *iter;
		Triangle triangle;
		indexTriangle.GetTriangle( triangle, vertexArray );
		totalArea += triangle.Area();
	}

	return totalArea;
}

bool Polygon::Tessellate( void ) const
{
	Plane plane;
	if( !GetPlane( plane ) )
		return false;

	indexTriangleList->clear();

	std::vector< int > indexArray;
	for( int i = 0; i < ( signed )vertexArray->size(); i++ )
		indexArray.push_back(i);

	while( indexArray.size() > 2 )
	{
		int i;
		for( i = 0; i < ( signed )indexArray.size(); i++ )
		{
			IndexTriangle indexTriangle(
					indexArray[i],
					indexArray[ ( i + 1 ) % indexArray.size() ],
					indexArray[ ( i + 2 ) % indexArray.size() ] );

			Triangle triangle;
			indexTriangle.GetTriangle( triangle, vertexArray );

			Vector edge[2];
			edge[0].Subtract( triangle.vertex[1], triangle.vertex[0] );
			edge[1].Subtract( triangle.vertex[2], triangle.vertex[1] );

			Vector cross;
			cross.Cross( edge[0], edge[1] );
			double dot = cross.Dot( plane.normal );
			if( dot < 0.0 )
				continue;

			int j;
			for( j = 0; j < ( signed )indexArray.size(); j++ )
			{
				if( j == i || j == ( i + 1 ) % indexArray.size() || j == ( i + 2 ) % indexArray.size() )
					continue;
				if( triangle.ContainsPoint( ( *vertexArray )[ indexArray[j] ] ) )
					break;
			}

			if( j < ( signed )indexArray.size() )
				continue;

			indexTriangleList->push_back( indexTriangle );
			indexArray.erase( indexArray.begin() + ( i + 1 ) % indexArray.size() );
			break;
		}

		if( i == ( signed )indexArray.size() )
			return false;		// Avoid an infinite loop.
	}

	return true;
}

// Polygon.cpp