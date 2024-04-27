#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <corecrt_math_defines.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

const char* WINDOWTITLE = "Werlds";
const char* GLUITITLE = "Texture Mapping";

// ========================================================
//   /-----------\
//   | CONSTANTS |
//   \-----------/

// what the glui package defines as true and false:
const int GLUITRUE = true;
const int GLUIFALSE = false;

// the escape key:
const int ESCAPE = 0x1b;

// initial window size:
const int INIT_WINDOW_SIZE = 1024;

// size of the 3d box to be drawn:
const float BOXSIZE = 2.f;

// multiplication factors for input interaction:
//  (these are known from previous experience)
const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:
const float MINSCALE = 0.05f;

// scroll wheel button values:
const int SCROLL_WHEEL_UP = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:
const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// active mouse buttons (or them together):
const int LEFT = 4;
const int MIDDLE = 2;
const int RIGHT = 1;

// which texture setting:
enum Textures
{
	NONE,
	REGULAR,
	DISTORTED
};

// which projection:
enum Projections
{
	ORTHO,
	PERSP,
	BYZAN
};

// which view:
enum Viewpoint
{
	internalView,
	externalView
};

// which button:
enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):
const GLfloat BACKCOLOR[] = { 0., 0., 0., 1. };

// line width for the axes:
const GLfloat AXES_WIDTH = 3.;

// ========================================================
//   /------------------\
//   | THE COLOR SYSTEM |
//   \------------------/

enum Colors {
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK,
	OSU_ORANGE
};

char* ColorNames[] = {
	(char*)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta",
	(char*)"White",
	(char*)"Black",
	(char*)"OSU Orange"
};

// the color definitions:
// this order must match the menu order
const GLfloat Colors[][3] = {
	{ 1.,		0.,		0. },		// red
	{ 1.,		1.,		0. },		// yellow
	{ 0.,		1.,		0. },		// green
	{ 0.,		1.,		1. },		// cyan
	{ 0.,		0.,		1. },		// blue
	{ 1.,		0.,		1. },		// magenta
	{ 1.,		1.,		1. },		// white
	{ 0.,		0.,		0. },		// black
	{ 0.843f,	0.247f,	0.035f },	// OSU orange
};

// ========================================================
//   /----------------\
//   | FOG PARAMETERS |
//   \----------------/
const GLfloat FOGCOLOR[4] = { .0f, .0f, .0f, 1.f };
const GLenum  FOGMODE = GL_LINEAR;
const GLfloat FOGDENSITY = 0.30f;
const GLfloat FOGSTART = 1.5f;
const GLfloat FOGEND = 4.f;

// ========================================================

//   /-------------------------------\
//   | NON-CONSTANT GLOBAL VARIABLES |
//   \-------------------------------/
int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
GLuint  MoonList;				// Earth display list
GLuint  MarqueeList;			// Marquee display list
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
int		WhichColor;				// index into Colors[ ]
int		WhichTexture;			// NONE, REGULAR, OR DISTORTED
int		WhichProjection;		// ORTHO or PERSP
int		WhichViewpoint;			// internalView or externalView
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees
float	RotationAngle;			// Earth's rotation angle
bool	Frozen;					// Freeze option
unsigned char*	Texture;				// Texture, for Earth
unsigned char*	Texture2;				// 2nd Texture, for Moon
unsigned int	WorldTex;				// World Texture
unsigned int	MoonTex;				// Moon Texture
struct point2*	SphPts;					// Sphere points
int				SphNumLngs, SphNumLats;	// Number of longitudinal & latitudinal divisions on sphere

#ifndef POINT2_H
#define POINT2_H
struct point2 {
	float x, y, z;		// coordinates
	float nx, ny, nz;	// surface normal
	float s, t;			// texture coords
};

inline void DrawPoint(struct point2* p) {
	glNormal3fv(&p->nx);
	glTexCoord2fv(&p->s);
	glVertex3fv(&p->x);
}
#endif

inline struct point2* SphPtsPointer(int lat, int lng) {
	if (lat < 0) lat += (SphNumLats - 1);
	if (lng < 0) lng += (SphNumLngs - 0);
	if (lat > SphNumLats - 1) lat -= (SphNumLats - 1);
	if (lng > SphNumLngs - 1) lng -= (SphNumLngs - 0);
	return &SphPts[SphNumLngs * lat + lng];
}


// ========================================================
//   /----------------\
//   | MODEL INCLUDES |
//   \----------------/


