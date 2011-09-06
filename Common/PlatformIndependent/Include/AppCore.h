/*
 *  AppCore.h
 *  MacApp
 *
 *  Created by Lars Schneider on 19.05.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
 *
 */
#ifndef APPCORE_H_V2Q20ZVP
#define APPCORE_H_V2Q20ZVP

#include "CrossPlatformHelper.h"
#include "MiniGL.h"
#include "Shader.h"
#include "PointStructs.h"
#include "DebugConfig.h"

namespace WVSClientCommon
{


class Octree;


class AppCore
{
	MiniGL::Camera* const		_camera;

	//ShaderProgram				_postProcessingShaderProgram;
	ShaderProgram				_textureShaderProgram;
	ShaderProgram				_pointShaderProgram;
	ShaderProgram				_blurShaderProgram;
	
	MiniGL::ViewFrustum*		_viewFrustum;
	MiniGL::Skybox*				_skybox;

#if DIRECT_VBO
	GLuint						_dataVBO[2];
#elif NO_VBO
	uint32_t*					_gpuBuffer;
#endif
	GLuint						_fbTexture[2];
	GLuint						_framebuffer[3];
	GLuint						_depthBuffer;
	GLuint						_colorBuffer, defaultFramebuffer;
	
	
	uint32_t					_pointsToRenderCount;
	Octree*						_octree;
	double						_lastFrame;
	
	bool						_isNewDataAvialable;
	uint8_t						_renderingRequired;
	
	float_t*					_voxelScreensizeCircumcircleRadius;
	float_t						_renderQuality;
	double_t					_lastCameraParameterChangeTimestamp;
	uint8_t						_currentCameraParameterRequest;
	uint32_t					_nodeRestoreQuota;
	
	uint8_t						_staticTextureCompleteCount;
	bool						_isStaticTextureAvailable[6];
	uint8_t*					_staticTextureData[6];
	uint16_t					_staticTextureEdgeLength[6];
	GLuint						_staticTexture[6];
	
	uint32_t					_invalidDataCount;
	uint32_t					_newDataQueueCount;
	
	bool						_isUserInputMode;
	bool						_isInteractiveMode;
	void*						_context;
	
	void renderFullscreenTexture();
	void printStatistics() const;
	
public:
	AppCore(MiniGL::Camera* const camera);
	~AppCore();
	
	void initOpenGL();
	
	void preUserInput();
	void postUserInput();

	void updateScreenSizeRelatedConstants();
	bool render();
	void mainLoop(void* context, const GLuint* const frameBuffer, 
		const GLuint* const colorBuffer, const GLuint screenWidth, const GLuint screenHeight);
	
	void setRenderingRequired();
	void processingOfNewPointDataStarted();
	void processingOfNewPointDataCompleteted();
	void processingOfNewPointDataCompletetedWithError();
	void invalidDataReceived();
	
	
	void renderCallbackHandler(const uint32_t pointsToRenderCount, bool* const abortRendering) const;

	/**
	Request arbitrary layers from WVS and returns the image request URL for the color layer
	*/
	void requestPointCloudForCurrentView(const uint8_t fraction = 1) const;
	void addPoint(const WVSPoint* const point) const;
	
	void loadStaticTextureIfAvailable();
	void setStaticTexture(	const uint8_t textureID, 
							const uint8_t* const imageData,
							const uint16_t edgeLength);
};


}

#endif /* end of include guard: APPCORE_H_V2Q20ZVP */
