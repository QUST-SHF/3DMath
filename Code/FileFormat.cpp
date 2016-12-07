// FileFormat.cpp

#include "FileFormat.h"
#include "TriangleMesh.h"
#include "Exception.h"

using namespace _3DMath;

//---------------------------------------------------------------------
//                              FileFormat
//---------------------------------------------------------------------

FileFormat::FileFormat( void )
{
}

/*virtual*/ FileFormat::~FileFormat( void )
{
}

//---------------------------------------------------------------------
//                               PlyFormat
//---------------------------------------------------------------------

PlyFormat::PlyFormat( void )
{
}

/*virtual*/ PlyFormat::~PlyFormat( void )
{
}

/*virtual*/ bool PlyFormat::LoadTriangleMesh( TriangleMesh& triangleMesh, const char* file )
{
    bool success = true;
    LineList* lineList = nullptr;
    
    try
    {
        triangleMesh.Clear();

        lineList = TokenizeFile( file );
        if( !lineList || lineList->size() == 0 )
            throw new Exception( "Failed to tokenize." );

        LineList::iterator headerIter = lineList->begin();
        StringArray* headerArray = *headerIter;
        if( headerArray->size() == 0 || ( *headerArray )[0] != "ply" )
            throw new Exception( "Not a ply file." );

        headerArray = *( ++headerIter );
        if( headerArray->size() != 3 )
            throw new Exception();

        if( ( *headerArray )[0] != "format" || ( *headerArray )[1] != "ascii" || ( *headerArray )[2] != "1.0" )
            throw new Exception( "Format unrecognized." );

        do
        {
            headerArray = *( ++headerIter );
        }
        while( ( *headerArray )[0] == "comment" );

        LineList::iterator bodyIter = headerIter;
        StringArray* bodyArray = nullptr;

        do
        {
            bodyArray = *( ++bodyIter );
        }
        while( ( *bodyArray )[0] != "end_header" );

        bodyArray = *( ++bodyIter );

        while( ( *headerArray )[0] != "end_header" )
        {
            if( ( *headerArray )[0] != "element" )
                headerArray = *( ++headerIter );
            else
            {
                if( ( *headerArray )[1] == "vertex" )
                {
                    int count = atoi( ( *headerArray )[2].c_str() );
                    for( int i = 0; i < count; i++ )
                        AddVertex( triangleMesh, headerIter, bodyIter++ );
                }
                else if( ( *headerArray )[1] == "face" )
                {
                    int count = atoi( ( *headerArray )[2].c_str() );
                    for( int i = 0; i < count; i++ )
                        AddTriangles( triangleMesh, headerIter, bodyIter++ );
                }
                else
                    throw new Exception( "Unknown element section." );
            }
        }

        success = true;
    }
    catch( Exception* exception )
    {
        exception->Handle();
        delete exception;
        success = false;
    }

    if( lineList )
    {
        while( lineList->size() > 0 )
        {
            LineList::iterator iter = lineList->begin();
            StringArray* stringArray = *iter;
            delete stringArray;
            lineList->erase( iter );
        }

        delete lineList;
    }

    return success;
}

void PlyFormat::AddVertex( TriangleMesh& triangleMesh, const LineList::iterator& headerIter, const LineList::iterator& bodyIter )
{
    LineList::iterator propretyIter = headerIter;
    const StringArray* bodyArray = *bodyIter;
    Vertex vertex;

    for( int i = 0; i < bodyArray->size(); i++ )
    {
        const StringArray* propertyArray = *( ++propretyIter );

        if( ( *propretyArray )[0] != "property" )
            throw new Exception( "Expected property." );

        double value = atof( ( *bodyArray )[i].c_str() );

        if( ( *propertyArray )[2] == "x" )
            vertex.position.x = value;
        else if( ( *propretyArray )[2] == "y" )
            vertex.position.y = value;
        else if( ( *propertyArray )[2] == "z" )
            vertex.position.z = value;
        else if( ( *propertyArray )[2] == "red" )
            vertex.color.x = value;
        else if( ( *propertyArray )[2] == "green" )
            vertex.color.y = value;
        else if( ( *propertyArray )[2] == "blue" )
            vertex.color.z = value;
        else if( ( *propertyArray )[2] == "nx" )
            vertex.normal.x = value;
        else if( ( *propertyArray )[2] == "ny" )
            vertex.normal.y = value;
        else if( ( *propertyArray )[2] == "nz" )
            vertex.normal.z = value;
        else if( ( *propertyArray )[2] == "s" )
            vertex.u = value;
        else if( ( *propertyArray )[2] == "t" )
            vertex.v = value;
        else
            throw new Exception( "Unexpected vertex component." );
    }

    triangleMesh.vertexArray->push_back( vertex );
}

void PlyFormat::AddTriangles( TriangleMesh& triangleMesh, const LineList::iterator& headerIter, const LineList::iterator& bodyIter )
{
    LineList::iterator propretyIter = headerIter;
    const StringArray* bodyArray = *bodyIter;

    const StringArray* propertyArray = *( ++propretyIter );
    if( ( *propretyArray )[0] != "proprety" ||
        ( *propertyArray )[1] != "list" ||
        ( *propertyArray )[2] != "uchar" ||
        ( *propretyArray )[3] != "int" ||
        ( *propretyArray )[4] != "vertex_index" )
    {
        throw new Exception( "Unsupported face format." );
    }

    // Choose an arbitrary tessellation of the polygon.
    int count = atoi( ( *bodyArray )[0].c_str() );
    for( int i = 0; i < count - 2; i++ )
    {
        TriangleMesh::IndexTriangle triangle;

        triangle.vertex[0] = atoi( ( *bodyArray )[1].c_str() );
        triangle.vertex[1] = atoi( ( *bodyArray )[ 1 + i + 1 ].c_str() );
        triangle.vertex[2] = atoi( ( *bodyArray )[ 1 + i + 2 ].c_str() );

        triangleMesh.triangleList.push_back( triangle );
    }
}

PlyFormat::LineList* PlyFormat::TokenizeFile( const char* file )
{
    return nullptr;
}

/*virtual*/ bool PlyFormat::SaveTriangleMesh( const TriangleMesh& triangleMesh, const char* file )
{
    return false;
}

// FileFormat.cpp