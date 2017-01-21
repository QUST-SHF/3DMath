// Graph.h

#pragma once

#include "Defines.h"
#include "HandleObject.h"

namespace _3DMath
{
	class GraphNode;
	class GraphTraversor;
	class NamedAdjacencyGraphTraversor;

	typedef std::list< GraphNode* > GraphNodeList;
	typedef std::vector< GraphNode* > GraphNodeArray;

	_3DMATH_API void DeleteGraph( GraphNode* graphNode );
}

class _3DMATH_API _3DMath::GraphNode : public _3DMath::HandleObject
{
public:

	GraphNode( void );
	virtual ~GraphNode( void );

	GraphNode* GetAdjacency( const std::string& name );
	void SetAdjacency( const std::string& name, GraphNode* graphNode );

	typedef std::map< std::string, int > AdjacencyMap;
	AdjacencyMap adjacencyMap;
};

namespace _3DMath
{
	template< typename Data >
	class _3DMATH_API TemplateGraphNode : public _3DMath::GraphNode
	{
	public:

		TemplateGraphNode( void )
		{
		}

		TemplateGraphNode( const Data& data )
		{
			this->data = data;
		}

		virtual ~TemplateGraphNode( void )
		{
		}

		Data data;
	};
}

class _3DMATH_API _3DMath::GraphTraversor
{
public:

	enum Mode
	{
		DEPTH_FIRST,
		BREADTH_FIRST,
	};

	GraphTraversor( GraphNode* graphNode, Mode mode = BREADTH_FIRST );
	virtual ~GraphTraversor( void );

	void Reset( GraphNode* graphNode, Mode mode = BREADTH_FIRST );

	virtual bool Traverse( GraphNode*& graphNode );
	virtual void EnqueueUnvisitedAdjacencies( GraphNode* graphNode );

	void EnqueueIfNotVisitedOrEnqueued( int graphNodeHandle );

	Mode mode;
	ObjectHandleList handleQueue;
	
	typedef std::set< int > HandleSet;
	HandleSet visitationSet, enqueuedSet;
};

class _3DMATH_API _3DMath::NamedAdjacencyGraphTraversor : public _3DMath::GraphTraversor
{
public:

	NamedAdjacencyGraphTraversor( const std::string& name, GraphNode* graphNode, Mode mode = BREADTH_FIRST );
	virtual ~NamedAdjacencyGraphTraversor( void );

	virtual void EnqueueUnvisitedAdjacencies( GraphNode* graphNode ) override;

	std::string name;
};

// Graph.h