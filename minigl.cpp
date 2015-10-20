/**
 * minigl.cpp
 * -------------------------------
 * Implement miniGL here.
 * Do not use any additional files
 */

#include <cstdio>
#include <vector>
#include <cmath>
 #include <stack>
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

  }

  Matrix4 operator=(const Matrix4 &m){
    for(int i = 0; i < 4;i++)
      for(int j = 0; j < 4; j++)
        matrix4[i][j] = m.matrix4[i][j];
    
    return *this;
  }
  
  // taken from https://msdn.microsoft.com/en-us/library/hh873134.aspx
  Matrix4 operator*(Matrix4 m){ 
    MGLfloat s;
    Matrix4 temp;
    for (int i=0;i<4;i++)
      for (int j=0;j<4;j++){
        s=0;
        for (int e=0;e<4;e++)
          s+=matrix4[i][e]* m.matrix4[e][j];
        
        temp.matrix4[i][j]=s;
      }

   return temp;
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

string stack_status;
stack <Matrix4> proj_stack;
stack <Matrix4> model_stack;


Matrix4 currentMatrix;



//HELPER FUNCTIONS

float slope(int x, int y, int x2, int y2){
  return (y2 - y)/(x2-x);
}

/** 
    set pixel (x,y) to RGB color on frame buffer to draw 
  */ 
void set_pixel(unsigned int x, unsigned int y, RGB coloring) 
{ 
 	MGLpixel color = 0; 
 	MGL_SET_RED(color, coloring.R); 
 	MGL_SET_GREEN(color, coloring.G); 
 	MGL_SET_BLUE(color, coloring.B); 
  framebuffer[x][y] = color; //draw everything queued up on buffer
}  


void draw_line(int x0, int y0, int x1, int y1)
{
    //NOT WORKING CODE(PUT BETTER CODE HERE!!)
    float dx = x1 - x0;
    float dy = y1 - y0;
    float yNext = y0;
    float xNext = x0;
    //vertical line
    if( dx == 0){
        for(int y = y0; y < y1; ++y)
            set_pixel(x0, y, white_color );
    }
    float m = dy/dx;

    // Right Side
    if( dx > 0 )   {
        //quad I, m <= 1
        //quad IV, m >= -1
        if(  m <=  1 && m >= -1  ){
            for(int x = x0; x < x1; ++x){
                set_pixel(x, yNext,white_color );
                yNext = yNext + m; // dda algotithm y_k+1 = yk + m for every x incriment
            }
        }
        //quad I, m > 1
        if( m > 1 ){
            for(int y = y0; y < y1; ++y){
                set_pixel(xNext, y,white_color );
                xNext = xNext + 1/m;
            }
        }
        //quad IV m < 1
        if( m < 1){
            for(int y = y0; y > y1; --y){
                set_pixel(xNext, y ,white_color);
                xNext = xNext - 1/m;
            }
        }
        
    }
    
    //left side
    if( dx < 0){
        //quad II , quad III
        //   m > -1
        if(  m >= -1  ){
            for(int x = x0; x > x1; --x){
                set_pixel(x, yNext,white_color );
                yNext = yNext - m; // dda algotithm y_k+1 = yk + m for every x incriment
            }
        }
        
        //quad II 
        // m < -1
        if( dy > 0){
            if( m < -1 ){
                for(int y = y0; y < y1; ++y){
                    set_pixel(xNext, y,white_color );
                    xNext = xNext + 1/m;
                }
            }            
        }
        //quad III
        // m > 1
        if( dy < 0){
            if( m > 1 ){
                cout << "???";
                for(int y = y0; y > y1; --y){
                    set_pixel(xNext, y,white_color );
                    xNext = xNext - 1/m;
                }
            }            
        } 
        
    }
    return;
}


//from  http://www.geeksforgeeks.org/check-whether-a-given-point-lies-inside-a-triangle-or-not/
/* A utility function to calculate area of triangle formed by (x1, y1), 
   (x2, y2) and (x3, y3) */

double area(int x1, int y1, int x2, int y2, int x3, int y3)
{
   return abs((x1*(y2-y3) + x2*(y3-y1)+ x3*(y1-y2))/2.0);
}

/* A function to check whether point P(x, y) lies inside the triangle formed 
   by A(x1, y1), B(x2, y2) and C(x3, y3) */
bool isInsideTri(int x1, int y1, int x2, int y2, int x3, int y3, int x, int y)
{   
   /* Calculate area of triangle ABC */
   float A = area (x1, y1, x2, y2, x3, y3);
 
   /* Calculate area of triangle PBC */  
   float A1 = area (x, y, x2, y2, x3, y3);
 
   /* Calculate area of triangle PAC */  
   float A2 = area (x1, y1, x, y, x3, y3);
 
   /* Calculate area of triangle PAB */   
   float A3 = area (x1, y1, x2, y2, x, y);
   
   /* Check if sum of A1, A2 and A3 is same as A */
   return (A == A1 + A2 + A3);
}




void plotLines(){
    

    for (int i=0; i < points_array.size(); ++i){
      //must multiply coordinates by sceenheight , screenwidth to scale properly
      //plot points for now -> delete later
      set_pixel( points_array[i].x,points_array[i].y, white_color);

    }
    if( mgl_mode == MGL_TRIANGLES){

      int x1 = points_array[0].x;
      int y1 = points_array[0].y;
      int z1 = points_array[0].z;

      int x2 = points_array[1].x;
      int y2 = points_array[1].y;
      int z2 = points_array[1].z;

      int x3 = points_array[2].x;
      int y3 = points_array[2].y;
      int z3 = points_array[2].z;

      draw_line(x1,y1,x2,y2);
      draw_line(x2,y2,x3,y3);
      draw_line(x3,y3,x1,y1);


      //next shade in triangle!
      //check baycentric coordinates
      for(unsigned x = 0; x < MGL_SCREEN_WIDTH; x++) {
        for(unsigned y = 0; y < MGL_SCREEN_HEIGHT; ++y) {
          if( isInsideTri(x1,y1,x2,y2,x3,y3,x,y) )
            set_pixel(x,y,white_color);

            }
        }
    }   
    for (int i=0; i < points_array.size(); ++i){
      //must multiply coordinates by sceenheight , screenwidth to scale properly
      //plot points for now -> delete later
      set_pixel( points_array[i].x,points_array[i].y, white_color);

    }

    if( mgl_mode == MGL_TRIANGLES){

      int x1 = points_array[0].x;
      int y1 = points_array[0].y;
      int z1 = points_array[0].z;

      int x2 = points_array[1].x;
      int y2 = points_array[1].y;
      int z2 = points_array[1].z;

      int x3 = points_array[2].x;
      int y3 = points_array[2].y;
      int z3 = points_array[2].z;

      draw_line(x1,y1,x2,y2);
      draw_line(x2,y2,x3,y3);
      draw_line(x3,y3,x1,y1);


      //next shade in triangle!
      //check baycentric coordinates
      for(unsigned x = 0; x < MGL_SCREEN_WIDTH; x++) {
        for(unsigned y = 0; y < MGL_SCREEN_HEIGHT; ++y) {
          if( isInsideTri(x1,y1,x2,y2,x3,y3,x,y) )
            set_pixel(x,y,white_color);

      }
    }



  }

  if( mgl_mode == MGL_QUADS){
      int x1 = points_array[0].x;
      int y1 = points_array[0].y;
      int z1 = points_array[0].z;

      int x2 = points_array[1].x;
      int y2 = points_array[1].y;
      int z2 = points_array[1].z;

      int x3 = points_array[2].x;
      int y3 = points_array[2].y;
      int z3 = points_array[2].z;
      
      int x4 = points_array[3].x;
      int y4 = points_array[3].y;
      int z4 = points_array[3].z;

      draw_line(x1,y1,x2,y2);
      draw_line(x2,y2,x3,y3);
      draw_line(x3,y3,x4,y4);
      draw_line(x4,y4,x1,y1);
    }

}

    //next plot connect each point to make lines
    //currently own collection of vectors -> need to perform matrix multiplaction one each




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

//load projection matrix and multiply vertex to scale it
// void projectionMatrix(x,y,z){


// }

Vertex3 convert_to_screen(MGLfloat x, MGLfloat y, MGLfloat z){

  //Create a 4x4 matrix with the point coordinates
  Matrix4 tmp;
  tmp.matrix4[0][3] = x;
  tmp.matrix4[1][3] = y;
  tmp.matrix4[2][3] = z;
  tmp.matrix4[3][3] = 1;

  //load projection matrix on top of projectional stack (gonna be orthogonal matrix or fulcurum matrix)

  Matrix4 proj = proj_stack.top();
  
  //Scale screen to 0,0 -> 2,2
  Matrix4 scaler;
  scaler.matrix4[0][0] = MGL_SCREEN_WIDTH/2; 
  scaler.matrix4[1][1] = MGL_SCREEN_HEIGHT/2;
  scaler.matrix4[2][2] = 1; 
  scaler.matrix4[3][3] = 1;
  

  //Load matrix that translate screen up 1 right 1
  Matrix4 translater;
  translater.matrix4[0][0] = 1;
  translater.matrix4[1][1] = 1;
  translater.matrix4[2][2] = 1;
  translater.matrix4[3][3] = 1;
  translater.matrix4[0][3] = 1;
  translater.matrix4[1][3] = 1;
  translater.matrix4[2][3] = 1;
  translater.matrix4[3][3] = 1;

  tmp = proj * tmp;
  tmp = translater * tmp;
  tmp = scaler * tmp;
  tmp.print_matrix();
  return Vertex3( tmp.matrix4[0][3] , tmp.matrix4[1][3], tmp.matrix4[2][3]);

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
    //need to convert coordinates to orthographic 
    cout << "x: " << x<< ", y: " << y  << endl;
    Vertex3 vertex = convert_to_screen( x , y, 0);

    //Vertex3 vertex (x ,y ,0);
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

    cout << "x: " << x<< ", y: " << y  << endl;
    Vertex3 vertex = convert_to_screen( x , y, z);

    //Vertex3 vertex (x ,y ,0);
    points_array.push_back(vertex);

}

/**
 * Set the current matrix mode (modelview or projection).
 */
void mglMatrixMode(MGLmatrix_mode mode)
{

  mglPushMatrix();
  if( mode == MGL_MODELVIEW){
    stack_status = "modelview";
  }

    if( mode == MGL_PROJECTION){
    stack_status = "projection";
  }
}

/**
 * Push a copy of the current matrix onto the stack for the
 * current matrix mode.
 */
void mglPushMatrix()
{

  if( stack_status == "projection" )
    proj_stack.push(currentMatrix);
  

  else
    model_stack.push(currentMatrix);


}

/**
 * Pop the top matrix from the stack for the current matrix
 * mode.
 */
void mglPopMatrix()
{
    if( stack_status == "projection" )
    proj_stack.pop();
  

  else
    model_stack.pop();

}

/**
 * Replace the current matrix with the identity.
 */
void mglLoadIdentity()
{
   Matrix4 identityMatrix;
   identityMatrix.matrix4[0][0] = 1;
   identityMatrix.matrix4[1][1] = 1;
   identityMatrix.matrix4[2][2] = 1;
   identityMatrix.matrix4[3][3] = 1;

   currentMatrix = identityMatrix;


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
   Matrix4 identityMatrix;
   identityMatrix.matrix4[0][0] = 1;
   identityMatrix.matrix4[1][1] = 1;
   identityMatrix.matrix4[2][2] = 1;
   identityMatrix.matrix4[3][3] = 1;
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


  Matrix4 tmp;
  tmp.matrix4[0][3] = -(right+left)/(right-left);
  tmp.matrix4[1][3] = -(top+bottom)/(top-bottom);
  tmp.matrix4[2][3] = -(far+near)/(far-near);
  tmp.matrix4[3][3] = 1;
  tmp.matrix4[0][0] = 2/(right-left);
  tmp.matrix4[1][1] = 2/(top-bottom);
  tmp.matrix4[2][2] = 2/(far-near);

  //matrix4x4 temp3 = temp* (*current_matrix);
  Matrix4 temp3 = currentMatrix * tmp;
  currentMatrix =  temp3;

   currentMatrix.print_matrix();



}

/**
 * Set the current color for drawn shapes.
 */
void mglColor(MGLbyte red,
              MGLbyte green,
              MGLbyte blue)
{
}
