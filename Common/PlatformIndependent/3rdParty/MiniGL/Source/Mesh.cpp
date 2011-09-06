#include "Mesh.h"
#include <cmath>
#include <cassert>
#include <cstdio>


namespace MiniGL
{

	
Mesh::Mesh(GLsizei numberOfVertices, GLenum drawMode,  bool hasNormals, bool hasTextureCoordinates)
: SceneNode()
{	
	_drawMode = drawMode;
	_numberOfVertices = numberOfVertices;
	_numberOfIndicies = 0;
	_numberOfTextureHandles = 0;
	_normalDataOffset = 0;
	_uvDataOffset = 0;
	_dataFloatStride = 3;
	
	if (hasNormals)
	{
		_dataFloatStride += 3;
		_normalDataOffset = 3;
	}
	
	if (hasTextureCoordinates)
	{
		_dataFloatStride += 2;
		_uvDataOffset = (hasNormals ? 6 : 3);
	}
	
	_dataVBO = 0;
	_indexVBO = 0;
	
	_data = new GLfloat[numberOfVertices * _dataFloatStride];
	_indexData = 0;
	_textureHandle = 0;
}

	
Mesh::~Mesh()
{
	delete[] _data;
	if (hasIndexData()) delete[] _indexData;
	if (hasTextureHandles()) delete[] _textureHandle;
}    
	

void Mesh::setShaderAttributeVertexArrayHandle(GLuint handle)
{
	_shaderAttributeVertexArrayHandle = handle;
}


void Mesh::setShaderAttributeNormalArrayHandle(GLuint handle)
{
	_shaderAttributeNormalArrayHandle = handle;
}


void Mesh::setShaderAttributeTexCoordArrayHandle(GLuint handle)
{
	_shaderAttributeTexCoordArrayHandle = handle;
}

	
void Mesh::circleTable(float **sint, float **cost, const int n)
{
	int i;
	const int size = fabs( (double)n );
	float angle;
	
	assert( n );
	angle = 2 * M_PI / ( float )n;
	
	*sint = ( float * )calloc( sizeof( float ), size + 1 );
	*cost = ( float * )calloc( sizeof( float ), size + 1 );
	
	if( !( *sint ) || !( *cost ) )
	{
		free( *sint );
		free( *cost );
		printf("Failed to allocate memory in circleTable\n");
	}
	
	for( i = 0; i < size; i++ )
	{
		( *sint )[ i ] = sin( angle * i );
		( *cost )[ i ] = cos( angle * i );
	}
	
	/* Last sample is duplicate of the first */
	( *sint )[ size ] = ( *sint )[ 0 ];
	( *cost )[ size ] = ( *cost )[ 0 ];
}

	
//void Mesh::generateObjectLinearTextureCoords(GLfloat sx, GLfloat sy, GLfloat sz, GLfloat sw, GLfloat tx, GLfloat ty, GLfloat tz, GLfloat tw) 
//{
//	if (_uvs == NULL)
//	{
//		_uvs = new GLfloat[_numberOfVertices*2];
//	}
//	
//	unsigned int i;
//	for (i =0; i < _numberOfVertices; i++)
//	{
//		_uvs[i*2+0] = _vertices[i*3+0] * sx + _vertices[i*3+1] * sy + _vertices[i*3+2] * sz;
//		_uvs[i*2+1] = _vertices[i*3+0] * tx + _vertices[i*3+1] * ty + _vertices[i*3+2] * tz; 
//	}
//}

	
bool Mesh::hasNormalData()
{
	return (_normalDataOffset != 0);
}

	
bool Mesh::hasUVData()
{
	return (_uvDataOffset != 0);
}

	
bool Mesh::hasIndexData()
{
	return ((_numberOfIndicies != 0) && (_indexData != 0));
}	


bool Mesh::hasTextureHandles()
{
	return ((_numberOfTextureHandles != 0) && (_textureHandle != 0));
}

	
void Mesh::setVertex(int index, GLfloat x, GLfloat y, GLfloat z)
{
	if (!(index < _numberOfVertices))
	{
		printf("Mesh error. Invalid vertices index.\n");
		return;
	}
	
	_data[index * _dataFloatStride + 0] = x;
	_data[index * _dataFloatStride + 1] = y;
	_data[index * _dataFloatStride + 2] = z;
}


void Mesh::getVertex(int index, GLfloat &x, GLfloat &y, GLfloat &z)
{
	x = _data[index * _dataFloatStride + 0];
	y = _data[index * _dataFloatStride + 1];
	z = _data[index * _dataFloatStride + 2];
}

	
void Mesh::setNormal(int index, GLfloat x, GLfloat y, GLfloat z)
{
	if (!((index < _numberOfVertices) && this->hasNormalData()))
	{
		printf("Mesh error. Invalid normal index.\n");
		return;
	}
	
	_data[index * _dataFloatStride + _normalDataOffset + 0] = x;
	_data[index * _dataFloatStride + _normalDataOffset + 1] = y;
	_data[index * _dataFloatStride + _normalDataOffset + 2] = z;
}

	
void Mesh::setUV(int index, GLfloat u, GLfloat v)
{
	if (!((index < _numberOfVertices) && this->hasUVData()))
	{
		printf("Mesh error. Invalid uvs index.\n");
		return;
	}
	
	_data[index * _dataFloatStride + _uvDataOffset + 0] = u;
	_data[index * _dataFloatStride + _uvDataOffset + 1] = v;
}


void Mesh::createTextureHandleArray(GLsizei numberOfTextureHandles)
{
	_numberOfTextureHandles = numberOfTextureHandles;
	_textureHandle =  new GLuint[_numberOfTextureHandles];
}


void Mesh::setTextureHandle(GLsizei index, GLuint textureHandle)
{
	if (!((index < _numberOfTextureHandles) && this->hasTextureHandles()))
	{
		printf("Mesh error. Invalid index.\n");
		return;
	}
	
	_textureHandle[index] = textureHandle;
}


void Mesh::createIndexArray(GLsizei numberOfIndicies)
{
	_numberOfIndicies = numberOfIndicies;
	_indexData =  new GLshort[_numberOfIndicies];
}
	
	
void Mesh::setIndex(GLsizei index, GLsizei vertexPosition)
{
	if (!((index < _numberOfIndicies) && this->hasIndexData()))
	{
		printf("Mesh error. Invalid index.\n");
		return;
	}
	
	_indexData[index] = vertexPosition;
}
	
	
GLenum Mesh::getDrawMode()
{
	return _drawMode;
}


void Mesh::setDrawMode(GLenum drawMode)
{
	_drawMode = drawMode;
}

	
GLuint Mesh::getNumberOfVertices()
{
	return _numberOfVertices;
}

	
void Mesh::createVertexBufferObjects()
{
	_dataVBO = new GLuint[1];
	
	glGenBuffers(1, _dataVBO);
	
	unsigned int uiSize = _numberOfVertices * (sizeof(GLfloat) * _dataFloatStride);
	
	glBindBuffer(GL_ARRAY_BUFFER, _dataVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, uiSize, _data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
	
	
void Mesh::createIndexBufferObjects()
{
	_indexVBO = new GLuint[1];
	
	glGenBuffers(1, _indexVBO);
	
	unsigned int uiSize = _numberOfIndicies * sizeof(GLshort);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexVBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, uiSize, _indexData, GL_STATIC_DRAW);
}


void Mesh::render()
{
	renderWithIndexBuffer(GL_FALSE, 0, 0, 0, _numberOfVertices, 0);
}


void Mesh::renderWithIndexBuffer(
	GLboolean drawWithIndicies,
	GLsizei indexBuffer,
	GLsizei numberOfIndicies,
	GLsizei vertexOffset,
	GLsizei numberOfVerticies,
	int textureID)
{
	// bind the VBO for the mesh
	glBindBuffer(GL_ARRAY_BUFFER, _dataVBO[0]);
	
	// bind the index buffer, won't hurt if the handle is 0
	if (drawWithIndicies)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexVBO[indexBuffer]);
	}
	else
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// Enable the vertex attribute arrays
	glEnableVertexAttribArray(_shaderAttributeVertexArrayHandle);
	glVertexAttribPointer(_shaderAttributeVertexArrayHandle, 3, GL_FLOAT, GL_FALSE,
		_dataFloatStride * sizeof(GLfloat),
		(void *)(vertexOffset * _dataFloatStride * sizeof(GLfloat)));
	
