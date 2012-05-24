#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>

int main(int argc, char** argv)
{
  int ArgCount = 1;
  char* InputName  = argv[ArgCount++];
  char* OutputName = argv[ArgCount++];

  int XSize = atoi(argv[ArgCount++]);
  int YSize = atoi(argv[ArgCount++]);
  int ZSize = atoi(argv[ArgCount++]);

  int XYZ      = XSize*YSize*ZSize;
  int XYZ_plus = (XSize+1)*(YSize+1)*(ZSize+1);

  char* data = new char[XYZ];
  float* vertices = new float[XYZ_plus*3];

  unsigned int c = 0;
 
  for(int k=0; k<=ZSize; k++)
    for(int j=0; j<=YSize; j++)
      for(int i=0; i<=XSize; i++)
	{
	  vertices[c++] = (float)i;
	  vertices[c++] = (float)j;
	  vertices[c++] = (float)k;
	}

  std::ifstream ifm;
  ifm.open(InputName, std::ios::binary);
  ifm.read((char*)data, XSize*YSize*ZSize*sizeof(char));
  ifm.close();

  // Count number of visible faces
  unsigned int IdxCount =0;
  c = 0;
  unsigned int XY = XSize*YSize;
  
  for(int k=0; k<ZSize; k++)
    {
      for(int j=0; j<YSize; j++)
	{
	  for(int i=0; i<XSize; i++)
	    {
	      // If the voxel is not visible, no further
	      // processing is required
	      if(data[c]!=1)
		{
		  c++;
		  continue;
		}
	      
	      // Check x direction
	      if(i==0       || data[c-1]!=1)     IdxCount+=4;
	      if(i==XSize-1 || data[c+1]!=1)     IdxCount+=4;	      
	      // Check y direction
	      if(j==0       || data[c-XSize]!=1) IdxCount+=4;
	      if(j==YSize-1 || data[c+XSize]!=1) IdxCount+=4;
	      // Check z direction
	      if(k==0       || data[c-XY]!=1)    IdxCount+=4;
	      if(k==ZSize-1 || data[c+XY]!=1)    IdxCount+=4;
	      
	      c++;
	    }
	}
    }

  unsigned int* idx = new unsigned int[IdxCount];  
  unsigned int ic = 0;
  c = 0;

  for(int k=0; k<ZSize; k++)
    {
      unsigned int kXY   = k*(XSize+1)*(YSize+1);
      unsigned int kXY_p = (k+1)*(XSize+1)*(YSize+1);

      for(int j=0; j<YSize; j++)
	{
	  unsigned int jX   = j*(XSize+1);
	  unsigned int jX_p = (j+1)*(XSize+1);
	  for(int i=0; i<XSize; i++)
	    {
	      // If the voxel is not visible, no further
	      // processing is required
	      if(data[c]!=1)
		{
		  c++;
		  continue;
		}
	      
	      // Check x direction
	      if(i==0       || data[c-1]!=1) 
	      	{		  	      	  
	      	  idx[ic++] = kXY   + jX   + i;
	      	  idx[ic++] = kXY_p + jX   + i;
	      	  idx[ic++] = kXY_p + jX_p + i;	      	  
	      	  idx[ic++] = kXY   + jX_p + i;
	      	}
	      if(i==XSize-1 || data[c+1]!=1) 
	      	{		 
	      	  idx[ic++] = kXY   + jX   + i+1;
	      	  idx[ic++] = kXY   + jX_p + i+1;
	      	  idx[ic++] = kXY_p + jX_p + i+1;	 
	      	  idx[ic++] = kXY_p + jX   + i+1;
	      	}
	      // Check y direction
	      if(j==0       || data[c-XSize]!=1) 
	      	{		 
	      	  idx[ic++] = kXY   + jX  + i;
	      	  idx[ic++] = kXY   + jX  + i+1;
	      	  idx[ic++] = kXY_p + jX  + i+1;
	      	  idx[ic++] = kXY_p + jX  + i;
	      	}
	      if(j==YSize-1 || data[c+XSize]!=1) 
	      	{
	      	  idx[ic++] = kXY_p + jX_p + i;
	      	  idx[ic++] = kXY_p + jX_p + i+1;
	      	  idx[ic++] = kXY   + jX_p + i+1;
	      	  idx[ic++] = kXY   + jX_p + i;	      	  	      	 	      	  
	      	}
	      // Check z direction
	      if(k==0       || data[c-XY]!=1) 
	      	{		  
	      	  idx[ic++] = kXY   + jX   + i;		  		 
	      	  idx[ic++] = kXY   + jX_p + i;
	      	  idx[ic++] = kXY   + jX_p + i+1;
	      	  idx[ic++] = kXY   + jX   + i+1;
	      	}
	      if(k==ZSize-1 || data[c+XY]!=1) 
	      	{	      	
	      	  idx[ic++] = kXY_p + jX   + i;		       
	      	  idx[ic++] = kXY_p + jX   + i+1;
	      	  idx[ic++] = kXY_p + jX_p + i+1;
	      	  idx[ic++] = kXY_p + jX_p + i;
	      	}
     
	      c++;
	    }
	}
    }

  std::cout << "IdxCount: " << IdxCount << std::endl;


  // Write results to output file
  std::ofstream ofm;
  ofm.open(OutputName, std::ios::trunc);

  ofm << "ply"                                               << std::endl;
  ofm << "format binary_little_endian 1.0"                   << std::endl;
  ofm << "element vertex "  << XYZ_plus                      << std::endl;
  ofm << "property float x"                                  << std::endl;
  ofm << "property float y"                                  << std::endl;
  ofm << "property float z"                                  << std::endl;
  ofm << "element face " << (int)(IdxCount/4)                << std::endl;
  ofm << "property list uchar uint vertex_indices"           << std::endl;
  ofm << "end_header"                                        << std::endl;

  // Write vertex
  ofm.write((char*)vertices, XYZ_plus*3*sizeof(float));

  // Write faces
  unsigned char face_c = 4;
  int FaceSize = 4*sizeof(unsigned int);

  for(unsigned int i=0; i<IdxCount; i+=4)
    {
      ofm.write((char*)&face_c, 1);	  
      ofm.write((char*)&idx[i], FaceSize);
    }

  ofm.close();

  delete[] data;
  delete[] vertices;
  delete[] idx;

  return 0;
}
