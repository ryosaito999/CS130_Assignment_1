/**
 * minigl.cpp
 * -------------------------------
 * Implement miniGL here.
 * Do not use any additional files
 */

#include <cstdio>
#include <vector>
#include <cstdlib>
#include <string>
#include <iostream>
#include "minigl.h"

using namespace std;
class Vertex3{
    
    public:
    float x;
    float y;
    float z;

    Vertex3(){
        x = 0;
        y = 0;
        z = 0;
    }
    Vertex3(float a, float b, float c){
        x = a;
        y = b;
        z = c;
    }
};

//not sure what to use this for??
class Vertex4{

    public:
    float x;
    float y;
    float z;
    float w;

    Vertex4(){
        x = 0;
        y = 0;
        z = 0;
        w=0;
    }
    Vertex4(float a, float b, float c){
        x = a;
        y = b;
        z = c;
        w = 1;
    }

    Vertex4(float a, float b, float c, float d){
      x = a;
      y = b;
      z = c;
      w = d;
    }

    //convert vertex3 -> vertex4
    Vertex4(Vertex3 v , float n){
        x = v.x;
        y = v.y;
        z = v.z;
        w = n;
    }
};
class RGB{

public:
   int R;
   int G;
   int B;

    
 RGB(){
      R=0;
      G=0;
      B=0;
   } 
   
  RGB(int r, int g, int b){
      R=r;
      G=g;
      B=b;
   } 
   
};

class Matrix4 {
public:
  
  float matrix4[4][4];
  
  //creates matrix row by row filled with 0's
  Matrix4(){
    for(int y=0; y< 4; ++y)
      for(int x= 0 ; x <4 ; ++x){
        matrix4[x][y] = 0 ;
      }

      print_matrix();
  }

  void print_matrix(){ 
    for(int y=0; y< 4; ++y){
      cout <<  endl;
      for(int x= 0 ; x <4 ; ++x){
        cout << matrix4[x][y] << "    ";
      }
    }

    cout << "printed" << endl;
  }
};

//variables
MGLpoly_mode mgl_mode; //triangles or squares?
bool isDrawing;
vector <Vertex3> points_array; 

MGLpixel MGL_SCREEN_WIDTH = 320;
MGLpixel MGL_SCREEN_HEIGHT = 240;
MGLpixel resolution = MGL_SCREEN_WIDTH/MGL_SCREEN_HEIGHT;
MGLpixel framebuffer[320][240];
RGB white_color(255,255,255);//white color

//HELPER FUNCTIONS

float slope(int x, int y, int x2, int y2){
  return (y2 - y)/(x2-x);
}

/** 
    set pixel (x,y) to RGB color on frame buffer to draw 
  */ 
void plot(unsigned int x, unsigned int y, RGB coloring) 
{ 
 	MGLpixel color = 0; 
 	MGL_SET_RED(color, coloring.R); 
 	MGL_SET_GREEN(color, coloring.G); 
 	MGL_SET_BLUE(color, coloring.B); 
  framebuffer[x][y] = color; //draw everything queued up on buffer
}  
/* Convert given x,y to screen coordinates to make point visible*/
void convertScreen_plot(unsigned int x, unsigned int y, RGB coloring){
  
  //scale points first
  plot( x,y, coloring);
  Matrix4 m;
}

void plotLines(){
    
    for (int i=0; i < points_array.size(); ++i){
      //must multiply coordinates by sceenheight , screenwidth to scale properly
      points_array[i].x = points_array[i].x * MGL_SCREEN_WIDTH; 
      points_array[i].y = points_array[i].y * MGL_SCREEN_HEIGHT;
      plot( points_array[i].x,points_array[i].y, white_color);

    }
}




/**
 * Standard macro to report errors
 */
inline void MGL_ERROR(const char* description) {
    printf("%s\n", description);
    exit(1);
}


/**
 * Read pixel data starting with the pixel at coordinates
 * (0, 0), up to (width,  height), into the array
 * pointed to by data.  The boundaries are lower-inclusive,
 * that is, a call with width = height = 1 would just read
 * the pixel at (0, 0).
 *
 * Rasterization and z-buffering should be performed when
 * this function is called, so that the data array is filled
 * with the actual pixel values that should be displayed on
 * the two-dimensional screen.
 */
void mglReadPixels(MGLsize width,
                   MGLsize height,
                   MGLpixel *data)
{
 
  plotLines();
	for(unsigned x = 0; x < width; x++) 
 		for(unsigned y = 0; y < height; ++y) 
 			data[y*width+x] = framebuffer[x][y]; 
}

