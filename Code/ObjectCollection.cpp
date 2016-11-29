// ObjectCollection.cpp

#include "ObjectCollection.h"

using namespace _3DMath;

Object::Object( void )
{
	id = 0;
}

/*virtual*/ Object::~Object( void )
{
}

ObjectCollection::ObjectCollection( void )
{
	objectMap = new ObjectMap();
	nextId = 1;
}

/*virtual*/ ObjectCollection::~ObjectCollection( void )
{
	while( objectMap->size() > 0 )
	{
		ObjectMap::iterator iter = objectMap->begin();
		Object* object = iter->second;
		delete object;
		objectMap->erase( iter );
	}

	delete objectMap;
}

bool ObjectCollection::AddObject( Object* object )
{
	if( object->id != 0 )
		return false;

	object->id = nextId++;
	objectMap->insert( std::pair< int, Object* >( object->id, object ) );
	return true;
}

Object* ObjectCollection::FindObject( int objectId )
{
	Object* object = nullptr;

	ObjectMap::iterator iter = objectMap->find( objectId );
	if( iter != objectMap->end() )
		object = iter->second;

	return object;
}

// ObjectCollection.cpp