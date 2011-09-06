/*
 *  BackingStore.h
 *  MacApp
 *
 *  Created by Lars Schneider on 07.08.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
 *
 */

#ifndef BACKINGSTORE_H
#define BACKINGSTORE_H


#include <stddef.h>
#include <stdint.h>
#include "Octree.h"


namespace WVSClientCommon
{


class BackingStore
{

protected:
	FILE*						_file;

public:
	BackingStore();
	~BackingStore();
	
	bool init(const char* const filename);
	bool close() const;

	virtual bool readNode(const long position, Octree::Node* const outNode) const;
	bool readQuantPointBlock(	long position, 
								const uint32_t numberOfBlocks,
								Octree::QuantPointBlock* const outfirstQuantPointBlock) const;
								
	virtual bool writeNode(	const Octree::Node* const node, 
							long* const outPosition) const;
	
	bool writeNodeToFile(	FILE* const file, 
							const Octree::Node* const node,
							long* const outPosition) const;
};
	

}

#endif