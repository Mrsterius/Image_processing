// SketchupApplication.cpp : Defines the entry point for the console application.
//
//#include "stdafx.h"
#include <slapi/slapi.h>
#include <slapi/geometry.h>
#include <slapi/initialize.h>
#include <slapi/model/model.h>
#include <slapi/model/entities.h>
#include <slapi/model/face.h>
#include <slapi/model/edge.h>
#include <slapi/model/vertex.h>
#include <vector>
#include <slapi/model/image.h>
#include <iostream>
#include <slapi/unicodestring.h>
#include <fstream>

using namespace std;
int main() {

	// Always initialize the API before using it
	SUInitialize();
	// Create an empty model
	SUModelRef model = SU_INVALID;
	

	SUResult res = SUModelCreate(&model);
	// It's best to always check the return code from each SU function call.
	// Only showing this check once to keep this example short.
	if (res != SU_ERROR_NONE)
	return 1;


	// Get the entity container of the model
	SUEntitiesRef entities = SU_INVALID;
	SUModelGetEntities(model, &entities);
	
	/*

	//HARD CODED OBJECTS--------------------------------------------------------------------------------------
	//O B J E C T 1 -----------------------

	// Create a loop input describing the vertex ordering for a face's outer loop
	/*
	SULoopInputRef outer_loop1 = SU_INVALID;
	SULoopInputCreate(&outer_loop1);
	for (size_t i = 0; i < 4; ++i)
		{
			SULoopInputAddVertexIndex(outer_loop1, i);
		}
	*/
	// Create the face
	SUFaceRef face1 = SU_INVALID;
	SUPoint3D vertices1[4] = {	
							{0,10,0},
							{0,50,0},
							{10,50,0},
							{10,10,0}
							};
	SUFaceCreateSimple(&face1, vertices1,4);// &outer_loop1);
	
	// O B J E C T 2 -------------------------------
	/*
	SULoopInputRef outer_loop2 = SU_INVALID;
	SULoopInputCreate(&outer_loop2);
	for (size_t i = 0; i < 4; ++i)
		{
			SULoopInputAddVertexIndex(outer_loop2, i);
		}
	*/
	SUFaceRef face2 = SU_INVALID;
	SUPoint3D vertices2[4] = {	
							{0,50,0},
							{0,60,0},
							{30,60,0},
							{30,50,0}
							};
	SUFaceCreateSimple(&face2, vertices2,4);// &outer_loop2);

	//-----------------------------------
	// O B J E C T  3 ----------------------
	/*
	SULoopInputRef outer_loop3 = SU_INVALID;
	SULoopInputCreate(&outer_loop3);
	for (size_t i = 0; i < 6; ++i)
		{
			SULoopInputAddVertexIndex(outer_loop3, i);
		}
	*/
	SUFaceRef face3 = SU_INVALID;
	SUPoint3D vertices3[6] = {	
							{40,50,0},
							{40,60,0},
							{70,60,0},
							{70,10,0},
							{60,10,0},
							{60,50,0}
							};
	/**/

	SUFaceCreateSimple(&face3, vertices3,6);// &outer_loop3);
	
	
	// Add the faces to the entities
	//SUEntitiesAddFaces(entities, 1, &face1);
	//SUEntitiesAddFaces(entities, 1, &face2);
	//SUEntitiesAddFaces(entities, 1, &face3);
	
	//------------------------------------------------------------------------------------------------------
	//
	//-------------------------------
	// AUTOMATIC ADDITION OF FACES
	/*
	const int no_of_contours=3;
	const int no_of_pts = 4;
	SUPoint3D verita[no_of_pts];
	SUFaceRef face_try[no_of_contours] = SU_INVALID;
	for (int ct=0;ct<no_of_contours;ct++)
	{
		
	verita[0].x=0;
	verita[0].y=0;
	verita[0].z=0;
	
	verita[1].x=0;
	verita[1].y=10 + ct*10;
	verita[1].z=0;

	verita[2].x=10 + ct*10;
	verita[2].y=10 + ct*10;
	verita[2].z=0;

	verita[3].x=10 + ct*10;
	verita[3].y=0;
	verita[3].z=0;
	
	SUFaceCreateSimple(&face_try[ct], verita,4);
	SUEntitiesAddFaces(entities, 1, &face_try[ct]);
	}
	*/
	//------------------------------------------------------------------------------------------------------
	//READING FROM A FILE AND CREATING FACES ACCORDINGLY
	double a[5000], b[5000];
	for(int p=0;p<135;p++)
	{
	unsigned long int count=0,n;
	char buffer[150];
	n=sprintf_s(buffer,"C:\\Users\\Himanshu\\Documents\\Visual Studio 2012\\Projects\\ConstructionAreaDetect\\ConstructionAreaDetect\\Coordinates\\cnt%d.txt",p);
	//std::cout<<buffer<<"\n";
	fstream b_file (buffer);
	while(!b_file.eof())
	{
	b_file>> a[count]>>b[count];
	count++;
	}
	cout<<count<<endl;

	const int npts=count;
	const int ncts=135;
	SUFaceRef face_filetry[ncts] = SU_INVALID;
	SUPoint3D veriFila[5000];
	for (int nc=0;nc<ncts;nc++)
	{
	for (int n=0;n<npts;n++)
	{
		veriFila[n].x=a[n];
		veriFila[n].y=b[n];
		veriFila[n].z=0;
	}
	
	SUFaceCreateSimple(&face_filetry[nc], veriFila,npts);
	SUEntitiesAddFaces(entities, 1, &face_filetry[nc]);
	}
	b_file.close();
	}

	//------------------------------------------------------------------------------------------------------
	double area;
	SUFaceGetArea(face3, &area);
	//printf("%d",area);
	//std::cout<<area;




	
	//Adding image into Entity
	SUImageRef image = SU_INVALID;

	const char* pth ="C:/Users/Himanshu/Documents/Visual Studio 2012/Projects/Images/test6.jpg";
	SUResult res2 = SUImageCreateFromFile(&image,pth);
	if (res2 != SU_ERROR_NONE)
		std::cout<<"Error "<<res2<<" in SUImageCreateFromFile\n";


	SUResult res1= SUEntitiesAddImage(entities, image);
	if (res1!=SU_ERROR_NONE)
		std::cout<<"Error "<<res1<<" in SUEntitiesAddImage";
	
	
	


	std::cout<<"\nPheww.. Continued without Errors :P\n";
	// Save the in-memory model to a file
	SUModelSaveToFileWithVersion(model, "MODEL23H_Reduce_HistEQ.skp",SUModelVersion_SU2013);
	// Must release the model or there will be memory leaks
	SUModelRelease(&model);
	// Always terminate the API when done using it
	SUTerminate();
	getchar();
	return area;
}


