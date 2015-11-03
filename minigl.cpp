/**
 * minigl.cpp
 * -------------------------------
 * Implement miniGL here.
 * Do not use any additional files
 */

#include <cstdio>
#include <vector>
#include <cmath>
#include <math.h>
#include <stack>
#include <cstdlib>
#include <string>
#include <iostream>
#include "minigl.h"

using namespace std;
class Vertex3{ //store vertex information here!
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

class RGB{

public:
   MGLbyte R;
   MGLbyte G;
   MGLbyte B;

    
 RGB(){
      R=0;
      G=0;
      B=0;
   } 
  RGB(MGLbyte r, MGLbyte g, MGLbyte b){
      R=r;
      G=g;
      B=b;
   } 

    RGB operator*(double s){  //needed for barcentric color fillings!
      return RGB( MGLbyte(R*s), MGLbyte(G*s), MGLbyte(B*s) ); 
    }
    
    RGB operator+(RGB r){ 
      return RGB( MGLbyte(r.R+R), MGLbyte(r.G + G), MGLbyte(r.B + B) ); 
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

  // = operator
  Matrix4 operator=(const Matrix4 &m){
    for(int i = 0; i < 4;i++)
      for(int j = 0; j < 4; j++)
        matrix4[i][j] = m.matrix4[i][j];
    
    return *this;
  }
  
  // taken from https://msdn.microsoft.com/en-us/library/hh873134.aspx
  //multiply operator
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


  void print_matrix(string str){ 
    for(int y=0; y< 4; ++y){
      cout <<  endl;
      for(int x= 0 ; x <4 ; ++x){
        cout << matrix4[x][y] << "    ";
      }
    }

    cout << endl << str << endl << endl;;
  }
};

//variables
MGLpoly_mode mgl_mode; //triangles or squares?
bool isDrawing;
vector <Vertex3> points_array; 

MGLpixel SCREEN_WIDTH = 320;
MGLpixel SCREEN_HEIGHT = 240;
MGLpixel screenBuffer[320][240];
float zBuffer[320][240];
RGB current_color; //current color of the points we are drawing
vector < RGB > colorBuffer;  //color stack

string stack_status; //Projection or modelview at the current moment?

stack <Matrix4> proj_stack; 
stack <Matrix4> model_stack;

//out current matrix on top of the stack
//all transformations will be applied to this matrix!
Matrix4 currentMatrix;

//HELPER FUNCTIONS

float slope(int x, int y, int x2, int y2){
  return (y2 - y)/(x2-x);
}

/** 
    set pixel (x,y) to RGB color on frame buffer to draw 
  */ 

void initBuffers(){
    for(unsigned x = 0; x < SCREEN_WIDTH; x++) 
      for(unsigned y = 0; y < SCREEN_HEIGHT; ++y) 
        zBuffer[x][y] = 999.0;
}

void set_pixel(unsigned int x, unsigned int y, float z , RGB coloring) 
{ 


  if( z <= zBuffer[x][y]){

    
    zBuffer[x][y] = z;   	
    MGLpixel color = 0; 
   	MGL_SET_RED(color, coloring.R); 
   	MGL_SET_GREEN(color, coloring.G); 
   	MGL_SET_BLUE(color, coloring.B); 
    screenBuffer[x][y] = color; //draw everything queued up on buffer
  }
}  

//from  http://www.geeksforgeeks.org/check-whether-a-given-point-lies-inside-a-triangle-or-not/
/* A utility function to calculate area of triangle formed by (x1, y1), 
   (x2, y2) and (x3, y3) */

