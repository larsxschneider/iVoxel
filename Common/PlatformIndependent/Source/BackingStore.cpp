/*
 *  BackingStore.cpp
 *  MacApp
 *
 *  Created by Lars Schneider on 07.08.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
 *
 */

#include "BackingStore.h"
#include <iostream>
#include "DebugConfig.h"
#include <fcntl.h>


namespace WVSClientCommon
{


BackingStore::BackingStore()
{
}


BackingStore::~BackingStore()
{
}


bool BackingStore::init(const char* const filename)
{
	// Init file
	_file = fopen(filename, "wb+");

	// F_NOCACHE tells the system that you don't expect to read that data* off the disk again any
	// time soon, so it shouldn't bother caching it.
	// see http://stackoverflow.com/questions/1945619/help-needed-with-f-nocache-in-mac
	// see http://bethblog.com/index.php/2010/10/29/john-carmack-discusses-rage-on-iphoneipadipod-touch/
	// see http://stackoverflow.com/questions/2299402/how-does-one-do-raw-io-on-mac-os-x-ie-equivalent-to-linuxs-o-direct-flag
	// TODO: profile this setting!
	fcntl(fileno(_file), F_NOCACHE, 1);
	
	// Resevere space for the root node
	uint8_t byte = 0;
	fwrite(&byte, 1, 8*sizeof(Octree::Node*), _file);
	
	return true; // TODO: File exception handling
}


bool BackingStore::close() const
{
	return (fclose(_file) == 0);
}
	
	
bool BackingStore::readNode(const long position, Octree::Node* const outNode) const
{
	outNode->reset();
	
	fseek(_file, position, SEEK_SET);
	fread(outNode->children, sizeof(Octree::Node*), 8, _file);
	fread(&(outNode->quantPointCount), sizeof(uint16_t), 1, _file);
	
	for (uint8_t i = 0; i < 8; ++i) if (outNode->children[i] != NULL) outNode->unsetChildInMemory(i);

	return true; // TODO: File exception handling
}


bool BackingStore::readQuantPointBlock(	long position, 
										const uint32_t numberOfBlocks,
										Octree::QuantPointBlock* const outfirstQuantPointBlock) const
{
			
	long offset =	sizeof(Octree::Node*) * 8
				+	sizeof(uint16_t);
				
	position += offset;
	fseek(_file, position, SEEK_SET);

	Octree::QuantPointBlock* block = outfirstQuantPointBlock;
	for (int i = 0; i < numberOfBlocks; ++i)
	{
		// Make sure a block exists to write into
		assert(block);
		
		// Read file data and write it into block
		fread(block, sizeof(QuantPoint), OCTREE_POINTS_PER_POINT_DATA_BLOCK, _file);
		
		// Jump to the next block
		block = block->next;
	}
	
	// Make sure we reached the last block
	assert(block == NULL);
	
	return true; // TODO: File exception handling
}


bool BackingStore::writeNode(const Octree::Node* const node, long* const outPosition) const
{
	return writeNodeToFile(_file, node, outPosition);
}


bool BackingStore::writeNodeToFile(	FILE* const file, 
									const Octree::Node* const node,
									long* const outPosition) const
{
	// Make sure the node has no children that are in memory
	for (uint8_t i = 0; i < 8; ++i) 
		assert(!((node->children[i] != NULL) && (node->isChildInMemory(i))));
	
	// Right now we always append the file (will leave gaps)
	fseek(file, 0, SEEK_END);
	*outPosition = ftell(file);
	
	// Write node meta info
	fwrite(node->children, sizeof(Octree::Node*), 8, file);
	fwrite(&(node->quantPointCount), sizeof(uint16_t), 1, file);
	
	// Write node data
	Octree::QuantPointBlock* block = node->data;
	
	while (block != NULL)
	{
		assert(node->quantPointCount > 0);
		
		fwrite(block, sizeof(QuantPoint), OCTREE_POINTS_PER_POINT_DATA_BLOCK, file);
				
		// Jump to next block
		block = block->next;
	}
	
	return true; // TODO: File exception handling
}

}