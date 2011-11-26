/*
 *  Copyright (c) 2011, Lars Schneider
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  Redistributions of source code must retain the above copyright notice, this
 *  list of conditions and the following disclaimer.
 *  Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "AppCore.h"
#include "APIFactory.h"
#include "AppConfig.h"
#include "DebugConfig.h"
#include "PerformanceTests.h"
#include "Octree.h"

#if IMPORT_STATIC_POINT_CLOUD == 1
//#include "ImportHelper.h"
#endif

#if DIRECT_VBO
static int bufferID = 0;
#endif


namespace WVSClientCommon
{


AppCore::AppCore(MiniGL::Camera* const camera) : 
	_camera(camera),
	_isNewDataAvialable(false),
	_renderQuality(RENDER_QUALITY_COARSE_ADJUSTMENT),
	_isUserInputMode(false),
	_invalidDataCount(0),
	_voxelScreensizeCircumcircleRadius(NULL),
	_newDataQueueCount(0)
{
	APIFactory::GetInstance().setupResourcePath();
	APIFactory::GetInstance().createLocks(LOCK_COUNT);

	bool isStaticPointCloud = false;
	char backingStoreFile[2048];
#if ((USE_STATIC_POINT_CLOUD == 1) && (IMPORT_STATIC_POINT_CLOUD == 0))
	isStaticPointCloud = true;

	#if (TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE)
		APIFactory::GetInstance().getResourcePathASCII(backingStoreFile, 2048, STATIC_POINT_CLOUD_FILE);
	#else
        strncpy(backingStoreFile, "/Users/slx/Code/MasterThesis/Assets/PointClouds/lucy_and_manuscript.dat", 2048);
	#endif
#else
	APIFactory::GetInstance().getCachePathASCII(backingStoreFile, 2048, OCTREE_BACKING_STORE_FILENAME);
#endif

	_viewFrustum = new MiniGL::ViewFrustum(_camera);
	_viewFrustum->updateCameraViewParameterCache();
	_octree = new Octree(	GPU_MAX_POINTS, 
							backingStoreFile, 
							isStaticPointCloud, 
							_viewFrustum,
							this,
							&AppCore::renderCallbackHandler);
							
#if USE_VOXEL_ACCU
	_octree->setVoxelAccuThreshold(OCTREE_POINT_ACCU_THRESHOLD);
#endif
		
#if IMPORT_STATIC_POINT_CLOUD

	WVSPoint p;
	p.radius = 0.001f;
	p.color.red = 255;
	p.color.green = 0;
	p.color.blue = 0;
	p.position.x = 0;
	p.position.y = 0;
	p.position.z = 0;
	_octree->addPoint(&p);
//	
////		p.color.red = 255;
////	p.color.green = 0;
////	p.color.blue = 0;
////	p.position.x = 16;
////	p.position.y = 0;
////	p.position.z = 0;
////	_octree->addPoint(&p, false);
////
////bool s = false;
////	
//
//
//			p.position.x = 0;
//			p.position.y = 6.0;
//			p.position.z = 0;
//			
//			_octree->addPoint(&p, false);
//			
//			
//						p.position.x = 0;
//			p.position.y = 4.0f;
//			p.position.z = 0;
//			
//			_octree->addPoint(&p, false);
//			
//				p.position.x = 0;
//			p.position.y = 8.0;
//			p.position.z = 0;
//			
//			_octree->addPoint(&p, false);
//			
//							p.position.x = 0;
//			p.position.y = 0.0;
//			p.position.z = 0;
//			
//			_octree->addPoint(&p, false);
//			
//			
//	for (float x = 0.0f; x < 320.0f; x += 1.0f)
//	{
//		for (float z = 0.0f; z < 620.0f; z += 1.0f)
//		{
//			p.position.x = x;
//			p.position.y = z;
//			p.position.z = 0;
//			
//			_octree->addPoint(&p, false);
//		}
//	}
//	
//	_octree->printStatistics();

//	ImportHelper::ImportPLYFormat(_octree, "/Volumes/Data/MasterThesis/PointCloudSource/lucy_gedreht.ply", 343, 14027872);
//	ImportHelper::ImportPLYFormat(_octree, "/Volumes/Data/MasterThesis/PointCloudSource/manuscript.ply", 341, 2155617);

//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/41717.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/41718.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/41719.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/41720.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/41721.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/41722.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/41817.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/41818.tfw-color.xyzrgba");


//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/41819.tfw-color.xyzrgba");


//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/41820.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/41821.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/41822.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/41917.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/41918.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/41919.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/41920.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/41921.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/41922.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/42017.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/42018.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/42019.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/42020.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/42021.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/42022.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/42117.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/42118.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/42119.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/42120.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/42121.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/42122.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/42217.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/42218.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/42219.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/42220.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/42221.tfw-color.xyzrgba");
//ImportHelper::ImportRicoFormat(_octree,"/Volumes/Data/MasterThesis/Berlin/AlexUndHBF/42222.tfw-color.xyzrgba");
//	ImportHelper::ImportCTFormat(_octree, "/CTModell-Binary-8bit-256x512x171.raw");
//ImportHelper::ImportPLYFormat(_octree, "/abc.ply", 335, 2741093);
//
//	_octree->saveToDisk("/neu.dat");
//	exit(0);
#endif

}


AppCore::~AppCore()
{
	//TODO: glDeleteTextures
	unloadShaderProgram(_textureShaderProgram);
	unloadShaderProgram(_pointShaderProgram);
	
	//delete[] _pointsToRender;
	delete[] _voxelScreensizeCircumcircleRadius;
	
	delete _octree;
	delete _camera;
	delete _viewFrustum;
	delete _skybox;
}


void AppCore::initOpenGL()
{
	APIFactory::GetInstance().lock(OPENGL_LOCK);

#if _WIN32
	glewInit();
#endif

	// Setup OpenGL (ES)
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
#ifdef OPENGL
	// Enable OpenGL settings (not necessary with OpenGL ES)
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
//	if (EWA_POINT_SPLATTING)
//	{
//		glEnable(GL_POINT_SPRITE);
//		glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
//	}
#endif

	// Setup Shader
	loadSimplePointSplattingShaderProgram(_pointShaderProgram);
	loadBlurShaderProgram(_blurShaderProgram);
//	loadPostProcessingShaderProgram(_postProcessingShaderProgram);
	loadTextureShaderProgram(_textureShaderProgram);	
	
	// Setup point VBO
	_pointsToRenderCount = 0;
	
#if DIRECT_VBO
	#if DIRECT_VBO_DOUBLE_BUFFERED	
	int buffers = 2;
	#else
	int buffers = 1;
	#endif
	glGenBuffers(buffers, _dataVBO);
	for (int i = 0; i < buffers; ++i)
	{
		glBindBuffer(GL_ARRAY_BUFFER, _dataVBO[i]);
		glBufferData(GL_ARRAY_BUFFER, GPU_POINT_VRAM_MB * 1024 * 1024, NULL, GL_STREAM_DRAW);
	}
#elif NO_VBO
	_gpuBuffer = (uint32_t*)malloc(GPU_POINT_VRAM_MB * 1024 * 1024);
#endif

#if !DISCARD_SKYBOX
	// Setup static images (Skybox)
	_staticTextureCompleteCount = 0;
	glGenTextures(6, &_staticTexture[0]);
	for (int i = 0; i < 6; ++i)
	{
		_isStaticTextureAvailable[i] = false;
		_staticTextureData[i] = NULL;
		_staticTextureEdgeLength[i] = 0;
	}

	_skybox = new MiniGL::Skybox(CAMERA_FAR_PLANE / 2, true, 512, _staticTexture, true);
	_skybox->setShaderAttributeVertexArrayHandle(TEXTURE_SHADER_ATTRIBUTE_VERTEX_ARRAY);
	_skybox->setShaderAttributeNormalArrayHandle(TEXTURE_SHADER_ATTRIBUTE_NORMAL_ARRAY);
	_skybox->setShaderAttributeTexCoordArrayHandle(TEXTURE_SHADER_ATTRIBUTE_TEXCOORD_ARRAY);
	
	// Trigger static image requests
	glEnable(GL_TEXTURE_2D);

	#if STATIC_SKYBOX
		uint32_t edge1, edge2;

		MiniGL::AllocJPEGImageFromFile("/front.jpg", _staticTextureData[0], edge1, edge2, true);
		_isStaticTextureAvailable[0] = true;
		_staticTextureEdgeLength[0] = 512;
		
		MiniGL::AllocJPEGImageFromFile("/right.jpg", _staticTextureData[1], edge1, edge2, true);
		_isStaticTextureAvailable[1] = true;
		_staticTextureEdgeLength[1] = 512;
		
		MiniGL::AllocJPEGImageFromFile("/back.jpg", _staticTextureData[2], edge1, edge2, true);
		_isStaticTextureAvailable[2] = true;
		_staticTextureEdgeLength[2] = 512;
		
		MiniGL::AllocJPEGImageFromFile("/left.jpg", _staticTextureData[3], edge1, edge2, true);
		_isStaticTextureAvailable[3] = true;
		_staticTextureEdgeLength[3] = 512;
		
		MiniGL::AllocJPEGImageFromFile("/top.jpg", _staticTextureData[4], edge1, edge2, true);
		_isStaticTextureAvailable[4] = true;
		_staticTextureEdgeLength[4] = 512;
		
		MiniGL::AllocJPEGImageFromFile("/bottom.jpg", _staticTextureData[5], edge1, edge2, true);
		_isStaticTextureAvailable[5] = true;
		_staticTextureEdgeLength[5] = 512;
	#else
	#endif
#endif

	APIFactory::GetInstance().unlock(OPENGL_LOCK);
	setRenderingRequired();
}


void AppCore::printStatistics() const
{
#if PRINT_STATISTICS
	std::cout << std::endl << "----------------------------------------" << std::endl 
		<< std::endl << ">> Render Quality:" << _renderQuality << std::endl;
	
	_octree->printStatistics();
	
	std::cout << std::endl << ">> GPU Statistics" << std::endl;
	
	if ((_octree->quantPointCount() > 0) && (_pointsToRenderCount > 0))
	{	
		std::cout << "Points on GPU (All): \t" 
			<< (100 * _pointsToRenderCount)/_octree->quantPointCount() << "% (" 
			<< _pointsToRenderCount << ")" << std::endl;
			
		std::cout << "Points per Node: \t" 
			<< _octree->quantPointCount()/_octree->nodeCount() << std::endl;
	}
	else
	{
		std::cout << "No points on GPU." << std::endl;
	}
#endif
}


void AppCore::addPoint(const WVSPoint* const point) const
{
	_octree->addPoint(point);
}


void AppCore::updateScreenSizeRelatedConstants()
{
	APIFactory::GetInstance().lock(OPENGL_LOCK);
	
	// Calc screensize voxel circum circle radius based on screen size and frustum
	// see "Rendering von Punktbasierten Oberflaechen durch Splatting auf der GPU", Koppitz 2009, page 36
	// Attention: Koppitz forumular is wrong! (n missing in denomitator)
	float_t splatsizeFactor = float_t(_camera->getScreenHeight()) / tanf(_camera->getFieldOfViewInRad() / 2.0f);

	if (_voxelScreensizeCircumcircleRadius == NULL)
		_voxelScreensizeCircumcircleRadius = new float_t[OCTREE_LEAF_LEVEL+1];
	
	for (uint8_t i = 0; i <= OCTREE_LEAF_LEVEL; ++i)
		_voxelScreensizeCircumcircleRadius[i] = _octree->getVoxelCircumcircleRadius(i) * splatsizeFactor;

	_octree->updateScreenSizeRelatedConstants();
	
	APIFactory::GetInstance().unlock(OPENGL_LOCK);
}


void AppCore::setRenderingRequired()
{
	APIFactory::GetInstance().lock(OPENGL_LOCK);
	_renderingRequired = true;
	APIFactory::GetInstance().unlock(OPENGL_LOCK);
}


void AppCore::processingOfNewPointDataStarted()
{
	APIFactory::GetInstance().lock(OPENGL_LOCK);
	++_newDataQueueCount;
	APIFactory::GetInstance().unlock(OPENGL_LOCK);
}


void AppCore::processingOfNewPointDataCompleteted()
{
	APIFactory::GetInstance().lock(WVS_CODEC_LOCK);
	// Reset invalid data count in case we receive valid data
	_invalidDataCount = 0;
	APIFactory::GetInstance().unlock(WVS_CODEC_LOCK);
	APIFactory::GetInstance().lock(OPENGL_LOCK);
	_isNewDataAvialable = true;
	--_newDataQueueCount;
	APIFactory::GetInstance().unlock(OPENGL_LOCK);
	logInfo("\n----------------------------------------\n");
	logInfo(">> Requested WVS Layers processed.");
	logInfo("   New points available.");
}


void AppCore::processingOfNewPointDataCompletetedWithError()
{
	APIFactory::GetInstance().lock(OPENGL_LOCK);
	--_newDataQueueCount;
	APIFactory::GetInstance().unlock(OPENGL_LOCK);
	APIFactory::GetInstance().showMessage("Warning", "Zero voxels received.");
}


void AppCore::invalidDataReceived()
{
	APIFactory::GetInstance().lock(WVS_CODEC_LOCK);
	_invalidDataCount++;
	// Show error message after 3 consecutive errors 
	if (_invalidDataCount > 3)
	{
		APIFactory::GetInstance().showMessage("Error", "Invalid data received. WVS server down?!");
		_invalidDataCount = 0;
	}
	APIFactory::GetInstance().unlock(WVS_CODEC_LOCK);
}


void AppCore::loadStaticTextureIfAvailable()
{
	for (int i = 0; i < 6; ++i)
	{
		if (_isStaticTextureAvailable[i])
		{	
			APIFactory::GetInstance().lock(OPENGL_LOCK);
				
			glBindTexture(GL_TEXTURE_2D, _staticTexture[i]);
			
			// Starting in iOS 3.1, setting texture filter settings (glTexParameter) before
			// loading texture images (glTexImage2D) allows OpenGL ES to optimize its memory
			// usage and texture load times.
			// see http://developer.apple.com/iphone/library/documentation/3DDrawing/Conceptual/OpenGLES_ProgrammingGuide/OpenGLESPlatforms/OpenGLESPlatforms.html
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
				_staticTextureEdgeLength[i],
				_staticTextureEdgeLength[i], 
				0, GL_RGB, GL_UNSIGNED_BYTE, _staticTextureData[i]);
			free(_staticTextureData[i]);
			
			_isStaticTextureAvailable[i] = false;
			_renderingRequired = true;
			++_staticTextureCompleteCount;
			
			APIFactory::GetInstance().unlock(OPENGL_LOCK);
		}
	}
}


void AppCore::setStaticTexture(const uint8_t textureID, const uint8_t* const imageData, const uint16_t edgeLength)
{
	APIFactory::GetInstance().lock(OPENGL_LOCK);
	
	// Check if texture is already downloaded
	if (!(	(_isStaticTextureAvailable[textureID] == true) || 
			(_staticTextureData[textureID] != NULL) ||
			(imageData == NULL)))
	{
		// Allocate memory for texture and copy texture
		size_t imageSizeInBytes = edgeLength * edgeLength * 3;
		_staticTextureData[textureID] = (uint8_t*)malloc(imageSizeInBytes);
		memcpy(_staticTextureData[textureID], imageData, imageSizeInBytes);
		_staticTextureEdgeLength[textureID] = edgeLength;
		
		_isStaticTextureAvailable[textureID] = true;
	}
	
	APIFactory::GetInstance().unlock(OPENGL_LOCK);
}


void AppCore::preUserInput()
{
	APIFactory::GetInstance().lock(OPENGL_LOCK);
	_isUserInputMode = true;
	_isInteractiveMode = true;
	APIFactory::GetInstance().unlock(OPENGL_LOCK);
}


void AppCore::postUserInput()
{
	APIFactory::GetInstance().lock(OPENGL_LOCK);
	_lastCameraParameterChangeTimestamp = APIFactory::GetInstance().getTimeInMS();
	_currentCameraParameterRequest = 0;
	_renderingRequired = true;
	_isUserInputMode = false;
	APIFactory::GetInstance().unlock(OPENGL_LOCK);
}


void AppCore::mainLoop(void* context, const GLuint* const frameBuffer, 
	const GLuint* const colorBuffer, const GLuint screenWidth, const GLuint screenHeight)
{
	_context = context;	
	
	// Start in interactive mode to restore a few nodes and show something on startup
	_isInteractiveMode = true;
	_lastCameraParameterChangeTimestamp = APIFactory::GetInstance().getTimeInMS();
	
	// Init camera parameter
	((MiniGL::SimpleCamera*)_camera)->setFrustum(screenWidth, screenHeight, 
						CAMERA_FIELD_OF_VIEW_IN_DEGREE,
						CAMERA_NEAR_PLANE,
						CAMERA_FAR_PLANE);
	updateScreenSizeRelatedConstants();

	if (frameBuffer && colorBuffer)
	{
#if POST_PROCESSING
		// Create the color textures
		int fullTextureSize = 512;
		glGenTextures(2, _fbTexture);
		
		glBindTexture(GL_TEXTURE_2D, _fbTexture[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fullTextureSize, fullTextureSize, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		glBindTexture(GL_TEXTURE_2D, _fbTexture[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fullTextureSize, fullTextureSize, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
		
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
		
		// Generate depth buffer
		glGenRenderbuffers(1, &_depthBuffer);
		
		// Generate frame buffers
		glGenFramebuffers(2, _framebuffer);
		
		// Setup first framebuffer (with depth buffer)
		glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer[0]);
		glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, fullTextureSize, fullTextureSize);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fbTexture[0], 0);

		// Setup second framebuffer (without depth buffer)
		glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer[1]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fbTexture[1], 0);
#else
		glGenRenderbuffers(1, &_depthBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, *frameBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBuffer);
		
		glViewport(0, 0, screenWidth, screenHeight);
#endif

		glBindRenderbuffer(GL_RENDERBUFFER, *colorBuffer);
	}
	
	initOpenGL();
			
	while(true)
	{
	
#if POST_PROCESSING
		// Render scene to _fbTexture[0] (with depth buffer)
		glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer[0]);
		glViewport(0, 0, fullTextureSize, fullTextureSize);
		
		float m_fTexelOffset = 1.0f / (float)fullTextureSize;
		// Altered weights for the faster filter kernel 
		float w1 = 0.0555555f;
		float w2 = 0.2777777f;
		float intraTexelOffset = (w2 / (w1 + w2)) * m_fTexelOffset;
		m_fTexelOffset += intraTexelOffset;

		if (render())
		{
			// Render _fbTexture[0] to _fbTexture[1]
			// Apply horizontal  *  POSSIBILITY OF SUCH DAMAGE.
			glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer[1]);
			glViewport(0, 0, fullTextureSize, fullTextureSize);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _fbTexture[0]);
			
			glUseProgram(_blurShaderProgram.handle);
			glUniform1f( *  POSSIBILITY OF SUCH DAMAGE._SHADER_UNIFORM_TEXEL_OFFSET_X, m_fTexelOffset);
			glUniform1f( *  POSSIBILITY OF SUCH DAMAGE._SHADER_UNIFORM_TEXEL_OFFSET_Y, 0.0f);	
			renderFullscreenTexture();
			
			// Render _fbTexture[1] to color buffer
			// Apply vertical  *  POSSIBILITY OF SUCH DAMAGE.
			glBindFramebuffer(GL_FRAMEBUFFER, *frameBuffer);
			glViewport(0, 0, screenWidth, screenHeight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _fbTexture[1]);
			
			glUseProgram(_blurShaderProgram.handle);
			glUniform1f(_blurShaderProgram.uniforms[ *  POSSIBILITY OF SUCH DAMAGE._SHADER_UNIFORM_TEXEL_OFFSET_X], 0.0f);
			glUniform1f(_blurShaderProgram.uniforms[ *  POSSIBILITY OF SUCH DAMAGE._SHADER_UNIFORM_TEXEL_OFFSET_Y], m_fTexelOffset);	
			renderFullscreenTexture();
	
			APIFactory::GetInstance().presentRenderBuffer(_context);
		}
#else
		if (render()) APIFactory::GetInstance().presentRenderBuffer(_context);
#endif
		
		// Restore nodes from backing store if in interactive mode
		if (_isInteractiveMode) _octree->restoreNodes(_nodeRestoreQuota);
	}
}


void AppCore::renderFullscreenTexture()
{
	glDisable(GL_DEPTH_TEST);

	// Enable vertex arributes
	glEnableVertexAttribArray(POST_PROCESSING_SHADER_ATTRIBUTE_VERTEX_ARRAY);
	glEnableVertexAttribArray(POST_PROCESSING_SHADER_ATTRIBUTE_TEXCOORD_ARRAY);

	const float afVertexData[] = { -1.0f, -1.0f, +1.0f, -1.0f,  
								   -1.0f, +1.0f, +1.0f, +1.0f };

	const float afTexCoordData[] = { 0.0f, 0.0f, 1.0f, 0.0f,  
									 0.0f, 1.0f, 1.0f, 1.0f };
									 
	glVertexAttribPointer(POST_PROCESSING_SHADER_ATTRIBUTE_VERTEX_ARRAY, 2, GL_FLOAT, GL_FALSE, 0, afVertexData);
	glVertexAttribPointer(POST_PROCESSING_SHADER_ATTRIBUTE_TEXCOORD_ARRAY, 2, GL_FLOAT, GL_FALSE, 0, afTexCoordData);

	// Draw the quad
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Disable vertex arributes
	glDisableVertexAttribArray(POST_PROCESSING_SHADER_ATTRIBUTE_VERTEX_ARRAY);
	glDisableVertexAttribArray(POST_PROCESSING_SHADER_ATTRIBUTE_TEXCOORD_ARRAY);

	// Disable blending
	glEnable(GL_DEPTH_TEST);
}


bool AppCore::render()
{	
#if !DISCARD_SKYBOX
	// Try to load textures if not already completed
	if (_staticTextureCompleteCount <= 6) loadStaticTextureIfAvailable();
#endif

	APIFactory::GetInstance().lock(OPENGL_LOCK);
	
	double timestamp = APIFactory::GetInstance().getTimeInMS();
	
#if !(RENDER_OCTREE_DEBUG_VOXELS || USE_STATIC_POINT_CLOUD)
	if		(   !(_currentCameraParameterRequest & 1) && 
				 (timestamp - _lastCameraParameterChangeTimestamp > WVS_RELOAD_DELAY[0]))
	{
		// Request 1/4 of screen resolution from WVS
		requestPointCloudForCurrentView(4);
		_currentCameraParameterRequest |= 1;
	}
	else if	(   !(_currentCameraParameterRequest & 2) && 
				 (timestamp - _lastCameraParameterChangeTimestamp > WVS_RELOAD_DELAY[1]))
	{
		// Request full screen resolution from WVS
		requestPointCloudForCurrentView(1);
		_currentCameraParameterRequest |= 3;
	}
#endif

	if (_isUserInputMode) _renderingRequired = true;
	if (_renderQuality < 1.0f) _renderingRequired = true;

#if !(RENDER_OCTREE_DEBUG_VOXELS || USE_STATIC_POINT_CLOUD)
	if (_isNewDataAvialable)
	{
		_renderingRequired = true;
		_isNewDataAvialable = false;
	}
#endif

	if (!_renderingRequired)
	{
		APIFactory::GetInstance().unlock(OPENGL_LOCK);
		APIFactory::GetInstance().processUserInterfaceEvents();
		return false;
	}
	_renderingRequired = false;
	
	// Updates the view frustum that is used for culling etc. based on the current
	// camera parameters
	_viewFrustum->updateCameraViewParameterCache();
	
	// Clear context
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
#if !DISCARD_SKYBOX
	// Render Skybox and ground plane
	if (_staticTextureCompleteCount > 0)
	{
		glUseProgram(_textureShaderProgram.handle);

		glUniformMatrix4fv(	_textureShaderProgram.uniforms[SHADER_UNIFORM_VIEW_PROJECTION_MATRIX],
							1, GL_FALSE, _viewFrustum->getViewProjectionMatrixCachePtr()->f);
							
		MATRIX model;
		MatrixTranslation(model,	_viewFrustum->getCameraPostionCachePtr()->x,
									_viewFrustum->getCameraPostionCachePtr()->y,
									_viewFrustum->getCameraPostionCachePtr()->z);
		glUniformMatrix4fv(	_textureShaderProgram.uniforms[SHADER_UNIFORM_MODEL_MATRIX],
							1, GL_FALSE, model.f);
		
		_skybox->render();
	}
#endif

	// Render points
    glUseProgram(_pointShaderProgram.handle);

	glUniform3fv(	_pointShaderProgram.uniforms[POINT_SHADER_UNIFORM_REGION_ORIGIN],
					OCTREE_LEAF_LEVEL + 1, _octree->getRegionOrginArray());
	
	glUniform1fv(	_pointShaderProgram.uniforms[POINT_SHADER_UNIFORM_VOXEL_INCIRCLE_DIAMETER],
					OCTREE_LEAF_LEVEL + 1,
					_octree->getVoxelIncircleDiameterArray());

	glUniform1fv(	_pointShaderProgram.uniforms[POINT_SHADER_UNIFORM_NODE_INCIRCLE_DIAMETER],
					OCTREE_LEAF_LEVEL + 1,
					_octree->getNodeIncircleDiameterArray());
					
	glUniform1fv(	_pointShaderProgram.uniforms[POINT_SHADER_UNIFORM_VOXEL_SCREENSIZE_CIRCUMCIRCLE_RADIUS],
					OCTREE_LEAF_LEVEL + 1,
					_voxelScreensizeCircumcircleRadius);
						
	// Set camera position (necessary for backface culling)
	glUniform3fv(	_pointShaderProgram.uniforms[POINT_SHADER_UNIFORM_CAMERA_POSITION],
					1, (GLfloat*)(_viewFrustum->getCameraPostionCachePtr()));
					
	glUniformMatrix4fv(	_pointShaderProgram.uniforms[POINT_SHADER_UNIFORM_PROJECTION_MATRIX],
						1, GL_FALSE, _viewFrustum->getProjectionMatrixCachePtr()->f);
						
	glUniformMatrix4fv(	_pointShaderProgram.uniforms[POINT_SHADER_UNIFORM_VIEW_MATRIX],
						1, GL_FALSE, _viewFrustum->getViewMatrixCachePtr()->f);
												

#if COMPACT_NODE_REGION_ENCODING
	const uint32_t regionVoxelFactor = 1;
	const uint8_t regionLevelStride = 4;
	const uint32_t regionLevelDataType = GL_UNSIGNED_BYTE;
#else
	const uint32_t regionVoxelFactor = 2;
	const uint8_t regionLevelStride = 8;
	const uint32_t regionLevelDataType = GL_UNSIGNED_SHORT;
#endif

#if DIRECT_VBO
	#if DIRECT_VBO_DOUBLE_BUFFERED	
	if (bufferID == 0) bufferID = 1; else bufferID = 0;
	#endif

	glBindBuffer(GL_ARRAY_BUFFER, _dataVBO[bufferID]);
	uint32_t* const gpuBuffer = (uint32_t*)glMapBufferOES(GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES);

	uint32_t* const regionLevelBuffer = gpuBuffer;
	uint32_t* const voxelBuffer = gpuBuffer + GPU_MAX_POINTS * regionVoxelFactor;
	
	glVertexAttribPointer(	POINT_SHADER_ATTRIBUTE_DATA1,
							4,
							GL_UNSIGNED_BYTE,
							GL_FALSE,
							0,
							(void *)(GPU_MAX_POINTS * regionLevelStride));
							
	glVertexAttribPointer(	POINT_SHADER_ATTRIBUTE_DATA2,
							4,
							regionLevelDataType,
							GL_FALSE,
							0,
							0);	

	glEnableVertexAttribArray(POINT_SHADER_ATTRIBUTE_DATA1);	
	glEnableVertexAttribArray(POINT_SHADER_ATTRIBUTE_DATA2);

	// Traverse octree, copy points into buffer and call rendering callback 
	_octree->copyPointsToBuffer(
		_renderQuality,
		!_isInteractiveMode,
		regionLevelBuffer,
		voxelBuffer,
		&_pointsToRenderCount,
		&_renderingRequired);
	
	glDisableVertexAttribArray(POINT_SHADER_ATTRIBUTE_DATA1);
	glDisableVertexAttribArray(POINT_SHADER_ATTRIBUTE_DATA2);
	
	glUnmapBufferOES(GL_ARRAY_BUFFER);
		
#elif NO_VBO
	uint32_t* const regionLevelBuffer = _gpuBuffer;
	uint32_t* const voxelBuffer = _gpuBuffer + GPU_MAX_POINTS * regionVoxelFactor;
	
	glVertexAttribPointer(	POINT_SHADER_ATTRIBUTE_DATA1, 
							4,
							GL_UNSIGNED_BYTE,
							GL_FALSE,
							4,
							voxelBuffer);
							
	glVertexAttribPointer(	POINT_SHADER_ATTRIBUTE_DATA2,
							4,
							regionLevelDataType,
							GL_FALSE,
							regionLevelStride,
							regionLevelBuffer);	

	glEnableVertexAttribArray(POINT_SHADER_ATTRIBUTE_DATA1);	
	glEnableVertexAttribArray(POINT_SHADER_ATTRIBUTE_DATA2);
	
	// Traverse octree, copy points into buffer and call rendering callback 	
	_octree->copyPointsToBuffer(
		_renderQuality,
		!_isInteractiveMode,
		regionLevelBuffer,
		voxelBuffer,
		&_pointsToRenderCount,
		&_renderingRequired);	

	glDisableVertexAttribArray(POINT_SHADER_ATTRIBUTE_DATA1);
	glDisableVertexAttribArray(POINT_SHADER_ATTRIBUTE_DATA2);	
	
#endif
		
#ifdef OPENGL_ES
	const GLenum discard_attachments[] = { GL_DEPTH_ATTACHMENT };
	glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, discard_attachments);
#endif

	if (_renderingRequired & Octree::OCTREE_RENDERING_CANCELED)
	{
		APIFactory::GetInstance().unlock(OPENGL_LOCK);
		
		//TODO: In case we overestimate the render quality by far this becomes a stupid loop
		_octree->estimateInteractiveRenderQuality(&_renderQuality, OCTREE_INTERACTIVE_RENDERING_POINT_THRESHOLD);
		return false;
	}

#if BENCHMARK_1_MILLION
	if (_pointsToRenderCount >= 1000000)
	{
		_renderingRequired = true;
		double time = APIFactory::GetInstance().getTimeInMS();
		printf("%f\n", time - _lastFrame);
		_lastFrame = time;
	}
#endif

	if (_isUserInputMode || ((timestamp - _lastCameraParameterChangeTimestamp) < 500))
	{
		// Within 500ms we consider any input as movement
		double_t renderingTimeInMS = APIFactory::GetInstance().getTimeInMS() - timestamp;
		if (renderingTimeInMS > (1000.0 / MINIMUM_FRAMERATE))
		{
			// Time to render last frame took longer than target framerate.
			// Lower render quality to speed up framerate.
			_renderQuality -= RENDER_QUALITY_FINE_ADJUSTMENT;
			_nodeRestoreQuota = 0;
		}
		else if (renderingTimeInMS < (1000.0 / MAXIMUM_FRAMERATE))
		{
			_renderQuality += RENDER_QUALITY_FINE_ADJUSTMENT;
			_nodeRestoreQuota = 32;
		}
		
		_renderingRequired = true;
	}
	else
	{
		// No movement with in the last 500ms. Increase render quality
		_renderQuality += RENDER_QUALITY_COARSE_ADJUSTMENT;
		_isInteractiveMode = false;
	}

	// Check range of render quality
	if (_renderQuality > 1.0f) _renderQuality = 1.0f;
	else if (_renderQuality < RENDER_QUALITY_FINE_ADJUSTMENT)
		_renderQuality = RENDER_QUALITY_FINE_ADJUSTMENT;
	
	APIFactory::GetInstance().unlock(OPENGL_LOCK);
	
	return true;
}


void AppCore::renderCallbackHandler(const uint32_t pointsToRenderCount, bool* const abortRendering) const
{
	// Send points to GPU
	if (pointsToRenderCount > 0) glDrawArrays(GL_POINTS, 0, pointsToRenderCount);

	// Process user input
	APIFactory::GetInstance().unlock(OPENGL_LOCK);
	APIFactory::GetInstance().processUserInterfaceEvents();
	APIFactory::GetInstance().lock(OPENGL_LOCK);
	
	// Abort multi batch rendering in interactive mode
	// Reason: We assume we can't renderer more than one batch with an approriate framerate
	*abortRendering = _isInteractiveMode;
}


} // namespace WVSClientCommon