// ========================================================
//   /---------------------\
//   | FUNCTION PROTOTYPES |
//   \---------------------/
void	Animate();
void	Display();
void	DoAxesMenu(int);
void	DoColorMenu(int);
void	DoDepthBufferMenu(int);
void	DoDepthFightingMenu(int);
void	DoDepthMenu(int);
void	DoDebugMenu(int);
void	DoMainMenu(int);
void	DoTextureMenu(int);
void	DoProjectMenu(int);
void	DoPositionMenu(int);
void	DoRasterString(float, float, float, char*);
void	DoStrokeString(float, float, float, float, char*);
float	ElapsedSeconds();
void	InitGraphics();
void	InitLists();
void	InitMenus();
void	Keyboard(unsigned char, int, int);
void	MouseButton(int, int, int, int);
void	MouseMotion(int, int);
void	Reset();
void	Resize(int, int);
void	Visibility(int);
void	OsuSphere(float radius, int slices, int stacks);
void	Axes(float);
int		ReadInt(FILE*);
short	ReadShort(FILE*);
void	HsvRgb(float[3], float[3]);
void	Cross(float[3], float[3], float[3]);
float	Dot(float[3], float[3]);
float	Unit(float[3], float[3]);
unsigned char* BmpToTexture(char*, int*, int*);


// ========================================================
//   /--------------\
//   | MAIN PROGRAM |
//   \--------------/
int main(int argc, char* argv[]) {
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)
	glutInit(&argc, argv);

	// setup all the graphics stuff:
	InitGraphics();

	// create the display structures that will not change:
	InitLists();

	// init all the global variables used by Display( ):
	// this will also post a redisplay
	Reset();

	// setup all the user interface stuff:
	InitMenus();

	// draw the scene once and wait for some interaction:
	// (this will never return)
	glutSetWindow(MainWindow);
	glutMainLoop();

	// glutMainLoop( ) never actually returns
	// the following line is here to make the compiler happy:
	return 0;
}


// ========================================================
//   /--------------------\
//   | ANIMATION CONTROLS |
//   \--------------------/

void Animate() {
	// put animation stuff in here -- change some global variables
	// for Display( ) to find:
	float Time;
	#define MS_IN_THE_ANIMATION_CYCLE	10000
	int ms = glutGet(GLUT_ELAPSED_TIME);	// in milliseconds
	ms %= MS_IN_THE_ANIMATION_CYCLE;
	//ms %= 100;
	Time = (float)ms / (float)MS_IN_THE_ANIMATION_CYCLE;        // [ 0., 1. )
	//float Time = (float)ms / (float)100;
	RotationAngle += 1.5f;

	// force a call to Display( ) next time it is convenient:
	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// ========================================================
//   /-------------------------\
//   | DRAW THE COMPLETE SCENE |
//   \-------------------------/
void Display() {
	// set which window we want to do the graphics into:
	glutSetWindow(MainWindow);

	// erase the background:
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

#ifdef DEMO_DEPTH_BUFFER
	if (DepthBufferOn == 0)
		glDisable(GL_DEPTH_TEST);
#endif

	// specify shading to be flat:
	glShadeModel(GL_FLAT);

	// set the viewport to a square centered in the window:
	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);

	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (WhichProjection == ORTHO)
		glOrtho(-250.f, 250.f, -250.f, 250.f, 0.1f, 1000.f);
	else if (WhichProjection == PERSP)
		gluPerspective(70.f, 1.f, 0.1f, 1000.f);
	else
		gluPerspective(70.f, 1.f, 1000.f, 0.1f);

	// place the objects into the scene:
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// set the eye position, look-at position, and up-vector:
	// Outside Eye Position
	if (WhichViewpoint == externalView) {
		gluLookAt(400.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 5.f, 0.f);
		// rotate the scene:
		glRotatef((GLfloat)Yrot, 0.f, 1.f, 0.f);
		glRotatef((GLfloat)Xrot, 1.f, 0.f, 0.f);
		// uniformly scale the scene:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);
	}
	// Inside Eye Position
	else {
		gluLookAt(0.f, 0.0f, 0.f, 0.f, 0.f, 10.f, 0.f, 0.5f, 0.f);
		// prevent the scene from rotating
		Yrot = 0;
		Xrot = 0;
		// maintain the scale where it was externally
		glScalef(1.f, 1.f, 1.f);
	}

	// set the fog parameters:
	if (DepthCueOn != 0) {
		glFogi(GL_FOG_MODE, FOGMODE);
		glFogfv(GL_FOG_COLOR, FOGCOLOR);
		glFogf(GL_FOG_DENSITY, FOGDENSITY);
		glFogf(GL_FOG_START, FOGSTART);
		glFogf(GL_FOG_END, FOGEND);
		glEnable(GL_FOG);
	}
	else {
		glDisable(GL_FOG);
	}

	// possibly draw the axes:
	if (AxesOn != 0) {
		glColor3fv(&Colors[WhichColor][0]);
		glCallList(AxesList);
	}

	// since we are using glScalef( ), be sure the normals get unitized:
	glEnable(GL_NORMALIZE);

	// TEXTURE SETTING:
	if (WhichTexture == NONE) {
		// Earth with no texture
		glPushMatrix();
			glTranslatef(0., 0., 0.);
			glRotatef(RotationAngle * 0.1f, 0.f, 1.f, 0.f);
			OsuSphere(100, 256, 256);
		glPopMatrix();
	}
	else if (WhichTexture == DISTORTED) {
		// Earth with distorted texture
		glPushMatrix();
			glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, WorldTex);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glTranslatef(0., 0., 0.);
				glRotatef(RotationAngle * 0.1f, 0.f, 1.f, 0.f);
				OsuSphere(100, 256, 256);
				glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}
	else {
		// Earth with normal texture
		glPushMatrix();
			glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, WorldTex);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glTranslatef(0., 0., 0.);
				glRotatef(RotationAngle * 0.1f, 0.f, 1.f, 0.f);
				OsuSphere(100, 256, 256);
			glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}

	// Moon
	glPushMatrix();
		glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, MoonTex);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glRotatef(RotationAngle * 0.2f + 90, 0.f, 1.f, 0.f);
			glTranslatef(300., 0., 0.);
			glCallList(MoonList);
		glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	/* DEMO => Z-Fighting */
	#ifdef DEMO_Z_FIGHTING
		if (DepthFightingOn != 0)
		{
			glPushMatrix();
			glRotatef(90.f, 0.f, 1.f, 0.f);
			glCallList(BoxList);
			glPopMatrix();
		}
	#endif

	// swap the double-buffered framebuffers:
	glutSwapBuffers();

	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !
	glFlush();
}


