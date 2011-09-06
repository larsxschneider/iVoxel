/*
 *  StaticBackingStore.cpp
 *  MacApp
 *
 *  Created by Lars Schneider on 24.08.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
 *
 */

#include "StaticBackingStore.h"

namespace WVSClientCommon
{


StaticBackingStore::StaticBackingStore(	const size_t maxNodesInMemory,
										const Octree::Node* const firstNodePointer)
	:	_maxNodesInMemory(maxNodesInMemory),
		_firstNodePointer(firstNodePointer),
		BackingStore()
{
	_nodeFilePosition = new long[maxNodesInMemory];
#if DEBUG
	for (size_t i = 0; i < maxNodesInMemory; ++i) _nodeFilePosition[i] = 0;
#endif
}


StaticBackingStore::~StaticBackingStore()
{
	delete[] _nodeFilePosition;
}


bool StaticBackingStore::open(const char* const filename, Octree::Node* const node)
{
	_file = fopen(filename, "rb");
	
	// Read root node
	node->reset();
	fread(node->children, sizeof(Octree::Node*), 8, _file);
	for (uint8_t i=0; i<8; ++i) if (node->children[i] != NULL) node->unsetChildInMemory(i);

	return true; // TODO: File exception handling
}


bool StaticBackingStore::readNode(const long position, Octree::Node* const outNode) const
{
	// Save file position of the node
	size_t index = (outNode - _firstNodePointer);
	
	assert(index >= 0);
	assert(index < _maxNodesInMemory);
	assert(_nodeFilePosition[index] == 0);
	_nodeFilePosition[index] = position;
	assert(_nodeFilePosition[index] != 0);
	
	// Read node
	assert(position != 0);
	return BackingStore::readNode(position, outNode);
}

	
bool StaticBackingStore::writeNode(const Octree::Node* const node, long* const outPosition) const
{
	// Make sure the node has no children that are in memory
	for (uint8_t i = 0; i < 8; ++i) 
		assert(!((node->children[i] != NULL) && (node->isChildInMemory(i))));
		
	// Restore file position of the node
	size_t index = (node - _firstNodePointer);
	assert(index >= 0);
	assert(index < _maxNodesInMemory);
	assert(_nodeFilePosition[index] != 0);
	*outPosition = _nodeFilePosition[index];
#if DEBUG
	_nodeFilePosition[index] = 0;
#endif
	assert(_nodeFilePosition[index] == 0);
	assert(*outPosition != 0);

	// Do *not* write the node to disk (because the node is already there)
	return true;
}


}