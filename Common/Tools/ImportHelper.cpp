/*
 *  ImportHelper.cpp
 *  MacApp
 *
 *  Created by Lars Schneider on 30.08.10.
 *  Copyright 2010 Lars Schneider. All rights reserved.
 *
 */

#include "ImportHelper.h"
#include "Octree.h"
#include <vector>
#include <fstream>


namespace WVSClientCommon
{


namespace ImportHelper
{

std::vector<std::string> split(const std::string& value, char separator)
{
    std::vector<std::string> result;
    std::string::size_type p = 0;
    std::string::size_type q;
    while ((q = value.find(separator, p)) != std::string::npos) {
        result.emplace_back(value, p, q - p);
        p = q + 1;
    }
    result.emplace_back(value, p);
    return result;
}


void ImportTraian(Octree* octree, const char* const filename)
{
	printf("Reading  %s...\n", filename);

    std::ifstream read(filename, std::ifstream::in | std::ifstream::binary);
    
    read.seekg(0, std::ifstream::end);
    long long file_size = read.tellg();
    read.seekg(0, std::ifstream::beg);

	WVSPoint p;
	p.radius = 1.0;

    float progress = 0;

    std::string line;
    while (std::getline(read, line))
    {
        const auto traianPoint = split(line, ' ');

        p.position.x = std::stof(traianPoint[0]);
        p.position.z = std::stof(traianPoint[1]);
        p.position.y = std::stof(traianPoint[2]);

        p.color.red = std::stoi(traianPoint[3]);
        p.color.green = std::stoi(traianPoint[4]);
        p.color.blue = std::stoi(traianPoint[5]);

        octree->addPoint(&p);

        float current_progress = 100.0f * (float)read.tellg() / (float)file_size;

        if (current_progress > 65.0f)
        {
            break;
        }

        if (roundf(current_progress) > roundf(progress))
        {
            progress = current_progress;
            printf("%.0f \n", progress);
        } 
    }
}


void ImportCTFormat(Octree* octree, const char* const filename)
{
	printf("Reading  %s...\n", filename);

FILE *out = fopen("/abc.txt", "w");
	FILE *file = fopen(filename, "rb");
	WVSPoint p;
	
	p.radius = 1;
	
	uint8_t byte[65536];
	fread(&byte, sizeof(uint8_t), 65536, file);
	int i = 0;
	
	for (int x = 0; x < 172; ++x)
	{
		for (int y = 0; y < 512; ++y)
		{
			for (int z = 0; z < 256; ++z)
			{
				if (byte[i] == 1)
				{
					p.position.x = z * 5;
					p.position.y = y * 5;
					p.position.z = x * 5;
					
					uint8_t gray;
					switch (y % 4) {
						case 0: gray = 0; break;
						case 1: gray = 30; break;
						case 2: gray = 60; break;
						case 3: gray = 30; break;
						//case 4: gray = 0 break;

					}
					//uint8_t gray = (y % 2 == 0 ? 255 : 200);//((sin(float(y)*512.0*0.05))*(1/10.0)+0.45) * 255.0f;
					p.color.red = gray;
					p.color.green = gray;
					p.color.blue = gray;
					
					 
					 
					octree->addPoint(&p);
					
					fprintf(out,"%i %i %i\n", z, y, x);
					//printf("%i %i %i\n", z, y, x);
				}
				
				i++;
				
				if (i == 65536)
				{
					fread(&byte, sizeof(uint8_t), 65536, file);
					i = 0;
				}
			}
		}
		
		//printf("%i/255\n", x);
	}
fclose (out);
	fclose(file);
	
	printf("fertig!");
}


void ImportRicoFormat(Octree* octree, const char* const filename)
{
	printf("Reading  %s...\n", filename);

	FILE *file = fopen(filename, "rb");
	
	// obtain file size:
	fseek(file , 0 , SEEK_END);
	long filesize = ftell(file);
	rewind(file);
  
	long pos = 0;
	long lastprogess = 0;
	long pointCount = 0;
	
	VECTOR3 min;
	VECTOR3 max;
	bool init = true;

	static const int pointchunk = 4096*2;
	uint8_t byte[16 * pointchunk];
	int byteIndex = 0;
	// Read 1024 points
	fread(&byte, sizeof(uint8_t), 16 * pointchunk, file);
		 
	int i = 0;
	while (pos < filesize)
	{
		++i;
		
		WVSPoint p;

		memcpy(&p.position, &(byte[byteIndex]), sizeof(VECTOR3));
		byteIndex += sizeof(VECTOR3);
		//fread(&p.position, sizeof(VECTOR3), 1, file);

		memcpy(&p.color, &(byte[byteIndex]), sizeof(RGBColor));
		byteIndex += sizeof(RGBColor) + 1;
		//fread(&p.color, sizeof(RGBColor), 1, file);
		//fseek(file, 1, SEEK_CUR);
		
		if (byteIndex >= 16 * pointchunk)
		{
			fread(&byte, sizeof(uint8_t), 16 * pointchunk, file);
			pos = ftell(file);
			byteIndex = 0;
		}
		
		p.radius = 0.001f;
		p.position.x *= 4;
		p.position.y *= 4;
		p.position.z *= 4;
		
		p.position.x -= 88510.73;
		p.position.y -= 279.36;
		p.position.z -= 80576.14;
		
		if (init)
		{
			init = false;
			min = p.position;
			max = p.position;
		}
		
		if (min.x > p.position.x) min.x = p.position.x; 
		if (min.y > p.position.y) min.y = p.position.y; 
		if (min.z > p.position.z) min.z = p.position.z; 
		
		if (max.x < p.position.x) max.x = p.position.x; 
		if (max.y < p.position.y) max.y = p.position.y; 
		if (max.z < p.position.z) max.z = p.position.z; 
		
		//PRINT_VECTOR3(p.position);
		
		octree->addPoint(&p);
		pointCount++;
		
		long progress = long(100.0f * float(pos) / float(filesize));
		if ((progress > lastprogess) && (progress % 2 == 0))
		{
			lastprogess = progress;
			printf("%.0f %%\n", 100.0f * float(pos) / float(filesize));
		}
	}

	fclose(file);
	
	printf("%li points loaded.\n\nBounding Box:\n", pointCount);
	PRINT_VECTOR3(min);
	PRINT_VECTOR3(max);
	
	octree->printStatistics();
}


void ImportPLYFormat(Octree* octree, const char* const filename, const int startbyte, const int vertices)
{
printf(filename);
	FILE *file = fopen(filename, "rb");
	
	// obtain file size:
	fseek(file , 0 , SEEK_END);
	long filesize = ftell(file);
	rewind(file);
  
	long pos = 0;
	long lastprogess = 0;
	long pointCount = 0;
	
	VECTOR3 min;
	VECTOR3 max;
	bool init = true;

	uint8_t byte;
	for (int i = 0; i < startbyte; ++i)
	{
		fread(&byte, sizeof(uint8_t), 1, file);
		printf("%c", byte);
	}
	
	VECTOR3 normal;
	MATRIX m;
	MatrixRotationX(m, M_PI_2);
		
	int i = 0;
	while (i < vertices)//((pos < filesize) && (lastprogess < 1))
	{
		++i;
		
		WVSPoint p;
		p.radius = 0.001f;

		fread(&p.position, sizeof(VECTOR3), 1, file);
		fread(&normal, sizeof(VECTOR3), 1, file);
		
//		uint8_t sevenBitNormal = 0;
//		float_t dot = 0;
//		float_t bestDot = 0;
//		for (uint8_t j=0; j < 128; ++j)
//		{
//			dot =	normal.x * sevenBitNormalMap[3*j+0] + 
//					normal.y * sevenBitNormalMap[3*j+1] +
//					normal.z * sevenBitNormalMap[3*j+2];
//			
//			if (dot > bestDot)
//			{
//				bestDot = dot;
//				sevenBitNormal = j;
//			}
//		}
//		assert(sevenBitNormal < 128);
//		p.normalIndex = sevenBitNormal;
	
		fread(&p.color, sizeof(RGBColor), 1, file);
		fseek(file, 1, SEEK_CUR);
		
		if (startbyte == 341)
		{
			p.position.x *= 25;
			p.position.y *= 25;
			p.position.z *= 25;
			
			p.position.z += 2500;
		}
		else {
			p.position.x *= 5;
			p.position.y *= 5;
			p.position.z *= 5;
		}

		
		if (init) {
			init = false;
			min = p.position;
			max = p.position;
		}
		
		if (min.x > p.position.x) min.x = p.position.x; 
		if (min.y > p.position.y) min.y = p.position.y; 
		if (min.z > p.position.z) min.z = p.position.z; 
		
		if (max.x < p.position.x) max.x = p.position.x; 
		if (max.y < p.position.y) max.y = p.position.y; 
		if (max.z < p.position.z) max.z = p.position.z; 
		
		octree->addPoint(&p);
		pointCount++;
		
		pos = ftell(file);
		
		long progress = long(100.0f * float(pos) / float(filesize));
		if ((progress > lastprogess) && (progress % 2 == 0))
		{
			lastprogess = progress;
			printf("%.0f %%\n", 100.0f * float(pos) / float(filesize));
		}
	}

	fclose(file);
	
	printf("%li points loaded.\n\nBounding Box:\n", pointCount);
	PRINT_VECTOR3(min);
	PRINT_VECTOR3(max);
	
}


}

}