 double implicitEq(float x, float y, float x1, float y1, float x2, float y2){
   return ( ((y1-y2)*x) + ((x2-x1)*y) +(x1*y2)-(x2*y1));
 }
/* A function to check whether point P(x, y) lies inside the triangle formed 
   by A(x1, y1), B(x2, y2) and C(x3, y3) */

//needed for example 6 (handles shading)
void isInsideTriColor(float x1, float y1, float z1,  float x2, float y2, float z2, float x3, float y3, float z3, int x, int y, RGB c1 , RGB c2, RGB c3)
{   
    //calculate baycentric coordinates
    double alpha = implicitEq(x,y,x2,y2,x3,y3) / implicitEq(x1,y1,x2,y2,x3,y3);
    double beta = implicitEq(x,y,x3,y3,x1,y1) / implicitEq(x2,y2,x3,y3,x1,y1);
    double gamma = 1 - alpha - beta;

    //double gamma = implicitEq(x,y,x1,y1,x2,y2) / implicitEq(x3,y3,x1,y1,x2,y2);

    //find z and color info using barycentric info
    current_color =  c1 *alpha  + c2* beta +  c3*gamma;
    float z = (z1 *alpha + z2*beta + z3* gamma) ;


    //See if point must be colored
    if(alpha >= 0 && beta >= 0 && gamma >= 0){
        set_pixel(x,y,z,current_color);
 }
}

//basic one for single color shapes
void isInsideTri(float x1, float y1, float z1,  float x2, float y2, float z2, float x3, float y3, float z3, int x, int y)
{   

    //calculate baycentric coordinates
    double alpha = implicitEq(x,y,x2,y2,x3,y3) / implicitEq(x1,y1,x2,y2,x3,y3);
    double beta = implicitEq(x,y,x3,y3,x1,y1) / implicitEq(x2,y2,x3,y3,x1,y1);
    double gamma = 1 - alpha - beta;

    //See if point must be colored
    if(alpha >= 0 && beta >= 0 && gamma >= 0){

        float z = (z1 *alpha + z2*beta + z3* gamma) ;
        set_pixel(x,y,z,current_color);
 }
}


void draw(){

    if( mgl_mode == MGL_TRIANGLES){

        //plot every 3 points a triangle
       for(unsigned int i = 0; i < points_array.size(); i = i+3){

        float x1 = points_array[i].x;
        float y1 = points_array[i].y;
        float z1 = points_array[i].z;

        float x2 = points_array[i+1].x;
        float y2 = points_array[i+1].y;
        float z2 = points_array[i+1].z;

        float x3 = points_array[i+2].x;
        float y3 = points_array[i+2].y;
        float z3 = points_array[i+2].z;

        RGB color_1 = colorBuffer[i];
        RGB color_2 = colorBuffer[i+1];
        RGB color_3 = colorBuffer[i+2];


        //check baycentric coordinates
        for(unsigned x = 0; x < SCREEN_WIDTH; x++) {
          for(unsigned y = 0; y < SCREEN_HEIGHT; ++y) {
              isInsideTriColor(x1,y1,z1, x2,y2, z2, x3,y3, z3, x,y, color_1, color_2, color_3 ) ;
          }
        }
      }
      
  }

  //should be in a for loop but no needd to for this assignment
  if( mgl_mode == MGL_QUADS){
      float x1 = points_array[0].x;
      float y1 = points_array[0].y;
      float z1 = points_array[0].z;

      float x2 = points_array[1].x;
      float y2 = points_array[1].y;
      float z2 = points_array[1].z;

      float x3 = points_array[2].x;
      float y3 = points_array[2].y;
      float z3 = points_array[2].z;
      
      float x4 = points_array[3].x;
      float y4 = points_array[3].y;
      float z4 = points_array[3].z;

      //next shade in triangle!
      //check baycentric coordinates
      for(unsigned x = 0; x < SCREEN_WIDTH; x++) {
        for(unsigned y = 0; y < SCREEN_HEIGHT; ++y) {
            isInsideTri(x1,y1,z1, x2,y2, z2, x3,y3, z3, x,y ) ;
            isInsideTri(x1,y1,z1, x4,y4, z4, x3,y3, z3, x,y ) ;

        }
      }
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
  // go through each pixel and set its coordinate x,y to screenBuffer color
	for(unsigned x = 0; x < width; x++) 
 		for(unsigned y = 0; y < height; ++y) 
 			data[y*width+x] = screenBuffer[x][y]; 
}

/**
 * Start specifying the vertices for a group of primitives,
 * whose type is specified by the given mode.
 */
void mglBegin(MGLpoly_mode mode)
{
    isDrawing = true;
    mgl_mode = mode;
}
/**
 * St specifying the vertices for a group of primitives.
 */
void mglEnd()
{
    draw();

    isDrawing = false;
    points_array.clear();
    colorBuffer.clear();

}
Vertex3 convert_to_screen(MGLfloat x, MGLfloat y, MGLfloat z){

  //Create a 4x4 matrix with xyz
  Matrix4 tmp;
  tmp.matrix4[3][0] = x;
  tmp.matrix4[3][1] = y;
  tmp.matrix4[3][2] = z;
  tmp.matrix4[3][3] = 1;

  //load projection matrix on t of projectional stack (gonna be orthogonal matrix or Frustum matrix)

  Matrix4 proj = proj_stack.top();
  //Scale screen to 0,0 -> 2,2
  Matrix4 scaler;
  scaler.matrix4[0][0] = SCREEN_WIDTH/2; 
  scaler.matrix4[1][1] = SCREEN_HEIGHT/2;
  scaler.matrix4[2][2] = 1; 
  scaler.matrix4[3][3] = 1;////
  
  //Load matrix that translate screen up 1 r 1
  Matrix4 translater;
  translater.matrix4[0][0] = 1;
  translater.matrix4[1][1] = 1;
  translater.matrix4[2][2] = 1;
  translater.matrix4[3][3] = 1;
  translater.matrix4[3][0] = 1;
  translater.matrix4[3][1] = 1;
  translater.matrix4[3][2] = 0;
  translater.matrix4[3][3] = 1;
  
  tmp = tmp * currentMatrix; //transfomation matrix
  tmp = tmp * proj; //
  tmp = tmp * translater;
  tmp = tmp * scaler;

  float xWidth =  tmp.matrix4[3][0] / tmp.matrix4[3][3];
  if(xWidth > SCREEN_WIDTH){
    xWidth = SCREEN_WIDTH;
  }

  return Vertex3( xWidth , tmp.matrix4[3][1]/tmp.matrix4[3][3] , tmp.matrix4[3][2]/tmp.matrix4[3][3] );

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
    Vertex3 vertex = convert_to_screen( x , y, 0);

    //Vertex3 vertex (x ,y ,0);
    points_array.push_back(vertex);
    colorBuffer.push_back(current_color);

    cout << "X: " << vertex.x << " Y: " << vertex. y << " z: " << vertex.z << endl;
}
/**
 * Specify a three-dimensional vertex.  Must appear between
 * calls to mglBegin() and mglEnd().
 */
void mglVertex3(MGLfloat x,
                MGLfloat y,
                MGLfloat z)
{
    Vertex3 vertex = convert_to_screen( x , y, z);
    points_array.push_back(vertex);
    colorBuffer.push_back(current_color);

    cout << "X: " << vertex.x << " Y: " << vertex. y << " z: " << vertex.z << endl;
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
    initBuffers();
    stack_status = "projection";
  }
}
/**
 * Push a copy of the current matrix onto the stack for the
 * current matrix mode.
 */
void mglPushMatrix()
{

  //how my current matrix works 

    /* 
      Current Matrix -> located outside the stack
      
      STACK
      M1
      m2
      ...

    */


  if( stack_status == "projection" )
    proj_stack.push(currentMatrix);
  
  else if  ( stack_status == "modelview" )
    model_stack.push(currentMatrix);

  else
    return;
}

/**
 * Pop the t matrix from the stack for the current matrix
 * mode.
 */
void mglPopMatrix()
{  // Set current matrix to top of stack! 
    if( model_stack.empty() ||  proj_stack.empty() ){
      cout << "ERROR, no matrix available";
      exit(1);
    }
    if( stack_status == "projection" ){
       currentMatrix = proj_stack.top();
       proj_stack.pop();
    }
   
    if( stack_status == "modelview" ){
      currentMatrix = model_stack.top();
      model_stack.pop();
  }
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

 //NOT USED
void mglLoadMatrix(const MGLfloat *matrix)
{}

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

 //not used!
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
  Matrix4 translater;
  translater.matrix4[0][0] = 1;
  translater.matrix4[1][1] = 1; //create translation matrix
  translater.matrix4[2][2] = 1;
  translater.matrix4[3][3] = 1;
  translater.matrix4[3][0] = x;
  translater.matrix4[3][1] = y;
  translater.matrix4[3][2] = z;
  currentMatrix =  translater * currentMatrix;
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
  
  float n =  sqrt(x*x + y*y + z*z);

  //first convert xyz to unit vector
  x = x/n;
  y = y/n;
  z = z/n;

  angle = angle * M_PI/180;

  Matrix4 rotater;
  rotater.matrix4[0][0] = x * x *( 1- cos(angle)) + cos(angle);
  rotater.matrix4[1][0] = x * y *( 1- cos(angle)) - z* sin(angle);
  rotater.matrix4[2][0] = x * z *( 1- cos(angle)) + y* sin(angle);

  rotater.matrix4[0][1] = x * y *( 1- cos(angle)) + z* sin(angle);
  rotater.matrix4[1][1] = y * y *( 1- cos(angle)) + cos(angle);
  rotater.matrix4[2][1] = z * y *( 1- cos(angle)) - x* sin(angle);

  rotater.matrix4[0][2] = z * x *( 1- cos(angle)) - y* sin(angle);
  rotater.matrix4[1][2] = z * y *( 1- cos(angle)) + x* sin(angle);
  rotater.matrix4[2][2] = z * z *( 1- cos(angle)) + cos(angle);
  rotater.matrix4[3][3] = 1;

  currentMatrix =  rotater * currentMatrix;

  //need to rotate around vector (x,y,z)
  //see glRotatef on opengl docs
}

/**
 * Multiply the current matrix by the scale matrix
 * for the given scale factors.
 */
void mglScale(MGLfloat x,
              MGLfloat y,
              MGLfloat z)
{
  Matrix4 scaler;
  scaler.matrix4[0][0] = x; 
  scaler.matrix4[1][1] = y;
  scaler.matrix4[2][2] = z; 
  scaler.matrix4[3][3] = 1;
  currentMatrix = scaler * currentMatrix;
}
/**
 * Multiply the current matrix by the perspective matrix
 * with the given clipping plane coordinates.
 */