/**
 * Start specifying the vertices for a group of primitives,
 * whose type is specified by the given mode.
 */
void mglBegin(MGLpoly_mode mode)
{
    isDrawing = true;
	//if mode is triangle set current mode to triangle
	if(mode ==  MGL_TRIANGLES) 
        mgl_mode == MGL_TRIANGLES;
    
    //if mode is quads
	else if(mode == MGL_QUADS) 
        mgl_mode == MGL_QUADS;

	//if something else
	else 
        cout << "ERROR" << endl;
}

/**
 * Stop specifying the vertices for a group of primitives.
 */
void mglEnd()
{
    cout << points_array.size() << endl;
    isDrawing = false;
}

/**
 * Specify a two-dimensional vertex; the x- and y-coordinates
 * are explicitly specified, while the z-coordinate is assumed
 * to be zero.  Must appear between calls to mglBegin() and
 * mglEnd().
 */
void mglVertex2(MGLfloat x,
                MGLfloat y)
{

    Vertex3 vertex (x,y,0);
    points_array.push_back(vertex);

}

/**
 * Specify a three-dimensional vertex.  Must appear between
 * calls to mglBegin() and mglEnd().
 */
void mglVertex3(MGLfloat x,
                MGLfloat y,
                MGLfloat z)
{
    Vertex3 vertex (x,y,z);
    points_array.push_back(vertex);

}

/**
 * Set the current matrix mode (modelview or projection).
 */
void mglMatrixMode(MGLmatrix_mode mode)
{
}

/**
 * Push a copy of the current matrix onto the stack for the
 * current matrix mode.
 */
void mglPushMatrix()
{

}

/**
 * Pop the top matrix from the stack for the current matrix
 * mode.
 */
void mglPopMatrix()
{
}

/**
 * Replace the current matrix with the identity.
 */
void mglLoadIdentity()
{
  // Matrix4 identityMatrix;
  // identityMatrix[0][0] = 1;
  // identityMatrix[1][1] = 1;
  // identityMatrix[2][2] = 1;
  // identityMatrix[3][3] = 1;



}

/**
 * Replace the current matrix with an arbitrary 4x4 matrix,
 * specified in column-major order.  That is, the matrix
 * is stored as:
 *
 *   ( a0  a4  a8  a12 )
 *   ( a1  a5  a9  a13 )
 *   ( a2  a6  a10 a14 )
 *   ( a3  a7  a11 a15 )
 *
 * where ai is the i'th entry of the array.
 */
void mglLoadMatrix(const MGLfloat *matrix)
{
}

/**
 * Multiply the current matrix by an arbitrary 4x4 matrix,
 * specified in column-major order.  That is, the matrix
 * is stored as:
 *
 *   ( a0  a4  a8  a12 )
 *   ( a1  a5  a9  a13 )
 *   ( a2  a6  a10 a14 )
 *   ( a3  a7  a11 a15 )
 *
 * where ai is the i'th entry of the array.
 */
void mglMultMatrix(const MGLfloat *matrix)
{
}

/**
 * Multiply the current matrix by the translation matrix
 * for the translation vector given by (x, y, z).
 */
void mglTranslate(MGLfloat x,
                  MGLfloat y,
                  MGLfloat z)
{
}

/**
 * Multiply the current matrix by the rotation matrix
 * for a rotation of (angle) degrees about the vector
 * from the origin to the point (x, y, z).
 */
void mglRotate(MGLfloat angle,
               MGLfloat x,
               MGLfloat y,
               MGLfloat z)
{
}

/**
 * Multiply the current matrix by the scale matrix
 * for the given scale factors.
 */
void mglScale(MGLfloat x,
              MGLfloat y,
              MGLfloat z)
{
}

/**
 * Multiply the current matrix by the perspective matrix
 * with the given clipping plane coordinates.
 */
void mglFrustum(MGLfloat left,
                MGLfloat right,
                MGLfloat bottom,
                MGLfloat top,
                MGLfloat near,
                MGLfloat far)
{
}

/**
 * Multiply the current matrix by the orthographic matrix
 * with the given clipping plane coordinates.
 */
void mglOrtho(MGLfloat left,
              MGLfloat right,
              MGLfloat bottom,
              MGLfloat top,
              MGLfloat near,
              MGLfloat far)
{
}

/**
 * Set the current color for drawn shapes.
 */
void mglColor(MGLbyte red,
              MGLbyte green,
              MGLbyte blue)
{
}
