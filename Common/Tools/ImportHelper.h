/*
 *  ImportHelper.h
 *  MacApp
 *
 *  Created by Lars Schneider on 30.08.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
 *
 */

#ifndef IMPORT_HELPER_H
#define IMPORT_HELPER_H


namespace WVSClientCommon
{


class Octree;


namespace ImportHelper
{

void ImportCTFormat(Octree* octree, const char* const filename);
void ImportRicoFormat(Octree* octree, const char* const filename);
void ImportPLYFormat(Octree* octree, const char* const filename, const int startbyte, const int vertices);

}


}


#endif // IMPORT_HELPER_H
