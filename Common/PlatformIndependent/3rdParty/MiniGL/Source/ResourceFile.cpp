/******************************************************************************

 @File         PVRTResourceFile.cpp

 @Title        

 @Copyright    Copyright (C) 2007 - 2008 by Imagination Technologies Limited.

 @Platform     ANSI compatible

 @Description  Simple resource file wrapper

******************************************************************************/
/*
All changes:
Oolong Engine for the iPhone / iPod touch
Copyright (c) 2007-2008 Wolfgang Engel  http://code.google.com/p/oolongengine/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
#include "ResourceFile.h"
#include <stdio.h>


namespace MiniGL
{


string CPVRTResourceFile::s_ReadPath;

/*!***************************************************************************
@Function			SetReadPath
@Input				pszReadPath The path where you would like to read from
@Description		Sets the read path
*****************************************************************************/
void CPVRTResourceFile::SetReadPath(const char* const pszReadPath)
{
	s_ReadPath = (pszReadPath) ? pszReadPath : "";
}

/*!***************************************************************************
@Function			GetReadPath
@Returns			The currently set read path
@Description		Returns the currently set read path
*****************************************************************************/
string CPVRTResourceFile::GetReadPath()
{
	return string(s_ReadPath);
}

/*!***************************************************************************
@Function			CPVRTResourceFile
@Input				pszFilename Name of the file you would like to open
@Description		Constructor
*****************************************************************************/
CPVRTResourceFile::CPVRTResourceFile(const char* const pszFilename) :
	m_bOpen(false),
	m_Size(0),	m_pData(0)
{
	string Path(s_ReadPath);
	Path += pszFilename;
	
	FILE* pFile = fopen(Path.c_str(), "rb");
	if (pFile)
	{
		// Get the file size
		fseek(pFile, 0, SEEK_END);
		m_Size = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		// read the data, append a 0 byte as the data might represent a string
		char* pData = new char[m_Size + 1];
		pData[m_Size] = '\0';
		size_t BytesRead = fread(pData, 1, m_Size, pFile);

		if (BytesRead != m_Size)
		{
			delete [] pData;
			m_Size = 0;
		}
		else
		{
			m_pData = pData;
			m_bOpen = true;
		}
		fclose(pFile);
	}
}

/*!***************************************************************************
@Function			~CPVRTResourceFile
@Description		Destructor
*****************************************************************************/
CPVRTResourceFile::~CPVRTResourceFile()
{
	Close();
}

/*!***************************************************************************
@Function			IsOpen
@Returns			true if the file is open
@Description		Is the file open
*****************************************************************************/
bool CPVRTResourceFile::IsOpen() const
{
	return m_bOpen;
}

/*!***************************************************************************
@Function			Size
@Returns			The size of the opened file
@Description		Returns the size of the opened file
*****************************************************************************/
size_t CPVRTResourceFile::Size() const
{
	return m_Size;
}

/*!***************************************************************************
@Function			DataPtr
@Returns			A pointer to the file data
@Description		Returns a pointer to the file data
*****************************************************************************/
const void* CPVRTResourceFile::DataPtr() const
{
	return m_pData;
}

/*!***************************************************************************
@Function			StringPtr
@Returns			The file data as a string
@Description		Returns the file as a null-terminated string
*****************************************************************************/
const char* CPVRTResourceFile::StringPtr() const
{
	return m_pData;
}

/*!***************************************************************************
@Function			Close
@Description		Closes the file
*****************************************************************************/
void CPVRTResourceFile::Close()
{
	if (m_bOpen)
	{
		m_bOpen = false;
		m_pData = 0;
		m_Size = 0;
	}
}

} // namespace MiniGL

