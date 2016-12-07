// FileFormat.cpp

#include "FileFormat.h"
#include "TriangleMesh.h"
#include "Exception.h"
#include "Renderer.h"

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

/*virtual*/ bool FileFormat::LoadTriangleMesh( TriangleMesh& triangleMesh, const std::string& file )
{
	std::ifstream stream;
	stream.open( file );
	if( !stream.is_open() )
		return false;

	return LoadTriangleMesh( triangleMesh, stream );
}

/*virtual*/ bool FileFormat::SaveTriangleMesh( const TriangleMesh& triangleMesh, const std::string& file )
{
	std::ofstream stream;
	stream.open( file );
	if( !stream.is_open() )
		return false;

	return SaveTriangleMesh( triangleMesh, stream );
}

/*static*/ FileFormat* FileFormat::CreateForFile( const std::string& file )
{
	FileFormat* fileFormat = nullptr;

	if( file.find( ".ply" ) >= 0 )
		fileFormat = new PlyFormat();

	return fileFormat;
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

/*virtual*/ bool PlyFormat::LoadTriangleMesh( TriangleMesh& triangleMesh, std::istream& stream )
{
    bool success = true;
    LineList* lineList = nullptr;
    
    try
    {
        triangleMesh.Clear();

        lineList = TokenizeFile( stream );
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
            if( ( *headerArray )[0] == "element" )
            {
                if( ( *headerArray )[1] == "vertex" )
                {
                    int count = atoi( ( *headerArray )[2].c_str() );
                    for( int i = 0; i < count; i++ )
					{
                        AddVertex( triangleMesh, headerIter, bodyIter );
						bodyIter++;
					}
                }
                else if( ( *headerArray )[1] == "face" )
                {
                    int count = atoi( ( *headerArray )[2].c_str() );
                    for( int i = 0; i < count; i++ )
					{
                        AddTriangles( triangleMesh, headerIter, bodyIter );
						bodyIter++;
					}
                }
                else
                    throw new Exception( "Unknown element section: " + ( *headerArray )[1] );
            }

			headerArray = *( ++headerIter );
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

    for( int i = 0; i < ( signed )bodyArray->size(); i++ )
    {
        const StringArray* propertyArray = *( ++propretyIter );

        if( ( *propertyArray )[0] != "property" )
            throw new Exception( "Expected property." );

        double value = atof( ( *bodyArray )[i].c_str() );

		std::string component = ( *propertyArray )[2];

        if( component == "x" )
            vertex.position.x = value;
        else if( component == "y" )
            vertex.position.y = value;
        else if( component == "z" )
            vertex.position.z = value;
        else if( component == "r" )
            vertex.color.x = value;
        else if( component == "g" )
            vertex.color.y = value;
        else if( component == "b" )
            vertex.color.z = value;
        else if( component == "nx" )
            vertex.normal.x = value;
        else if( component == "ny" )
            vertex.normal.y = value;
        else if( component == "nz" )
            vertex.normal.z = value;
        else if( component == "u" )
            vertex.u = value;
        else if( component == "v" )
            vertex.v = value;
        else
			throw new Exception( "Unexpected vertex component: " + component );
    }

    triangleMesh.vertexArray->push_back( vertex );
}

void PlyFormat::AddTriangles( TriangleMesh& triangleMesh, const LineList::iterator& headerIter, const LineList::iterator& bodyIter )
{
    LineList::iterator propretyIter = headerIter;
    const StringArray* bodyArray = *bodyIter;

    const StringArray* propertyArray = *( ++propretyIter );
    if( ( *propertyArray )[0] != "property" ||
        ( *propertyArray )[1] != "list" ||
        ( *propertyArray )[2] != "uchar" ||
        ( *propertyArray )[3] != "int" ||
        ( *propertyArray )[4] != "vertex_index" )
    {
        throw new Exception( "Unsupported face format." );
    }

    // Choose an arbitrary tessellation of the polygon.
    int count = atoi( ( *bodyArray )[0].c_str() );
    for( int i = 0; i < count - 2; i++ )
    {
		int vertex0 = atoi( ( *bodyArray )[1].c_str() );
		int vertex1 = atoi( ( *bodyArray )[ 1 + i + 1 ].c_str() );
		int vertex2 = atoi( ( *bodyArray )[ 1 + i + 2 ].c_str() );

        triangleMesh.triangleList->push_back( TriangleMesh::IndexTriangle( vertex0, vertex1, vertex2, &triangleMesh ) );
    }
}

PlyFormat::LineList* PlyFormat::TokenizeFile( std::istream& stream )
{
	LineList* lineList = new LineList();

	std::string line;
	while( std::getline( stream, line ) )
	{
		StringArray* stringArray = new StringArray();
		lineList->push_back( stringArray );

		std::regex regex( "(\\S+)" );
		std::sregex_iterator iter( line.begin(), line.end(), regex );
		while( iter != std::sregex_iterator() )
		{
			std::smatch match = *iter;
			std::string word = match.str();
			stringArray->push_back( word );
			iter++;
		}
	}

    return lineList;
}

/*virtual*/ bool PlyFormat::SaveTriangleMesh( const TriangleMesh& triangleMesh, std::ostream& stream )
{
	stream << "ply" << std::endl;
	stream << "format ascii 1.0" << std::endl;
	stream << "comment Generated by 3DMath library." << std::endl;
	stream << "element vertex " << triangleMesh.vertexArray->size() << std::endl;
	stream << "property double x" << std::endl;
	stream << "property double y" << std::endl;
	stream << "property double z" << std::endl;
	stream << "property double nx" << std::endl;
	stream << "property double ny" << std::endl;
	stream << "property double nz" << std::endl;
	stream << "property double r" << std::endl;
	stream << "property double g" << std::endl;
	stream << "property double b" << std::endl;
	stream << "property double u" << std::endl;
	stream << "property double v" << std::endl;
	stream << "element face " << triangleMesh.triangleList->size() << std::endl;
	stream << "property list uchar int vertex_index" << std::endl;
	stream << "end_header" << std::endl;

	for( int i = 0; i < ( signed )triangleMesh.vertexArray->size(); i++ )
	{
		const Vertex& vertex = ( *triangleMesh.vertexArray )[i];

		stream << vertex.position.x << " " << vertex.position.y << " " << vertex.position.z << " ";
		stream << vertex.normal.x << " " << vertex.normal.y << " " << vertex.normal.z << " ";
		stream << vertex.color.x << " " << vertex.color.y << " " << vertex.color.z << " ";
		stream << vertex.u << " " << vertex.v << std::endl;
	}

	for( TriangleMesh::IndexTriangleList::const_iterator iter = triangleMesh.triangleList->cbegin(); iter != triangleMesh.triangleList->cend(); iter++ )
	{
		const TriangleMesh::IndexTriangle& triangle = *iter;

		stream << "3 " << triangle.vertex[0] << " " << triangle.vertex[1] << " " << triangle.vertex[2] << std::endl;
	}

    return true;
}

// FileFormat.cpp