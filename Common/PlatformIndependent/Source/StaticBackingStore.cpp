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