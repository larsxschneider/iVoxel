#ifndef _MINIGL_MESH_H_
#define _MINIGL_MESH_H_

#include "CrossPlatformHelper.h"
#include "SceneNode.h"


namespace MiniGL
{
		
		
class Mesh : public SceneNode
{

	GLfloat*		_data;
	GLshort*		_indexData;
	GLuint*			_textureHandle;
	
	GLuint			_dataFloatStride,
					_normalDataOffset,
					_uvDataOffset,
					_shaderAttributeVertexArrayHandle,
					_shaderAttributeNormalArrayHandle,
					_shaderAttributeTexCoordArrayHandle;
					
	GLsizei			_numberOfVertices,
					_numberOfIndicies,
					_numberOfTextureHandles;
					
	GLenum			_drawMode;
	
protected:
	
	static void circleTable(float **sint, float **cost, const int n);
	
	void createVertexBufferObjects();
	void createIndexBufferObjects();
	
	void createIndexArray(GLsizei numberOfIndicies);
	void createTextureHandleArray(GLsizei numberOfTextureHandles);
	
	void setIndex(GLsizei index, GLsizei vertexPosition);
	void setVertex(int index, GLfloat x, GLfloat y, GLfloat z);
	void setNormal(int index, GLfloat x, GLfloat y, GLfloat z);
	void setUV(int index, GLfloat u, GLfloat v);
	void setTextureHandle(GLsizei index, GLuint textureHandle);
	
	void getVertex(int index, GLfloat &x, GLfloat &y, GLfloat &z);
	
	bool hasIndexData();
	bool hasNormalData();
	bool hasUVData();
	bool hasTextureHandles();
	
	GLenum getDrawMode();
	void setDrawMode(GLenum drawMode);
	
public:
		
	Mesh(GLsizei numberOfVertices, GLenum drawMode, bool hasNormals, bool hasTextureCoordinates);
	virtual ~Mesh();
	
	GLuint getNumberOfVertices();
	
	void render();
	void renderWithIndexBuffer(
		GLboolean drawWithIndicies,
		GLsizei indexBuffer,
		GLsizei numberOfIndicies,
		GLsizei vertexOffset,
		GLsizei numberOfVerticies,
		int textureID);
	
	void setShaderAttributeVertexArrayHandle(GLuint handle);
	void setShaderAttributeNormalArrayHandle(GLuint handle);
	void setShaderAttributeTexCoordArrayHandle(GLuint handle);
};
	
	
} // namespace MiniGL

#endif // _MINIGL_MESH_H_