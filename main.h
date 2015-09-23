//Copyright (C) 2014-2015  Corwin Hansen
#ifndef MAIN_H//include protection
	#define MAIN_H


#include <stdio.h>//input and output
#include <stdlib.h>//standard stuff
#include <math.h>//all the math functions
#include <string.h>//string related things
#include <time.h>//time on system
#include "SDL.h"//include sdl
#include "SDL_ttf.h"//include sdl_ttf
#include "SDL_image.h"//include sdl_image
#include "corwins.h"


#define FULLLSCREEN //define if fullscreen
//#define FPS //print out fps

#ifdef FULLLSCREEN
#define WINDOWFLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE    //fullscreen borderless resizable window 
#define WIDTH display.w //width and height of window
#define HEIGHT display.h
#else
#define WINDOWFLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE        //resizable window
#define WIDTH display.w/2 //width and height of window
#define HEIGHT display.h/2;
#endif


#define TEXT_INPUT_SHIFT 1 //shift to center when inputting text

#define WINDOW_NAME "Data Linearization"
#define DELAY 10 //default delay
#define DELAY_SLOW 100 //dealy while in background

//paths
#define RESOURCES "resources/" //media resources path. 
#define FONTS "fonts/" //fonts path. Inside media resources path.
#define FONT "din1451alt.ttf" //font path
#define POINT "Point.png" //player image

//What screen to display
#define DATA 0 //data input
#define GRAPH 1 //Graph

#define POINT_SIZE (1.0/75)//size of point 


int displaymode;//what screen to display


SDL_Window *window;//sdl window
SDL_Renderer *renderer;//sdl renderer
SDL_Event e;//event


int width, height, maxside;//width and hight and smallest of 2 side of window
double ws, hs;//width and height scaled to maxside
time_t lasttime;//last time mesured
unsigned long int lastframe;//last frame recorded
unsigned long int frame;//number of frames from start
unsigned long int fps;//frames per second

SDL_Texture *somethingwentwrong;//image to display if something goes wrong
SDL_Texture *PointIMG;//point image

double xpow, ypow;//x and y power
int xinv, yinv;//x and y inverse
double *Xdata;//array of x and y data
double *Ydata;
int length;//length of data
char xname[256];//name of x and y data
char yname[256];
char title[256];//title of graph

double scale;//scale up or down
double xshift;//shift in x direction
double yshift;//shift in y direction
double XShiftAll;//shift everything drawn in x direction
double YShiftAll;//shift everything drawn in y direction

char data[40][256];//array for data table

int selected;//0 for unselected, 1 for title, 2 for x axis, 3 for y axis, 4 - 44 for data.

uint32_t delay;//amount of delay for each frame
double MouseX, MouseY;//x and y position of mouse / touch normalised

int displayd;//if displayd or not

//fonts for different sizes (1/n of maxside)
TTF_Font* font_4;
TTF_Font* font_6;
TTF_Font* font_8;
TTF_Font* font_16;
TTF_Font* font_24;
TTF_Font* font_32;
TTF_Font* font_46;
TTF_Font* font_64;

#define SDL_RenderDrawLine(a, b, c, d, e) SDL_RenderDrawLine(a, b +	(XShiftAll * maxside), c +	(YShiftAll * maxside), d +	(XShiftAll * maxside), e +	(YShiftAll * maxside))

int EventFilter(void* userdata, SDL_Event* e);//event filter
void Quit(void);//quit everything
void GetDisplay(void);//get display
void Clicked(void);//x and y positions clicked
void Draged(void);//draged function to call when mouse is moved while it is down
void Resize(void);//recalculate numbers related to size and load texts
SDL_Texture* GetTexture(const char *file);//make texture from this file
TTF_Font* GetFont(const char *file, int size);//get font from file
SDL_Texture* GetTextTexture(TTF_Font* font, const char* text, int r, int g, int b);//make texture from font text and rgb
void DrawBase(void);//draw basic stuff
void DrawText(SDL_Texture *texture, double x, double y, SDL_Rect *rect, int center);//draw rect of texture at x and y position normalised. Null rect for whole texture. set center to 1 to center to x and y. Draws texture at full size
void DrawTextSelected(SDL_Texture *texture, double x, double y, SDL_Rect *rect, int center, int sel);//draw rect of texture at x and y position normalised. Null rect for whole texture. set center to 1 to center to x and y. Draws texture at full size. Add curser at end if selected value matches
void DrawTextAngled(SDL_Texture *texture, double x, double y, SDL_Rect *rect, int center, double deg);//draw rect of texture at x and y position normalised. Null rect for whole texture. set center to 1 to center to x and y. Draws texture at full size at an angle
void DrawTextAngledCentered(SDL_Texture *texture, double x, double y, SDL_Rect *rect, int centerx, int centery, double deg);//draw rect of texture at x and y position normalised. Null rect for whole texture. set center to 1 to center to x and y. Draws texture at full size at an angle
void DrawTextAngledCenteredEdge(SDL_Texture *texture, double x, double y, SDL_Rect *rect, int centerx, int centery, double deg);//draw rect of texture at x and y position normalised. Null rect for whole texture. set center to 1 to center to x and y. Draws texture at full size at an angle. Use x and y at other side
void DrawIMG(SDL_Texture *texture, double x, double y, SDL_Rect *rect, double w, double h, int center);//draw rect of texture at x and y position normalised at scale from maxside. Null rect for whole texture. set center to 1 to center to x and y
void Draw(void);//draw/update screen
oid Graph(const char* title, double x, double y, double w, double h, const char* xn, const char* yn, const double* xd, const double* yd, int dl, double xp, double yp, int xin, int yin);//draw graph with title starting from x and y position normalised at scale from maxside, and with w and h width and height with data name of xn and yn, and data of xd, and yd with length of dl. data will be modified by raising it to the power of xp and yp and making it inverse if xp or yp is 1.void Reset(void);//reset data and data manipulator

#endif
