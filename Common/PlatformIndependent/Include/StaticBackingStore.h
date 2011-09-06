/*
 *  StaticBackingStore.h
 *  MacApp
 *
 *  Created by Lars Schneider on 24.08.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
 *
 */

#ifndef STATICBACKINGSTORE_H
#define STATICBACKINGSTORE_H


#include <stddef.h>
#include <stdint.h>
#include "BackingStore.h"


namespace WVSClientCommon
{


class StaticBackingStore : public BackingStore
{
	const size_t				_maxNodesInMemory;
	const Octree::Node* const	_firstNodePointer;
	long*						_nodeFilePosition;

public:
	StaticBackingStore(	const size_t maxNodesInMemory,
						const Octree::Node* const firstNodePointer);
	~StaticBackingStore();
	
	bool open(const char* const filename, Octree::Node* const node);
	
	bool readNode(const long position, Octree::Node* const outNode) const;
	bool writeNode(const Octree::Node* const node, long* const outPosition) const;
};
	

}

#endif