void DoAxesMenu(int id) {
	AxesOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void DoColorMenu(int id) {
	WhichColor = id - RED;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void DoDebugMenu(int id) {
	DebugOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void DoDepthBufferMenu(int id) {
	DepthBufferOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void DoDepthFightingMenu(int id) {
	DepthFightingOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void DoDepthMenu(int id) {
	DepthCueOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void DoTextureMenu(int id) {
	WhichTexture = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// ========================================================
//   /--------------------\
//   | MAIN MENU CALLBACK |
//   \--------------------/
void DoMainMenu(int id) {
	switch (id)
	{
	case RESET:
		Reset();
		break;

	case QUIT:
		// gracefully close out the graphics:
		glutSetWindow(MainWindow);
		glFinish();
		// gracefully close the graphics window:
		glutDestroyWindow(MainWindow);
		// gracefully exit the program:
		exit(0);
		break;

	default:
		fprintf(stderr, "Don't know what to do with Main Menu ID %d\n", id);
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void DoProjectMenu(int id) {
	WhichProjection = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void DoPositionMenu(int id) {
	WhichViewpoint = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


/* DISPLAY A STRING OF CHARACTERS (USING A RASTER FONT) WITH GLUT */
void DoRasterString(float x, float y, float z, char* s) {
	glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);

	char c;			// one character to print
	for (; (c = *s) != '\0'; s++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}
}


/* DISPLAY A STRING OF CHARACTERS (USING A STROKE FONT) WITH GLUT */
void DoStrokeString(float x, float y, float z, float ht, char* s) {
	glPushMatrix();
	glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
	float sf = ht / (119.05f + 33.33f);
	glScalef((GLfloat)sf, (GLfloat)sf, (GLfloat)sf);
	char c;			// one character to print
	for (; (c = *s) != '\0'; s++) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
	}
	glPopMatrix();
}


/* RETURN ELAPSED TIME IN SECONDS */
// return the number of seconds since the start of the program:
float ElapsedSeconds() {
	// get # of milliseconds since the start of the program:
	int ms = glutGet(GLUT_ELAPSED_TIME);

	// convert it to seconds:
	return (float)ms / 1000.f;
}


/* INITIALIZE THE GLUI WINDOW */
void InitMenus() {
	glutSetWindow(MainWindow);

	int numColors = sizeof(Colors) / (3 * sizeof(int));
	int colormenu = glutCreateMenu(DoColorMenu);
	for (int i = 0; i < numColors; i++) {
		glutAddMenuEntry(ColorNames[i], i);
	}

	int texturemenu = glutCreateMenu(DoTextureMenu);
	glutAddMenuEntry("No Texture", NONE);
	glutAddMenuEntry("Regular Texture", REGULAR);
	glutAddMenuEntry("Distorted Texture", DISTORTED);

	int axesmenu = glutCreateMenu(DoAxesMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthcuemenu = glutCreateMenu(DoDepthMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthbuffermenu = glutCreateMenu(DoDepthBufferMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthfightingmenu = glutCreateMenu(DoDepthFightingMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int debugmenu = glutCreateMenu(DoDebugMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int projmenu = glutCreateMenu(DoProjectMenu);
	glutAddMenuEntry("Orthographic", ORTHO);
	glutAddMenuEntry("Perspective", PERSP);
	glutAddMenuEntry("Byzantine", BYZAN);

	int viewmenu = glutCreateMenu(DoPositionMenu);
	glutAddMenuEntry("Inside View", internalView);
	glutAddMenuEntry("Outside View", externalView);

	int mainmenu = glutCreateMenu(DoMainMenu);
	glutAddSubMenu("Axes", axesmenu);
	glutAddSubMenu("Axis Colors", colormenu);

#ifdef DEMO_DEPTH_BUFFER
	glutAddSubMenu("Depth Buffer", depthbuffermenu);
#endif

#ifdef DEMO_Z_FIGHTING
	glutAddSubMenu("Depth Fighting", depthfightingmenu);
#endif

	glutAddSubMenu("Textures", texturemenu);
	glutAddSubMenu("Depth Cue", depthcuemenu);
	glutAddSubMenu("Projection", projmenu);
	glutAddSubMenu("Viewpoint", viewmenu);
	glutAddMenuEntry("Reset", RESET);
	glutAddSubMenu("Debug", debugmenu);
	glutAddMenuEntry("Quit", QUIT);

	// attach the pop-up menu to the right mouse button:
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


/* INITIALIZE THE GLUT AND OpenGL LIBRARIES */
//	also setup callback functions
void InitGraphics() {
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// set the initial window configuration:

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);

	// open the window and set its title:

	MainWindow = glutCreateWindow(WINDOWTITLE);
	glutSetWindowTitle(WINDOWTITLE);

	// set the framebuffer clear values:

	glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);


	glutSetWindow(MainWindow);
	glutDisplayFunc(Display);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc(Visibility);
	glutEntryFunc(NULL);
	glutSpecialFunc(NULL);
	glutSpaceballMotionFunc(NULL);
	glutSpaceballRotateFunc(NULL);
	glutSpaceballButtonFunc(NULL);
	glutButtonBoxFunc(NULL);
	glutDialsFunc(NULL);
	glutTabletMotionFunc(NULL);
	glutTabletButtonFunc(NULL);
	glutMenuStateFunc(NULL);
	glutTimerFunc(-1, NULL, 0);

	glutIdleFunc(Animate);

	// init the glew package (a window must be open to do this):
#ifdef WIN32
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		fprintf(stderr, "glewInit Error\n");
	}
	else {
		fprintf(stderr, "GLEW initialized OK\n");
	}
	fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	// Texture for Earth
	int width, height;
	Texture = BmpToTexture("worldtex.bmp", &width, &height);
	if (Texture == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", "worldtex.bmp");
	else
		fprintf(stderr, "Width = %d ; Height = %d\n", width, height);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &WorldTex);
	glBindTexture(GL_TEXTURE_2D, WorldTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);

	// Texture for Moon
	int width2, height2;
	Texture2 = BmpToTexture("2k_Moon.bmp", &width2, &height2);
	if (Texture2 == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", "2k_Moon.bmp");
	else
		fprintf(stderr, "Width = %d ; Height = %d\n", width2, height2);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &MoonTex);
	glBindTexture(GL_TEXTURE_2D, MoonTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture2);
}


/* INITIALIZE THE DISPLAY LIST */
void InitLists() {
	glutSetWindow(MainWindow);

	// Moon Init List
	MoonList = glGenLists(1);
	glNewList(MoonList, GL_COMPILE);
		OsuSphere(27, 128, 128);
	glEndList();

	// create the axes:
	AxesList = glGenLists(1);
	glNewList(AxesList, GL_COMPILE);
		glLineWidth(AXES_WIDTH);
			Axes(1.5);
		glLineWidth(1.);
	glEndList();
}


/* KEYBOARD CALLBACK */
void Keyboard(unsigned char c, int x, int y) {
	if (DebugOn != 0)
		fprintf(stderr, "Keyboard: '%c' (0x%0x)\n", c, c);

	switch (c)
	{
	case 'b':
	case 'B':
		WhichProjection = BYZAN;
		break;

	case 'd':
	case 'D':
		WhichTexture = DISTORTED;
		break;

	case 'e':
	case 'E':
		WhichViewpoint = externalView;
		break;

	case 'f':
	case 'F':
		Frozen = !Frozen;
		if (Frozen)
			glutIdleFunc(NULL);
		else
			glutIdleFunc(Animate);
		break;

	case 'i':
	case 'I':
		WhichViewpoint = internalView;
		break;

	case 'n':
	case 'N':
		WhichTexture = NONE;
		break;

	case 'o':
	case 'O':
		WhichProjection = ORTHO;
		break;

	case 'p':
	case 'P':
		WhichProjection = PERSP;
		break;

	case 'q':
	case 'Q':
	case ESCAPE:
		DoMainMenu(QUIT);	// will not return here
		break;				// happy compiler

	case 'r':
	case 'R':
		WhichTexture = REGULAR;
		break;

	default:
		fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c);
	}

	// force a call to Display( ):
	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


/* CALLED WHEN MOUSE BUTTON TRANSITIONS DOWN OR UP */
void MouseButton(int button, int state, int x, int y) {
	// LEFT, MIDDLE, or RIGHT
	int b = 0;

	if (DebugOn != 0)
		fprintf(stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y);

	// get the proper button bit mask:
	switch (button) {
	case GLUT_LEFT_BUTTON:
		b = LEFT;		break;

	case GLUT_MIDDLE_BUTTON:
		b = MIDDLE;		break;

	case GLUT_RIGHT_BUTTON:
		b = RIGHT;		break;

	case SCROLL_WHEEL_UP:
		Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		break;

	case SCROLL_WHEEL_DOWN:
		Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		break;

	default:
		b = 0;
		fprintf(stderr, "Unknown mouse button: %d\n", button);
	}

	// button down sets the bit, up clears the bit:
	if (state == GLUT_DOWN) {
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else {
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


/* CALLED WHEN MOUSE MOVES WHILE A BUTTON IS PRESSED */
void MouseMotion(int x, int y) {
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if ((ActiveButton & LEFT) != 0) {
		Xrot += (ANGFACT * dy);
		Yrot += (ANGFACT * dx);
	}

	if ((ActiveButton & MIDDLE) != 0) {
		Scale += SCLFACT * (float)(dx - dy);

		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE) {
			Scale = MINSCALE;
		}
	}

	// new current position
	Xmouse = x;
	Ymouse = y;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


/* RESET THE TRANSFORMATION AND THE COLORS */
void Reset() {
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Frozen = false;
	Scale = 1.0;
	ShadowsOn = 0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	WhichTexture = REGULAR;
	WhichViewpoint = externalView;
	Xrot = Yrot = 0.;
}


/* CALLED WHEN USER RESIZES THE WINDOW */
void Resize(int width, int height) {
	// don't really need to do anything since window size is
	// checked each time in Display( ):
	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


/* HANDLE CHANGES TO THE WINDOW'S VISIBILITY */
void Visibility(int state) {
	if (DebugOn != 0) {
		fprintf(stderr, "Visibility: %d\n", state);
	}
	if (state == GLUT_VISIBLE) {
		glutSetWindow(MainWindow);
		glutPostRedisplay();
	}
	else {
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}


/* OSUSphere OBJECT */
void OsuSphere(float radius, int slices, int stacks) {
	// set the globals:
	SphNumLngs = slices;
	SphNumLats = stacks;
	if (SphNumLngs < 3)
		SphNumLngs = 3;
	if (SphNumLats < 3)
		SphNumLats = 3;

	// allocate the point data structure:
	SphPts = new struct point2[SphNumLngs * SphNumLats];

	// fill the SphPts structure:
	for (int ilat = 0; ilat < SphNumLats; ilat++) {
		// ilat=0/lat=0. is the south pole
		// ilat=SphNumLats-1, lat=+M_PI/2. is the north pole
		float lat = (float)-M_PI / 2.f + (float)M_PI * (float)ilat / (SphNumLats - 1.f);

		float xz = cosf(lat);
		float  y = sinf(lat);
		for (int ilng = 0; ilng < SphNumLngs; ilng++)				// ilng=0, lng=-M_PI and
			  // ilng=SphNumLngs-1, lng=+M_PI are the same meridian
		{
			float lng = (float)-M_PI + 2.f * (float)M_PI * (float)ilng / (float)(SphNumLngs - 1.f);
			float x = xz * cosf(lng);
			float z = -xz * sinf(lng);
			struct point2* p = SphPtsPointer(ilat, ilng);
			p->x = radius * x;
			p->y = radius * y;
			p->z = radius * z;
			p->nx = x;
			p->ny = y;
			p->nz = z;
			//p->s = ((lng + (float)M_PI) / (2.f * (float)M_PI));
			//p->t = ((lat + (float)M_PI / 2.f) / (float)M_PI);
			if (WhichTexture == DISTORTED) {
				p->s = ((lng + (float)M_PI) / (2.f * (float)M_PI));
				p->t = ((lat + (ilng * 0.01f) + (float)M_PI / (2.f)) / (float)M_PI);
			}
			else {
				p->s = ((lng + (float)M_PI) / (2.f * (float)M_PI));
				p->t = ((lat + (float)M_PI / (2.f)) / (float)M_PI);
			}
		}
	}

	struct point2 top, bot;		// top, bottom points

	top.x = 0.;		top.y = radius;		top.z = 0.;
	top.nx = 0.;	top.ny = 1.;		top.nz = 0.;
	top.s = 0.;		top.t = 1.;

	bot.x = 0.;		bot.y = -radius;	bot.z = 0.;
	bot.nx = 0.;	bot.ny = -1.;		bot.nz = 0.;
	bot.s = 0.;		bot.t = 0.;

	// connect the north pole to the latitude SphNumLats-2:
	glBegin(GL_TRIANGLE_STRIP);
	for (int ilng = 0; ilng < SphNumLngs; ilng++) {
		float lng = (float)-M_PI + 2.f * (float)M_PI * (float)ilng / (float)(SphNumLngs - 1.f);
		top.s = (lng + (float)M_PI) / (2.f * (float)M_PI);
		DrawPoint(&top);
		struct point2* p = SphPtsPointer(SphNumLats - 2, ilng);	// ilat=SphNumLats-1 is the north pole
		DrawPoint(p);
	}
	glEnd();

	// connect the south pole to the latitude 1:
	glBegin(GL_TRIANGLE_STRIP);
	for (int ilng = SphNumLngs - 1; ilng >= 0; ilng--) {
		float lng = (float)-M_PI + 2.f * (float)M_PI * (float)ilng / (float)(SphNumLngs - 1.f);
		bot.s = (lng + (float)M_PI) / (2.f * (float)M_PI);
		DrawPoint(&bot);
		struct point2* p = SphPtsPointer(1, ilng);					// ilat=0 is the south pole
		DrawPoint(p);
	}
	glEnd();

	// connect the horizontal strips:
	for (int ilat = 2; ilat < SphNumLats - 1; ilat++) {
		struct point2* p;
		glBegin(GL_TRIANGLE_STRIP);
		for (int ilng = 0; ilng < SphNumLngs; ilng++)
		{
			p = SphPtsPointer(ilat, ilng);
			DrawPoint(p);
			p = SphPtsPointer(ilat - 1, ilng);
			DrawPoint(p);
		}
		glEnd();
	}

	// clean-up:
	delete[] SphPts;
	SphPts = NULL;
}


///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////

// the stroke characters 'X' 'Y' 'Z' :
static float xx[] = { 0.f, 1.f, 0.f, 1.f };
static float xy[] = { -.5f, .5f, .5f, -.5f };
static int xorder[] = { 1, 2, -3, 4 };
static float yx[] = { 0.f, 0.f, -.5f, .5f };
static float yy[] = { 0.f, .6f, 1.f, 1.f };
static int yorder[] = { 1, 2, 3, -2, 4 };
static float zx[] = { 1.f, 0.f, 1.f, 0.f, .25f, .75f };
static float zy[] = { .5f, .5f, -.5f, -.5f, 0.f, 0.f };
static int zorder[] = { 1, 2, 3, 4, -5, 6 };

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;


// ========================================================
//   /--------------\
//   | DRAW 3D AXES |
//   \--------------/

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)
void Axes(float length) {
	glBegin(GL_LINE_STRIP);
	glVertex3f(length, 0., 0.);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., length, 0.);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., 0., length);
	glEnd();

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 4; i++)
	{
		int j = xorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(base + fact * xx[j], fact * xy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 5; i++)
	{
		int j = yorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(fact * yx[j], base + fact * yy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 6; i++)
	{
		int j = zorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(0.0, fact * zy[j], base + fact * zx[j]);
	}
	glEnd();
}


// ========================================================
//   /--------------------------------\
//   | READ A BMP FILE INTO A TEXTURE |
//   \--------------------------------/

// Defines for BMP into Texture
#define VERBOSE				false
#define BMP_MAGIC_NUMBER	0x4d42
#ifndef BI_RGB
#define BI_RGB				0
#define BI_RLE8				1
#define BI_RLE4				2
#endif

/* BMP FILE HEADER (struct) */
struct bmfh {
	short bfType;		// BMP_MAGIC_NUMBER = "BM"
	int bfSize;		// size of this file in bytes
	short bfReserved1;
	short bfReserved2;
	int bfOffBytes;		// # bytes to get to the start of the per-pixel data
} FileHeader;

/* BMP INFO HEADER (struct) */
struct bmih {
	int biSize;		// info header size, should be 40
	int biWidth;		// image width
	int biHeight;		// image height
	short biPlanes;		// #color planes, should be 1
	short biBitCount;	// #bits/pixel, should be 1, 4, 8, 16, 24, 32
	int biCompression;	// BI_RGB, BI_RLE4, BI_RLE8
	int biSizeImage;
	int biXPixelsPerMeter;
	int biYPixelsPerMeter;
	int biClrUsed;		// # colors in the palette
	int biClrImportant;
} InfoHeader;

/* READ BMP FILE INTO A TEXTURE */
unsigned char* BmpToTexture(char* filename, int* width, int* height)
{
	FILE* fp;
#ifdef _WIN32
	errno_t err = fopen_s(&fp, filename, "rb");
	if (err != 0)
	{
		fprintf(stderr, "Cannot open Bmp file '%s'\n", filename);
		return NULL;
	}
#else
	fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		fprintf(stderr, "Cannot open Bmp file '%s'\n", filename);
		return NULL;
	}
#endif

	FileHeader.bfType = ReadShort(fp);


	// if bfType is not BMP_MAGIC_NUMBER, the file is not a bmp:

	if (VERBOSE) fprintf(stderr, "FileHeader.bfType = 0x%0x = \"%c%c\"\n",
		FileHeader.bfType, FileHeader.bfType & 0xff, (FileHeader.bfType >> 8) & 0xff);
	if (FileHeader.bfType != BMP_MAGIC_NUMBER)
	{
		fprintf(stderr, "Wrong type of file: 0x%0x\n", FileHeader.bfType);
		fclose(fp);
		return NULL;
	}


	FileHeader.bfSize = ReadInt(fp);
	if (VERBOSE)	fprintf(stderr, "FileHeader.bfSize = %d\n", FileHeader.bfSize);

	FileHeader.bfReserved1 = ReadShort(fp);
	FileHeader.bfReserved2 = ReadShort(fp);

	FileHeader.bfOffBytes = ReadInt(fp);


	InfoHeader.biSize = ReadInt(fp);
	InfoHeader.biWidth = ReadInt(fp);
	InfoHeader.biHeight = ReadInt(fp);

	const int nums = InfoHeader.biWidth;
	const int numt = InfoHeader.biHeight;

	InfoHeader.biPlanes = ReadShort(fp);

	InfoHeader.biBitCount = ReadShort(fp);
	if (VERBOSE)	fprintf(stderr, "InfoHeader.biBitCount = %d\n", InfoHeader.biBitCount);

	InfoHeader.biCompression = ReadInt(fp);
	if (VERBOSE)	fprintf(stderr, "InfoHeader.biCompression = %d\n", InfoHeader.biCompression);

	InfoHeader.biSizeImage = ReadInt(fp);
	if (VERBOSE)	fprintf(stderr, "InfoHeader.biSizeImage = %d\n", InfoHeader.biSizeImage);

	InfoHeader.biXPixelsPerMeter = ReadInt(fp);
	InfoHeader.biYPixelsPerMeter = ReadInt(fp);

	InfoHeader.biClrUsed = ReadInt(fp);
	if (VERBOSE)	fprintf(stderr, "InfoHeader.biClrUsed = %d\n", InfoHeader.biClrUsed);

	InfoHeader.biClrImportant = ReadInt(fp);

	// fprintf( stderr, "Image size found: %d x %d\n", ImageWidth, ImageHeight );

	// pixels will be stored bottom-to-top, left-to-right:
	unsigned char* texture = new unsigned char[3 * nums * numt];
	if (texture == NULL)
	{
		fprintf(stderr, "Cannot allocate the texture array!\n");
		return NULL;
	}

	// extra padding bytes:

	int requiredRowSizeInBytes = 4 * ((InfoHeader.biBitCount * InfoHeader.biWidth + 31) / 32);
	if (VERBOSE)	fprintf(stderr, "requiredRowSizeInBytes = %d\n", requiredRowSizeInBytes);

	int myRowSizeInBytes = (InfoHeader.biBitCount * InfoHeader.biWidth + 7) / 8;
	if (VERBOSE)	fprintf(stderr, "myRowSizeInBytes = %d\n", myRowSizeInBytes);

	int numExtra = requiredRowSizeInBytes - myRowSizeInBytes;
	if (VERBOSE)	fprintf(stderr, "New NumExtra padding = %d\n", numExtra);


	// this function does not support compression:

	if (InfoHeader.biCompression != 0)
	{
		fprintf(stderr, "Wrong type of image compression: %d\n", InfoHeader.biCompression);
		fclose(fp);
		return NULL;
	}

	// we can handle 24 bits of direct color:
	if (InfoHeader.biBitCount == 24)
	{
		rewind(fp);
		fseek(fp, FileHeader.bfOffBytes, SEEK_SET);
		int t;
		unsigned char* tp;
		for (t = 0, tp = texture; t < numt; t++)
		{
			for (int s = 0; s < nums; s++, tp += 3)
			{
				*(tp + 2) = fgetc(fp);		// b
				*(tp + 1) = fgetc(fp);		// g
				*(tp + 0) = fgetc(fp);		// r
			}

			for (int e = 0; e < numExtra; e++)
			{
				fgetc(fp);
			}
		}
	}

	// we can also handle 8 bits of indirect color:
	if (InfoHeader.biBitCount == 8 && InfoHeader.biClrUsed == 256)
	{
		struct rgba32
		{
			unsigned char r, g, b, a;
		};
		struct rgba32* colorTable = new struct rgba32[InfoHeader.biClrUsed];

		rewind(fp);
		fseek(fp, sizeof(struct bmfh) + InfoHeader.biSize - 2, SEEK_SET);
		for (int c = 0; c < InfoHeader.biClrUsed; c++)
		{
			colorTable[c].r = fgetc(fp);
			colorTable[c].g = fgetc(fp);
			colorTable[c].b = fgetc(fp);
			colorTable[c].a = fgetc(fp);
			if (VERBOSE)	fprintf(stderr, "%4d:\t0x%02x\t0x%02x\t0x%02x\t0x%02x\n",
				c, colorTable[c].r, colorTable[c].g, colorTable[c].b, colorTable[c].a);
		}

		rewind(fp);
		fseek(fp, FileHeader.bfOffBytes, SEEK_SET);
		int t;
		unsigned char* tp;
		for (t = 0, tp = texture; t < numt; t++)
		{
			for (int s = 0; s < nums; s++, tp += 3)
			{
				int index = fgetc(fp);
				*(tp + 0) = colorTable[index].r;	// r
				*(tp + 1) = colorTable[index].g;	// g
				*(tp + 2) = colorTable[index].b;	// b
			}

			for (int e = 0; e < numExtra; e++)
			{
				fgetc(fp);
			}
		}

		delete[] colorTable;
	}

	fclose(fp);

	*width = nums;
	*height = numt;
	return texture;
}

/* Read Integer ??? */
int ReadInt(FILE* fp) {
	const unsigned char b0 = fgetc(fp);
	const unsigned char b1 = fgetc(fp);
	const unsigned char b2 = fgetc(fp);
	const unsigned char b3 = fgetc(fp);
	return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}

/* Read Short ??? */
short ReadShort(FILE* fp) {
	const unsigned char b0 = fgetc(fp);
	const unsigned char b1 = fgetc(fp);
	return (b1 << 8) | b0;
}

/* CONVERT HSV TO RGB */
// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );
void HsvRgb(float hsv[3], float rgb[3]) {
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while (h >= 6.)	h -= 6.;
	while (h < 0.) 	h += 6.;

	float s = hsv[1];
	if (s < 0.)
		s = 0.;
	if (s > 1.)
		s = 1.;

	float v = hsv[2];
	if (v < 0.)
		v = 0.;
	if (v > 1.)
		v = 1.;

	// if sat==0, then is a gray:

	if (s == 0.0)
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:

	float i = (float)floor(h);
	float f = h - i;
	float p = v * (1.f - s);
	float q = v * (1.f - s * f);
	float t = v * (1.f - (s * (1.f - f)));

	float r = 0., g = 0., b = 0.;			// red, green, blue
	switch ((int)i)
	{
	case 0:
		r = v;	g = t;	b = p;
		break;

	case 1:
		r = q;	g = v;	b = p;
		break;

	case 2:
		r = p;	g = v;	b = t;
		break;

	case 3:
		r = p;	g = q;	b = v;
		break;

	case 4:
		r = t;	g = p;	b = v;
		break;

	case 5:
		r = v;	g = p;	b = q;
		break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

/* CROSS ??? */
void Cross(float v1[3], float v2[3], float vout[3]) {
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

/* DOT ??? */
float Dot(float v1[3], float v2[3]) {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

/* UNIT ??? */
float Unit(float vin[3], float vout[3]) {
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}