	if (hasNormalData())
	{
		glEnableVertexAttribArray(_shaderAttributeNormalArrayHandle);
		glVertexAttribPointer(_shaderAttributeNormalArrayHandle, 3, GL_FLOAT, GL_FALSE,
			_dataFloatStride * sizeof(GLfloat),
			(void *)(_normalDataOffset * sizeof(GLfloat) + vertexOffset * _dataFloatStride * sizeof(GLfloat)));
	}
	
	if (hasUVData() && hasTextureHandles() && (textureID < _numberOfTextureHandles))
	{
		glBindTexture(GL_TEXTURE_2D, _textureHandle[textureID]);
		glEnableVertexAttribArray(_shaderAttributeTexCoordArrayHandle);
		glVertexAttribPointer(_shaderAttributeTexCoordArrayHandle, 2, GL_FLOAT, GL_FALSE,
			_dataFloatStride * sizeof(GLfloat),
			(void *)(_uvDataOffset * sizeof(GLfloat) + vertexOffset * _dataFloatStride * sizeof(GLfloat)));
	}
		
	if (drawWithIndicies)
	{
		glDrawElements(_drawMode, numberOfIndicies, GL_UNSIGNED_SHORT, 0);
	}
	else
	{
		glDrawArrays(_drawMode, 0, numberOfVerticies);
	}
	
	// Safely disable the vertex attribute arrays
	glDisableVertexAttribArray(_shaderAttributeVertexArrayHandle);
	glDisableVertexAttribArray(_shaderAttributeNormalArrayHandle);
	glDisableVertexAttribArray(_shaderAttributeTexCoordArrayHandle);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	
}


} // namespace MiniGL