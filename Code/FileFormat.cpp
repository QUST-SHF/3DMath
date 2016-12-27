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

	if( file.find( ".obj" ) >= 0 )
		fileFormat = new ObjFormat();

	return fileFormat;
}

FileFormat::LineArray* FileFormat::TokenizeFile( std::istream& stream )
{
	LineArray* lineArray = new LineArray();

	std::string line;
	while( std::getline( stream, line ) )
	{
		StringArray* stringArray = new StringArray();
		lineArray->push_back( stringArray );

#if 0
		std::regex regex( "(\\S+)" );
		std::sregex_iterator iter( line.begin(), line.end(), regex );
		while( iter != std::sregex_iterator() )
		{
			std::smatch match = *iter;
			std::string word = match.str();
			stringArray->push_back( word );
			iter++;
		}
#else
		std::string word;
		int length = line.length();
		for( int i = 0; i < length; i++ )
		{
			char ch = line.c_str()[i];
			if( !::isspace( ch ) )
				word += ch;
			else if( word.size() > 0 )
			{
				stringArray->push_back( word );
				word.clear();
			}
		}

		if( word.size() > 0 )
			stringArray->push_back( word );
#endif
	}

    return lineArray;
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
    LineArray* lineArray = nullptr;
    
    try
    {
		// TODO: Support binary PLY format too?

        triangleMesh.Clear();

        lineArray = TokenizeFile( stream );
        if( !lineArray || lineArray->size() == 0 )
            throw new Exception( "Failed to tokenize." );

        LineArray::iterator headerIter = lineArray->begin();
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

        LineArray::iterator bodyIter = headerIter;
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

    if( lineArray )
    {
        while( lineArray->size() > 0 )
        {
            LineArray::iterator iter = lineArray->begin();
            StringArray* stringArray = *iter;
            delete stringArray;
            lineArray->erase( iter );
        }

        delete lineArray;
    }

    return success;
}

void PlyFormat::AddVertex( TriangleMesh& triangleMesh, const LineArray::iterator& headerIter, const LineArray::iterator& bodyIter )
{
    LineArray::iterator propretyIter = headerIter;
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
        else if( component == "u" || component == "s" )
            vertex.texCoords.x = value;
        else if( component == "v" || component == "t" )
            vertex.texCoords.y = value;
        else
			throw new Exception( "Unexpected vertex component: " + component );
    }

    triangleMesh.vertexArray->push_back( vertex );
}

void PlyFormat::AddTriangles( TriangleMesh& triangleMesh, const LineArray::iterator& headerIter, const LineArray::iterator& bodyIter )
{
    LineArray::iterator propretyIter = headerIter;
    const StringArray* bodyArray = *bodyIter;

    const StringArray* propertyArray = *( ++propretyIter );
    if( ( *propertyArray )[0] != "property" ||
        ( *propertyArray )[1] != "list" ||
        ( *propertyArray )[2] != "uchar" ||
        ( ( *propertyArray )[3] != "int" && ( *propertyArray )[3] != "uint" )||
        ( *propertyArray )[4] != "vertex_indices" )
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

        triangleMesh.triangleList->push_back( IndexTriangle( vertex0, vertex1, vertex2 ) );
    }
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
	stream << "property list uchar int vertex_indices" << std::endl;
	stream << "end_header" << std::endl;

	for( int i = 0; i < ( signed )triangleMesh.vertexArray->size(); i++ )
	{
		const Vertex& vertex = ( *triangleMesh.vertexArray )[i];

		stream << vertex.position.x << " " << vertex.position.y << " " << vertex.position.z << " ";
		stream << vertex.normal.x << " " << vertex.normal.y << " " << vertex.normal.z << " ";
		stream << vertex.color.x << " " << vertex.color.y << " " << vertex.color.z << " ";
		stream << vertex.texCoords.x << " " << vertex.texCoords.y << std::endl;
	}

	for( IndexTriangleList::const_iterator iter = triangleMesh.triangleList->cbegin(); iter != triangleMesh.triangleList->cend(); iter++ )
	{
		const IndexTriangle& triangle = *iter;

		stream << "3 " << triangle.vertex[0] << " " << triangle.vertex[1] << " " << triangle.vertex[2] << std::endl;
	}

    return true;
}

//---------------------------------------------------------------------
//                               ObjFormat
//---------------------------------------------------------------------

ObjFormat::ObjFormat( void )
{
}

/*virtual*/ ObjFormat::~ObjFormat( void )
{
}

