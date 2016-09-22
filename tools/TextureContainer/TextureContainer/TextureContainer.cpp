// TextureContainer.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include "stdafx.h"
#include "jofilelib.hpp"
#include <iostream>
#include <algorithm>
#include <string>
#include "dirent.h"


struct texQuad
{
	float texCoordX, texCoordY;	///< Texture position (XY = [0,1]x[0,1]) relative to the lower left corner (0,0)
	float sizeX, sizeY;
};

using namespace std;

const int TEXTURESMAX = 16;

bool cmp( const Jo::Files::ImageWrapper* a, const Jo::Files::ImageWrapper* b ) 
{
    return a->Width() < b->Width() ;
}


int _tmain(int argc, _TCHAR* argv[])
{
	DIR *dir;
	dirent *ent;

	string names[TEXTURESMAX];

	CHAR path[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, path);
	strcat_s(path,"\\tex\\");
//	int i=0;
//	while(path[i] != 0) i++;
//	strcpy_s(&(path[i]),5, "\\tex\\");
//	path[i+5] = 0;
	if ((dir = opendir ((char*)&path/*"\\tex\\")*/)) != NULL) 
	{
		Jo::Files::ImageWrapper* images[TEXTURESMAX];
		int fileCount = 0;
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) 
		{
			//check for right ending
			if(strcmp(&(ent->d_name[strlen(ent->d_name)-4]), ".png") != 0) continue; 
			names[fileCount] = ent->d_name;
			names[fileCount].erase(names[fileCount].length()-4,4);
			printf ("%s\n", ent->d_name);
			Jo::Files::HDDFile file("tex\\"+string(ent->d_name));
			images[fileCount] = new Jo::Files::ImageWrapper(file, Jo::Files::Format::PNG );
			fileCount++;
		}
		closedir (dir);
		//calculate a good placement for all imgs
		texQuad* texQuads = new texQuad[fileCount];

//		for(int i=0; i<fileCount; i++)
		ZeroMemory((void*)&texQuads[0], sizeof(texQuad)*(fileCount));

		Jo::Files::ImageWrapper* imagesOrg[TEXTURESMAX];
		for(int i = 0; i < fileCount; i++)
			 imagesOrg[i] = images[i];
		sort( &images[0], &images[fileCount], cmp );
		int borderX = images[fileCount-1]->Width()+1;
		int borderY = 1;
		for(int i = fileCount-1; i>=0; i--)
		{
			//already placed, jump over
			if(texQuads[i].sizeX != 0.f) continue;
			texQuads[i].sizeX = images[i]->Width();
			texQuads[i].sizeY = images[i]->Height();
			texQuads[i].texCoordX = 1; 
			texQuads[i].texCoordY = borderY;//+texQuads[i].sizeY;
			int borderTmp = texQuads[i].sizeX;
			int borderMax = borderY + texQuads[i].sizeY;
			//search a smaller one that can be placed right from the previous one
			for(int c = i-1; c>=0; c--)
				if(images[c]->Width() < borderX - borderTmp)
				{
					if(texQuads[c].sizeX != 0) continue;
					texQuads[c].sizeX = images[c]->Width();
					texQuads[c].sizeY = images[c]->Height();
					texQuads[c].texCoordX = borderTmp + 1;
					texQuads[c].texCoordY = borderY;
					borderTmp += texQuads[c].sizeX + 1;
					if(borderMax < borderY + texQuads[c].sizeY) borderMax = borderY + texQuads[c].sizeY;
				} 
			//ensure that highest Y tex gets found
			borderY = borderMax + 2;
		}
		
		//write every img into one using the calculated positions
		borderX += 1;
		Jo::Files::ImageWrapper combinedImage(borderX, borderY, 4);
		
		//fill with 100% transparency to prevent visible edges
		for (int ix = 0; ix < borderX; ix++)
			for (int iy = 0; iy < borderY; iy++)
				combinedImage.Set(ix, iy, 3, 0.f);

		for(int i = 0; i<fileCount; i++)
			for(int ix = 0; ix < texQuads[i].sizeX; ix++)
				for(int iy = 0; iy < texQuads[i].sizeY; iy++)
				{
					int finalX = ix + texQuads[i].texCoordX; 
					int finalY = iy + texQuads[i].texCoordY;
					combinedImage.Set(finalX,finalY, 0, images[i]->Get(ix,iy,0));
					combinedImage.Set(finalX,finalY, 1, images[i]->Get(ix,iy,1));
					combinedImage.Set(finalX,finalY, 2, images[i]->Get(ix,iy,2));
					combinedImage.Set(finalX,finalY, 3, images[i]->Get(ix,iy,3));
				}
		try {
			Jo::Files::HDDFile combinedFile("combined.png", Jo::Files::HDDFile::OVERWRITE);
			combinedImage.Write(combinedFile, Jo::Files::Format::PNG);
			//transform numbers
			for (int i = 0; i < fileCount; i++)
			{
				texQuads[i].texCoordX /= borderX;
				texQuads[i].texCoordY += texQuads[i].sizeY;
				texQuads[i].texCoordY = 1 - (texQuads[i].texCoordY / (borderY));
				texQuads[i].sizeX /= borderX;
				texQuads[i].sizeY /= borderY;
			}

			/*write pos map
			 * NodeName = imgName without ending
			 */
			Jo::Files::MetaFileWrapper wrapper;
			//general tex info
			wrapper.RootNode[string("width")] = borderX;
			wrapper.RootNode[string("height")] = borderY;

			for (int i = 0; i < fileCount; i++)
			{
				//perform a linear search to find the original pos of the img to bind with the name
				int c = 0;
				while (images[i] != imagesOrg[c])
					c++;
				wrapper.RootNode[names[c]][string("positionX")] = texQuads[i].texCoordX;
				wrapper.RootNode[names[c]][string("positionY")] = texQuads[i].texCoordY;
				wrapper.RootNode[names[c]][string("sizeX")] = texQuads[i].sizeX;
				wrapper.RootNode[names[c]][string("sizeY")] = texQuads[i].sizeY;
				//			auto& imgRoot = wrapper.RootNode.Add[names[i]];
				//			imgRoot.Add
			}

			Jo::Files::HDDFile tableFile("combined.sraw", Jo::Files::HDDFile::OVERWRITE);
			wrapper.Write(tableFile, Jo::Files::Format::SRAW);
			tableFile.Flush();
		}
		catch (std::string _msg)
		{
			cout << _msg;
		}
//		auto& PosX = wrapper.RootNode.Add(string("positionX"), Jo::Files::MetaFileWrapper::ElementType::FLOAT, 256 );
//		auto& PosY = wrapper.RootNode.Add(string("positionY"), Jo::Files::MetaFileWrapper::ElementType::FLOAT, 256 );
//		auto& sizeX = wrapper.RootNode.Add(string("sizeX"), Jo::Files::MetaFileWrapper::ElementType::FLOAT, 256 );
//		auto& sizeY = wrapper.RootNode.Add(string("sizeY"), Jo::Files::MetaFileWrapper::ElementType::FLOAT, 256 );
//
		delete[] texQuads;
		for(fileCount--;fileCount>=0;fileCount--) delete images[fileCount]; 
	}
	else 
	{
		/* could not open directory */
		perror ("");
		return EXIT_FAILURE;
	}
//	Sleep(5000);
	return 0;
}