 //includes z axis for clipping purposes
void mglFrustum(MGLfloat l,
                MGLfloat r,
                MGLfloat b,
                MGLfloat t,
                MGLfloat n,
                MGLfloat f)
{
  Matrix4 tmp;
  tmp.matrix4[0][0] = 2*n/(r-l);
  tmp.matrix4[1][1] = 2*n/(t-b);
  tmp.matrix4[2][2] = -(f+n)/(f-n);
  tmp.matrix4[2][0] = (r+l)/(r-l);
  tmp.matrix4[2][1] = (t+b)/(t-b);
  tmp.matrix4[2][3] = -1;
  tmp.matrix4[3][2] = (-2*f*n)/(f-n);
  currentMatrix = tmp * currentMatrix ;
}

/**
 * Multiply the current matrix by the orthographic matrix
 * with the given clipping plane coordinates.
 */
void mglOrtho(MGLfloat l,
              MGLfloat r,
              MGLfloat b,
              MGLfloat t,
              MGLfloat n,
              MGLfloat f)
{
  Matrix4 tmp;
  tmp.matrix4[3][0] = -(r+l)/(r-l);
  tmp.matrix4[3][1] = -(t+b)/(t-b);
  tmp.matrix4[3][2] = -(f+n)/(f-n);
  tmp.matrix4[3][3] = 1;
  tmp.matrix4[0][0] = 2/(r-l);
  tmp.matrix4[1][1] = 2/(t-b);
  tmp.matrix4[2][2] = -2/(f-n);

  currentMatrix = tmp * currentMatrix;
}

/**
 * Set the current color for drawn shapes.
 */
void mglColor(MGLbyte red,
              MGLbyte green,
              MGLbyte blue)
{
  current_color.R = red;  //set colors of current color to RGB vals
  current_color.G = green;  
  current_color.B = blue;

}
