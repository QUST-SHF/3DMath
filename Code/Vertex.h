// Vertex.h

#pragma once

#include "Defines.h"
#include "Vector.h"

namespace _3DMath
{
	class Vertex;
}

class _3DMATH_API _3DMath::Vertex
{
public:

	Vertex( void );
	Vertex( const Vector& position );
	Vertex( const Vector& position, const Vector& color );
	Vertex( const Vector& position, const Vector& normal, double u, double v );
	~Vertex( void );

	Vector position;
	Vector normal;
	Vector color;
	Vector texCoords;
	double alpha;
};

namespace _3DMath
{
	typedef std::vector< Vertex > VertexArray;
}

// Vertex.h
