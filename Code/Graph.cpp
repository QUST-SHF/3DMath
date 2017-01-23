// Graph.cpp

#include "Graph.h"

namespace _3DMath
{
	// Of course, this will only delete one connected component of the graph.
	void DeleteGraph( GraphNode* graphNode )
	{
		GraphTraversor traversor( graphNode );
		while( traversor.Traverse( graphNode ) )
			delete graphNode;
	}
}

using namespace _3DMath;

//------------------------------------------------------------------------------------
//                                       GraphNode
//------------------------------------------------------------------------------------

GraphNode::GraphNode( void )
{
}

/*virtual*/ GraphNode::~GraphNode( void )
{
}

GraphNode* GraphNode::GetAdjacency( const std::string& name )
{
	GraphNode* adjacentNode = nullptr;

	AdjacencyMap::iterator iter = adjacencyMap.find( name );
	if( iter != adjacencyMap.end() )
	{
		int graphNodeHandle = iter->second;
		adjacentNode = ( GraphNode* )HandleObject::Dereference( graphNodeHandle );
	}

	return adjacentNode;
}

void GraphNode::SetAdjacency( const std::string& name, GraphNode* graphNode )
{
	AdjacencyMap::iterator iter = adjacencyMap.find( name );
	if( iter != adjacencyMap.end() )
		adjacencyMap.erase( iter );

	if( graphNode )
		adjacencyMap.insert( std::pair< std::string, int >( name, graphNode->GetHandle() ) );
}

//------------------------------------------------------------------------------------
//                                    GraphTraversor
//------------------------------------------------------------------------------------

GraphTraversor::GraphTraversor( GraphNode* graphNode, Mode mode /*= BREADTH_FIRST*/ )
{
	Reset( graphNode, mode );
}

/*virtual*/ GraphTraversor::~GraphTraversor( void )
{
}

void GraphTraversor::Reset( GraphNode* graphNode, Mode mode /*= BREADTH_FIRST*/ )
{
	this->mode = mode;

	handleQueue.clear();

	if( graphNode )
		handleQueue.push_back( graphNode->GetHandle() );

	visitationSet.clear();
	enqueuedSet.clear();
}

/*virtual*/ bool GraphTraversor::Traverse( GraphNode*& graphNode )
{
	graphNode = nullptr;
	if( handleQueue.size() == 0 )
		return false;

	ObjectHandleList::iterator iter = handleQueue.begin();
	int graphNodeHandle = *iter;
	handleQueue.erase( iter );

	visitationSet.insert( graphNodeHandle );

	graphNode = ( GraphNode* )HandleObject::Dereference( graphNodeHandle );

	EnqueueUnvisitedAdjacencies( graphNode );

	return true;
}

/*virtual*/ void GraphTraversor::EnqueueUnvisitedAdjacencies( GraphNode* graphNode )
{
	if( graphNode )
		for( GraphNode::AdjacencyMap::iterator iter = graphNode->adjacencyMap.begin(); iter != graphNode->adjacencyMap.end(); iter++ )
			EnqueueIfNotVisitedOrEnqueued( iter->second );
}

void GraphTraversor::EnqueueIfNotVisitedOrEnqueued( int graphNodeHandle )
{
	HandleSet::iterator iter = visitationSet.find( graphNodeHandle );
	if( iter != visitationSet.end() )
		return;

	iter = enqueuedSet.find( graphNodeHandle );
	if( iter != enqueuedSet.end() )
		return;
	
	switch( mode )
	{
		case BREADTH_FIRST:
		{
			handleQueue.push_back( graphNodeHandle );
			break;
		}
		case DEPTH_FIRST:
		{
			handleQueue.push_front( graphNodeHandle );
			break;
		}
	}

	enqueuedSet.insert( graphNodeHandle );
}

//------------------------------------------------------------------------------------
//                            NamedAdjacencyGraphTraversor
//------------------------------------------------------------------------------------

NamedAdjacencyGraphTraversor::NamedAdjacencyGraphTraversor( const std::string& name, GraphNode* graphNode, Mode mode /*= BREADTH_FIRST*/ ) : GraphTraversor( graphNode, mode )
{
	this->name = name;
}

/*virtual*/ NamedAdjacencyGraphTraversor::~NamedAdjacencyGraphTraversor( void )
{
}

/*virtual*/ void NamedAdjacencyGraphTraversor::EnqueueUnvisitedAdjacencies( GraphNode* graphNode )
{
	GraphNode::AdjacencyMap::iterator iter = graphNode->adjacencyMap.find( name );
	if( iter != graphNode->adjacencyMap.end() )
		EnqueueIfNotVisitedOrEnqueued( iter->second );
}

// Graph.cpp