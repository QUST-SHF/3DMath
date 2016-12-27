// Vertex.cpp

#include "Vertex.h"

using namespace _3DMath;

Vertex::Vertex( void )
{
	position.Set( 0.0, 0.0, 0.0 );
	normal.Set( 0.0, 0.0, 0.0 );
	color.Set( 1.0, 1.0, 1.0 );
	texCoords.Set( 0.0, 0.0, 0.0 );
	alpha = 1.0;
}

Vertex::Vertex( const Vector& position )
{
	this->position = position;
	normal.Set( 0.0, 0.0, 0.0 );
	color.Set( 0.0, 0.0, 0.0 );
	texCoords.Set( 0.0, 0.0, 0.0 );
	alpha = 1.0;
}

Vertex::Vertex( const Vector& position, const Vector& color )
{
	this->position = position;
	this->color = color;
	normal.Set( 0.0, 1.0, 0.0 );
	texCoords.Set( 0.0, 0.0, 0.0 );
	alpha = 1.0;
}

Vertex::Vertex( const Vector& position, const Vector& normal, double u, double v )
{
	this->position = position;
	this->normal = normal;
	texCoords.Set( u, v, 0.0 );
	color.Set( 0.0, 0.0, 0.0 );
	alpha = 1.0;
}

Vertex::~Vertex( void )
{
}

// Vertex.cpp