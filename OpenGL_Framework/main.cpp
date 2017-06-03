
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glew.h>
#include <freeglut.h>
#include <iostream>
#include <vector>
#include <glm\glm.hpp>
#include "ObjectLoader.h"
#include "ShaderLoader.h"

#define _USE_MATH_DEFINES
#include <math.h>  
/* Definitions of useful mathematical constants
* M_E        - e
* M_LOG2E    - log2(e)
* M_LOG10E   - log10(e)
* M_LN2      - ln(2)
* M_LN10     - ln(10)
* M_PI       - pi
* M_PI_2     - pi/2
* M_PI_4     - pi/4
* M_1_PI     - 1/pi
* M_2_PI     - 2/pi
* M_2_SQRTPI - 2/sqrt(pi)
* M_SQRT2    - sqrt(2)
* M_SQRT1_2  - 1/sqrt(2)
*/

#define WINDOW_TITLE_PREFIX "Chapter 2"

int
CurrentWidth = 800,
CurrentHeight = 600,
WindowHandle = 0;

float windowAspect = (float)CurrentWidth / (float)CurrentHeight;

unsigned FrameCount = 0;

GLuint
ProjectionMatrixUniformLocation,
ViewMatrixUniformLocation,
ModelMatrixUniformLocation,
VertexShaderId,
FragmentShaderId,
ProgramId,
VaoId,
VboId,
ColorBufferId,
IndexBufferId;

glm::mat4
ProjectionMatrix,
ViewMatrix,
ModelMatrix;

void Initialize(int, char*[]);
void InitWindow(int, char*[]);
void ResizeFunction(int, int);
void RenderFunction(void);
void TimerFunction(int);
void IdleFunction(void);
void Cleanup(void);
void CreateVBO(void);
void DestroyVBO(void);
//void CreateShaders(void);
//void DestroyShaders(void);

int main(int argc, char* argv[])
{
	Initialize(argc, argv);

	glutMainLoop();

	exit(EXIT_SUCCESS);
}

void Initialize(int argc, char* argv[])
{
	GLenum GlewInitResult;

	InitWindow(argc, argv);

	glewExperimental = GL_TRUE;
	GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult) {
		fprintf(
			stderr,
			"ERROR: %s\n",
			glewGetErrorString(GlewInitResult)
		);
		exit(EXIT_FAILURE);
	}

	fprintf(
		stdout,
		"INFO: OpenGL Version: %s\n",
		glGetString(GL_VERSION)
	);

	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	////ExitOnGLError("ERROR: Could not set OpenGL depth testing options");

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);
	////ExitOnGLError("ERROR: Could not set OpenGL culling options");

	ModelMatrix = glm::mat4(1.0f);
	ProjectionMatrix = glm::mat4(1.0f);
	ViewMatrix = glm::mat4(1.0f);
	//ViewMatrix = glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//Load and bind shader
	ProgramId = LoadShaders("simpleVertexShader.glsl", "simpleFragmentShader.glsl");
	glUseProgram(ProgramId);
	
	//Bind Uniform MVP to shader
	ModelMatrixUniformLocation = glGetUniformLocation(ProgramId, "ModelMatrix");
	ViewMatrixUniformLocation = glGetUniformLocation(ProgramId, "ViewMatrix");
	ProjectionMatrixUniformLocation = glGetUniformLocation(ProgramId, "ProjectionMatrix");

	CreateVBO();	
}

void InitWindow(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
	);

	glutInitWindowSize(CurrentWidth, CurrentHeight);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	WindowHandle = glutCreateWindow(WINDOW_TITLE_PREFIX);

	if (WindowHandle < 1) {
		fprintf(
			stderr,
			"ERROR: Could not create a new rendering window.\n"
		);
		exit(EXIT_FAILURE);
	}

	glutReshapeFunc(ResizeFunction);
	glutDisplayFunc(RenderFunction);
	glutIdleFunc(IdleFunction);
	glutTimerFunc(0, TimerFunction, 0);
	glutCloseFunc(Cleanup);
}

void ResizeFunction(int Width, int Height)
{
	CurrentWidth = Width;
	CurrentHeight = Height;
	glViewport(0, 0, CurrentWidth, CurrentHeight);
}

void RenderFunction(void)
{
	++FrameCount;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Zum zeichnen einer Triangle-List
	//glDrawArrays(GL_TRIANGLES, 0, 48);

	//Eingabe 1: Indexed-Face-Set
	//Eingabe 2: Anzahl Triangles
	//Eingabe 3: Gibt den Datentypen der Indizierung an für: glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(int), &Indices[0], GL_STATIC_DRAW);
	//Eingabe 4: ???
	glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_INT, (GLvoid*)0);

	glutSwapBuffers();
}

void IdleFunction(void)
{
	glutPostRedisplay();
}

void TimerFunction(int Value)
{
	if (0 != Value) {
		char* TempString = (char*)
			malloc(512 + strlen(WINDOW_TITLE_PREFIX));

		sprintf(
			TempString,
			"%s: %d Frames Per Second @ %d x %d",
			WINDOW_TITLE_PREFIX,
			FrameCount * 4,
			CurrentWidth,
			CurrentHeight
		);

		glutSetWindowTitle(TempString);
		free(TempString);
	}

	FrameCount = 0;
	glutTimerFunc(250, TimerFunction, 1);
}

void Cleanup(void)
{
	//DestroyShaders();
	DestroyVBO();
}

void CreateVBO(void)
{

	std::vector<glm::vec4> Vertices;
	std::vector<int> Indices;

	loadIndexedFaceObj("cube.obj", Vertices, Indices);

	float Colors[] = {
		0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,

		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f
	};

	GLenum ErrorCheckValue = glGetError();

	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);

	glGenBuffers(1, &VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(glm::vec4), &Vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &ColorBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &IndexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(int), &Indices[0], GL_STATIC_DRAW);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create a VBO: %s \n",
			gluErrorString(ErrorCheckValue)
		);

		exit(-1);
	}
}

void DestroyVBO(void)
{
	GLenum ErrorCheckValue = glGetError();

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &ColorBufferId);
	glDeleteBuffers(1, &VboId);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &IndexBufferId);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not destroy the VBO: %s \n",
			gluErrorString(ErrorCheckValue)
		);

		exit(-1);
	}
}