/*virtual*/ bool ObjFormat::LoadTriangleMesh( TriangleMesh& triangleMesh, std::istream& stream )
{
	bool success = true;

	LineArray* lineArray = nullptr;
    
    try
    {
		triangleMesh.Clear();

        lineArray = TokenizeFile( stream );
        if( !lineArray || lineArray->size() == 0 )
            throw new Exception( "Failed to tokenize." );

		int positionIndexBase = FindFirstLine( lineArray, "v" );
		if( positionIndexBase < 0 )
			throw new Exception( "Did not find vertex buffer." );

		int faceIndexBase = FindFirstLine( lineArray, "f" );
		if( faceIndexBase < 0 )
			throw new Exception( "Did not find face buffer." );

		// These are optional.
		int texCoordIndexBase = FindFirstLine( lineArray, "vt" );
		int normalIndexBase = FindFirstLine( lineArray, "vn" );

		// This will most likely not be as compressed as it could be.
		for( int faceIndex = 0; faceIndexBase + faceIndex < ( signed )lineArray->size(); faceIndex++ )
		{
			StringArray* faceLine = ( *lineArray )[ faceIndexBase + faceIndex ];
			if( ( *faceLine )[0] == "f" )
			{
				int j = triangleMesh.vertexArray->size();

				for( int i = 1; i < ( signed )faceLine->size(); i++ )
				{
					int positionIndex, texCoordIndex, normalIndex;
					std::string& vertexString = ( *faceLine )[i];
					if( !ParseVertex( vertexString, positionIndex, texCoordIndex, normalIndex ) )
						throw new Exception( "Failed to parse vertex: " + vertexString );

					StringArray* positionLine = ( positionIndex >= 0 ) ? ( *lineArray )[ positionIndexBase + positionIndex - 1 ] : nullptr;
					StringArray* texCoordLine = ( texCoordIndex >= 0 ) ? ( *lineArray )[ texCoordIndexBase + texCoordIndex - 1 ] : nullptr;
					StringArray* normalLine = ( normalIndex >= 0 ) ? ( *lineArray )[ normalIndexBase + normalIndex - 1 ] : nullptr;

					Vertex vertex;
					PopulateVector( positionLine, 1, vertex.position );
					if( texCoordLine )
						PopulateVector( texCoordLine, 1, vertex.texCoords );
					if( normalLine )
						PopulateVector( normalLine, 1, vertex.normal );

					triangleMesh.vertexArray->push_back( vertex );
				}

				// Choose an arbitrary tesselation of the face.
				int vertexCount = ( signed )faceLine->size() - 1;
				for( int i = 0; i < vertexCount - 2; i++ )
					triangleMesh.triangleList->push_back( IndexTriangle( j, j + i + 1, j + i + 2 ) );
			}
		}
	}
	catch( Exception* exception )
	{
		exception->Handle();
		delete exception;
		success = false;
	}

	for( int i = 0; i < ( signed )lineArray->size(); i++ )
		delete ( *lineArray )[i];
	delete lineArray;

	return success;
}

/*virtual*/ bool ObjFormat::SaveTriangleMesh( const TriangleMesh& triangleMesh, std::ostream& stream )
{
	return false;
}

int ObjFormat::FindFirstLine( const LineArray* lineArray, const std::string& lineType )
{
	for( int i = 0; i < ( signed )lineArray->size(); i++ )
	{
		StringArray* stringArray = ( *lineArray )[i];
		if( stringArray->size() > 0 && ( *stringArray )[0] == lineType )
			return i;
	}

	return -1;
}

bool ObjFormat::ParseVertex( const std::string& vertexString, int& positionIndex, int& texCoordIndex, int& normalIndex )
{
	positionIndex = -1;
	texCoordIndex = -1;
	normalIndex = -1;

	std::string stringList[3];

	int i = 0, j = 0;

	while( j < 3 && vertexString.data()[i] != '\0' )
	{
		while( vertexString.data()[i] != '/' && vertexString.data()[i] != '\0' )
			stringList[j] += vertexString.data()[ i++ ];

		j++;

		if( vertexString.data()[i] == '/' )
			i++;
	}

	if( stringList[0].length() > 0 )
		positionIndex = atoi( stringList[0].c_str() );

	if( stringList[1].length() > 0 )
		texCoordIndex = atoi( stringList[1].c_str() );

	if( stringList[2].length() > 0 )
		normalIndex = atoi( stringList[2].c_str() );

	return true;
}

void ObjFormat::PopulateVector( const StringArray* stringArray, int startIndex, Vector& vector )
{
	vector.Set( 0.0, 0.0, 0.0 );
	int j = 0;
	double* component = &vector.x;
	for( int i = startIndex; i < ( signed )stringArray->size() && j < 3; i++ )
		component[ j++ ] = atof( ( *stringArray )[i].c_str() );
}

// FileFormat.cpp