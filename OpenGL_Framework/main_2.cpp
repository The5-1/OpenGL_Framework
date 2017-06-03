//http://openglbook.com/chapter-1-getting-started.html


#include <stdio.h>
#include <glew.h>
#include <freeglut.h>
#include <glm\glm.hpp>
#include <vector>
#include "math.h"
#include "ShaderLoader.h"
#include "ObjectLoader.h"
#include "Utils.h"
#include <iostream>

#define WINDOW_TITLE_PREFIX "Chapter 1"

int
CurrentWidth = 800,
CurrentHeight = 600,
WindowHandle = 0;

unsigned FrameCount = 0;

GLuint
ProjectionMatrixUniformLocation,
ViewMatrixUniformLocation,
ModelMatrixUniformLocation,
ProgramId,
BufferIds[3] = { 0 };
//ShaderIds[3] = { 0 };

GLuint
VertexArrayObjectId, //Vertex Array Object
VertexBufferObjectId, //Vertex Buffer Object
ColorBufferId,
IndexBufferId;

int nrFaces = -1;
std::vector<glm::vec4> vertices;
std::vector<glm::vec4> colors;
std::vector<int> faces;

Matrix
ProjectionMatrix,
ViewMatrix,
ModelMatrix;

float CubeRotation = 0;
clock_t LastTime = 0;

void Initialize(int, char*[]);
void InitWindow(int, char*[]);
void ResizeFunction(int, int);
void RenderFunction(void);
void TimerFunction(int);
void IdleFunction(void);
//void CreateRaptor(void);
void CreateCube(void);
void DestroyCube(void);
void DrawCube(void);

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

	glGetError();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	ExitOnGLError("ERROR: Could not set OpenGL depth testing options");

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	ExitOnGLError("ERROR: Could not set OpenGL culling options");

	ModelMatrix = IDENTITY_MATRIX;
	ProjectionMatrix = IDENTITY_MATRIX;
	ViewMatrix = IDENTITY_MATRIX;
	TranslateMatrix(&ViewMatrix, 0, 0, -2);

	CreateCube();
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
	
	//glutCloseFunc(DestroyCube);
}

void ResizeFunction(int Width, int Height)
{
	CurrentWidth = Width;
	CurrentHeight = Height;
	glViewport(0, 0, CurrentWidth, CurrentHeight);
	ProjectionMatrix =
		CreateProjectionMatrix(
			60,
			(float)CurrentWidth / CurrentHeight,
			1.0f,
			100.0f
		);

	glUseProgram(ProgramId);
	glUniformMatrix4fv(ProjectionMatrixUniformLocation, 1, GL_FALSE, ProjectionMatrix.m);
	glUseProgram(0);
}

void RenderFunction(void)
{
	++FrameCount;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawCube();

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

void CreateCube()
{

	loadIndexedFaceObj("raptor.obj", vertices, faces);
	nrFaces = faces.size() / 3;
	//for (int i = 0; i < vertices.size(); i++) {
	//	colors.push_back(glm::vec4(1.0f));
	//}

	/*
	for (int i = 0; i < faces.size(); i++) {
		//std::cout << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << " " << vertices[i].w << std::endl;
		std::cout << faces[i] << std::endl;
		
	}
	std::cout << faces.size() / 3 << std::endl;
	*/

	/*const Vertex VERTICES[8] =
	{
		{ { -.5f, -.5f,  .5f, 1 },{ 0, 0, 1, 1 } },
		{ { -.5f,  .5f,  .5f, 1 },{ 1, 0, 0, 1 } },
		{ { .5f,  .5f,  .5f, 1 },{ 0, 1, 0, 1 } },
		{ { .5f, -.5f,  .5f, 1 },{ 1, 1, 0, 1 } },
		{ { -.5f, -.5f, -.5f, 1 },{ 1, 1, 1, 1 } },
		{ { -.5f,  .5f, -.5f, 1 },{ 1, 0, 0, 1 } },
		{ { .5f,  .5f, -.5f, 1 },{ 1, 0, 1, 1 } },
		{ { .5f, -.5f, -.5f, 1 },{ 0, 0, 1, 1 } }
	};

	const GLuint INDICES[36] =
	{
		0,2,1,  0,3,2,
		4,3,0,  4,7,3,
		4,1,5,  4,0,1,
		3,6,2,  3,7,6,
		1,6,5,  1,2,6,
		7,5,6,  7,4,5
	};*/

	//Load and bind shader
	ProgramId = LoadShaders("simpleVertexShader.glsl", "simpleFragmentShader.glsl");

	//Bind Uniform MVP to shader
	ModelMatrixUniformLocation = glGetUniformLocation(ProgramId, "ModelMatrix");
	ViewMatrixUniformLocation = glGetUniformLocation(ProgramId, "ViewMatrix");
	ProjectionMatrixUniformLocation = glGetUniformLocation(ProgramId, "ProjectionMatrix");
	ExitOnGLError("ERROR: Could not get shader uniform locations");

	glGenVertexArrays(1, &VertexArrayObjectId);
	glBindVertexArray(VertexArrayObjectId);

	glGenBuffers(1, &VertexBufferObjectId);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObjectId);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &ColorBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), &colors[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size()*sizeof(int), &faces[0], GL_STATIC_DRAW);

	//Verhindert änderungen am VertexArrayObject
	glBindVertexArray(0);

	//glGenVertexArrays(1, &BufferIds[0]);
	//ExitOnGLError("ERROR: Could not generate the VAO");
	//glBindVertexArray(BufferIds[0]);
	//ExitOnGLError("ERROR: Could not bind the VAO");

	//glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	//ExitOnGLError("ERROR: Could not enable vertex attributes");

	//glGenBuffers(2, &BufferIds[1]);
	//ExitOnGLError("ERROR: Could not generate the buffer objects");

	//glBindBuffer(GL_ARRAY_BUFFER, BufferIds[1]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);
	//ExitOnGLError("ERROR: Could not bind the VBO to the VAO");


	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VERTICES[0]), (GLvoid*)0);
	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VERTICES[0]), (GLvoid*)sizeof(VERTICES[0].Position));
	//ExitOnGLError("ERROR: Could not set VAO attributes");

	////WICHTIG: GL_ELEMENT_ARRAY_BUFFER zeigt an, dass es ein indez array ist (für indexed face set)
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferIds[2]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES, GL_STATIC_DRAW);
	//ExitOnGLError("ERROR: Could not bind the IBO to the VAO");

	//glBindVertexArray(0);
}

void DestroyCube()
{
	glDeleteProgram(ProgramId);
	ExitOnGLError("ERROR: Could not destroy the shaders");

	glDeleteBuffers(2, &BufferIds[1]);
	glDeleteVertexArrays(1, &BufferIds[0]);
	ExitOnGLError("ERROR: Could not destroy the buffer objects");
}

void DrawCube(void)
{
	float CubeAngle;
	clock_t Now = clock();

	if (LastTime == 0)
		LastTime = Now;

	CubeRotation += 45.0f * ((float)(Now - LastTime) / CLOCKS_PER_SEC);
	CubeAngle = DegreesToRadians(CubeRotation);
	LastTime = Now;

	ModelMatrix = IDENTITY_MATRIX;
	RotateAboutY(&ModelMatrix, CubeAngle);
	RotateAboutX(&ModelMatrix, CubeAngle);

	glUseProgram(ProgramId);
	ExitOnGLError("ERROR: Could not use the shader program");

	glUniformMatrix4fv(ModelMatrixUniformLocation, 1, GL_FALSE, ModelMatrix.m);
	glUniformMatrix4fv(ViewMatrixUniformLocation, 1, GL_FALSE, ViewMatrix.m);
	ExitOnGLError("ERROR: Could not set the shader uniforms");

	glBindVertexArray(VertexArrayObjectId);
	//glBindVertexArray(BufferIds[0]);
	ExitOnGLError("ERROR: Could not bind the VAO for drawing purposes");

	glDrawElements(GL_TRIANGLES, nrFaces, GL_UNSIGNED_INT, (GLvoid*)0);
	ExitOnGLError("ERROR: Could not draw the cube");

	glBindVertexArray(0);
	glUseProgram(0);
}

//void CreateRaptor()
//{
//
//	std::vector<glm::vec3> vertices;
//	bool res2 = loadSimpleObj("raptor.obj", vertices);
//
//	const Vertex VERTICES[8] =
//	{
//		{ { -.5f, -.5f,  .5f, 1 },{ 0, 0, 1, 1 } },
//		{ { -.5f,  .5f,  .5f, 1 },{ 1, 0, 0, 1 } },
//		{ { .5f,  .5f,  .5f, 1 },{ 0, 1, 0, 1 } },
//		{ { .5f, -.5f,  .5f, 1 },{ 1, 1, 0, 1 } },
//		{ { -.5f, -.5f, -.5f, 1 },{ 1, 1, 1, 1 } },
//		{ { -.5f,  .5f, -.5f, 1 },{ 1, 0, 0, 1 } },
//		{ { .5f,  .5f, -.5f, 1 },{ 1, 0, 1, 1 } },
//		{ { .5f, -.5f, -.5f, 1 },{ 0, 0, 1, 1 } }
//	};
//
//	const GLuint INDICES[36] =
//	{
//		0,2,1,  0,3,2,
//		4,3,0,  4,7,3,
//		4,1,5,  4,0,1,
//		3,6,2,  3,7,6,
//		1,6,5,  1,2,6,
//		7,5,6,  7,4,5
//	};
//
//	/*ShaderIds[0] = glCreateProgram();
//	ExitOnGLError("ERROR: Could not create the shader program");
//	{
//		ShaderIds[1] = LoadShader("simpleFragmentShader.glsl", GL_FRAGMENT_SHADER);
//		ShaderIds[2] = LoadShader("simpleVertexShader.glsl", GL_VERTEX_SHADER);
//		glAttachShader(ShaderIds[0], ShaderIds[1]);
//		glAttachShader(ShaderIds[0], ShaderIds[2]);
//	}
//	glLinkProgram(ShaderIds[0]);
//	ExitOnGLError("ERROR: Could not link the shader program");
//	*/
//
//	GLuint ProgramId = LoadShaders("simpleVertexShader.glsl", "simpleFragmentShader.glsl");
//
//	//Bind Uniforms to
//	ModelMatrixUniformLocation = glGetUniformLocation(ProgramId, "ModelMatrix");
//	ViewMatrixUniformLocation = glGetUniformLocation(ProgramId, "ViewMatrix");
//	ProjectionMatrixUniformLocation = glGetUniformLocation(ProgramId, "ProjectionMatrix");
//	ExitOnGLError("ERROR: Could not get shader uniform locations");
//
//	glGenVertexArrays(1, &BufferIds[0]);
//	ExitOnGLError("ERROR: Could not generate the VAO");
//	glBindVertexArray(BufferIds[0]);
//	ExitOnGLError("ERROR: Could not bind the VAO");
//
//	glEnableVertexAttribArray(0);
//	glEnableVertexAttribArray(1);
//	ExitOnGLError("ERROR: Could not enable vertex attributes");
//
//	glGenBuffers(2, &BufferIds[1]);
//	ExitOnGLError("ERROR: Could not generate the buffer objects");
//
//	glBindBuffer(GL_ARRAY_BUFFER, BufferIds[1]);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);
//	ExitOnGLError("ERROR: Could not bind the VBO to the VAO");
//
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VERTICES[0]), (GLvoid*)0);
//	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VERTICES[0]), (GLvoid*)sizeof(VERTICES[0].Position));
//	ExitOnGLError("ERROR: Could not set VAO attributes");
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferIds[2]);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES, GL_STATIC_DRAW);
//	ExitOnGLError("ERROR: Could not bind the IBO to the VAO");
//
//	glBindVertexArray(0);
//}

//typedef struct
//{
//	float XYZW[4];
//	float RGBA[4];
//} Vertex;
//
//int
//CurrentWidth = 800,
//CurrentHeight = 600,
//WindowHandle = 0;
//
//unsigned FrameCount = 0;
//
//glm::mat4
//ProjectionMatrix,
//ViewMatrix,
//ModelMatrix;
//
//GLuint
//ProjectionMatrixUniformLocation,
//ViewMatrixUniformLocation,
//ModelMatrixUniformLocation,
//BufferIds[3] = { 0 },
//ShaderIds[3] = { 0 };
//
//GLuint
//VertexShaderId,
//FragmentShaderId,
//ProgramId,
//VaoId,
//VboId;
//
//void Initialize(int, char*[]);
//void InitWindow(int, char*[]);
//void ResizeFunction(int, int);
//void renderScene(void);
//void RenderFunction(void);
//void TimerFunction(int);
//void IdleFunction(void);
//void Cleanup(void);
//void CreateVBO(void);
//void DestroyVBO(void);
//void ExitOnGLError(const char* error_message);
//
//int main(int argc, char* argv[])
//{
//	Initialize(argc, argv);
//
//	glutMainLoop();
//
//	exit(EXIT_SUCCESS);
//}
//
//void InitWindow(int argc, char* argv[])
//{
//	glutInit(&argc, argv);
//
//	glutInitContextVersion(4, 0);
//	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
//	glutInitContextProfile(GLUT_CORE_PROFILE);
//
//	glutSetOption(
//		GLUT_ACTION_ON_WINDOW_CLOSE,
//		GLUT_ACTION_GLUTMAINLOOP_RETURNS
//	);
//
//	glutInitWindowSize(CurrentWidth, CurrentHeight);
//
//	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
//
//	WindowHandle = glutCreateWindow(WINDOW_TITLE_PREFIX);
//
//	if (WindowHandle < 1) {
//		fprintf(
//			stderr,
//			"ERROR: Could not create a new rendering window.\n"
//		);
//		exit(EXIT_FAILURE);
//	}
//
//	glutDisplayFunc(RenderFunction);
//	glutReshapeFunc(ResizeFunction);
//
//	glutIdleFunc(IdleFunction);
//	glutTimerFunc(0, TimerFunction, 0);
//	glutCloseFunc(Cleanup);
//}
//
//void Initialize(int argc, char* argv[])
//{
//	GLenum GlewInitResult;
//
//	InitWindow(argc, argv);
//
//	glewExperimental = GL_TRUE;
//	GlewInitResult = glewInit();
//
//	if (GLEW_OK != GlewInitResult) {
//		fprintf(
//			stderr,
//			"ERROR: %s\n",
//			glewGetErrorString(GlewInitResult)
//		);
//		exit(EXIT_FAILURE);
//	}
//
//	fprintf(
//		stdout,
//		"INFO: OpenGL Version: %s\n",
//		glGetString(GL_VERSION)
//	);
//
//	ProgramId = LoadShaders("simpleVertexShader.glsl", "simpleFragmentShader.glsl");
//	glUseProgram(ProgramId);
//	CreateVBO();
//
//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
//	ExitOnGLError("ERROR: Could not set OpenGL depth testing options");
//
//	glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK);
//	glFrontFace(GL_CCW);
//	ExitOnGLError("ERROR: Could not set OpenGL culling options");
//
//	ModelMatrix = glm::mat4(1.0f);
//	ProjectionMatrix = glm::mat4(1.0f);
//	ViewMatrix = glm::mat4(1.0f);
//	//TranslateMatrix(&ViewMatrix, 0, 0, -2);
//}
//
//void renderScene(void) {
//
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	glBegin(GL_TRIANGLES);
//	glVertex3f(-2, -2, -5.0);
//	glVertex3f(2, 0.0, -5.0);
//	glVertex3f(0.0, 2, -5.0);
//	glEnd();
//
//	glutSwapBuffers();
//}
//
///*
//void ResizeFunction(int Width, int Height)
//{
//CurrentWidth = Width;
//CurrentHeight = Height;
//glViewport(0, 0, CurrentWidth, CurrentHeight);
//}
//*/
//
//void ResizeFunction(int width, int height) {
//
//	// Prevent a divide by zero, when window is too short
//	// (you cant make a window of zero width).
//	if (height == 0)
//		height = 1;
//	float ratio = width * 1.0 / height;
//
//	// Use the Projection Matrix
//	glMatrixMode(GL_PROJECTION);
//
//	// Reset Matrix
//	glLoadIdentity();
//
//	// Set the viewport to be the entire window
//	glViewport(0, 0, width, height);
//
//	// Set the correct perspective.
//	gluPerspective(45.0f, ratio, 0.1f, 100.0f);
//
//	// Get Back to the Modelview
//	glMatrixMode(GL_MODELVIEW);
//}
//
//void RenderFunction(void)
//{
//	++FrameCount;
//
//	// Clear Color and Depth Buffers
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	// Reset transformations
//	glLoadIdentity();
//
//	// Set the camera
//	gluLookAt(0.0f, 0.0f, 10.0f,
//		0.0f, 0.0f, 0.0f,
//		0.0f, 1.0f, 0.0f);
//
//	glDrawArrays(GL_TRIANGLES, 0, 3);
//
//	glutSwapBuffers();
//}
//
//void IdleFunction(void)
//{
//	glutPostRedisplay();
//}
//
//void TimerFunction(int Value)
//{
//	if (0 != Value) {
//		char* TempString = (char*)
//			malloc(512 + strlen(WINDOW_TITLE_PREFIX));
//
//		sprintf(
//			TempString,
//			"%s: %d Frames Per Second @ %d x %d",
//			WINDOW_TITLE_PREFIX,
//			FrameCount * 4,
//			CurrentWidth,
//			CurrentHeight
//		);
//
//		glutSetWindowTitle(TempString);
//		free(TempString);
//	}
//
//	FrameCount = 0;
//	glutTimerFunc(250, TimerFunction, 1);
//}
//
//void Cleanup(void)
//{
//	//DestroyShaders();
//	DestroyVBO();
//}
//
//void CreateVBO(void)
//{
//	Vertex Vertices[] =
//	{
//
//		{ { -0.8f, -0.8f, 0.0f, 1.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },
//		{ { 0.0f,  0.8f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } },
//		{ { 0.8f, -0.8f, 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f, 1.0f } }
//	};
//
//	GLenum ErrorCheckValue = glGetError();
//	const size_t BufferSize = sizeof(Vertices);
//	const size_t VertexSize = sizeof(Vertices[0]);
//	const size_t RgbOffset = sizeof(Vertices[0].XYZW);
//
//	glGenVertexArrays(1, &VaoId);
//	glBindVertexArray(VaoId);
//
//	glGenBuffers(1, &VboId);
//	glBindBuffer(GL_ARRAY_BUFFER, VboId);
//	glBufferData(GL_ARRAY_BUFFER, BufferSize, Vertices, GL_STATIC_DRAW);
//
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
//	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);
//
//	glEnableVertexAttribArray(0);
//	glEnableVertexAttribArray(1);
//
//	ErrorCheckValue = glGetError();
//	if (ErrorCheckValue != GL_NO_ERROR)
//	{
//		fprintf(
//			stderr,
//			"ERROR: Could not create a VBO: %s \n",
//			gluErrorString(ErrorCheckValue)
//		);
//
//		exit(-1);
//	}
//}
//
//void DestroyVBO(void)
//{
//	GLenum ErrorCheckValue = glGetError();
//
//	glDisableVertexAttribArray(1);
//	glDisableVertexAttribArray(0);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glDeleteBuffers(1, &VboId);
//
//	glBindVertexArray(0);
//	glDeleteVertexArrays(1, &VaoId);
//
//	ErrorCheckValue = glGetError();
//	if (ErrorCheckValue != GL_NO_ERROR)
//	{
//		fprintf(
//			stderr,
//			"ERROR: Could not destroy the VBO: %s \n",
//			gluErrorString(ErrorCheckValue)
//		);
//
//		exit(-1);
//	}
//}
//
//void ExitOnGLError(const char* error_message)
//{
//	const GLenum ErrorValue = glGetError();
//
//	if (ErrorValue != GL_NO_ERROR)
//	{
//		fprintf(stderr, "%s: %s\n", error_message, gluErrorString(ErrorValue));
//		exit(EXIT_FAILURE);
//	}
//}
//
///*All drawing (that means calls to glVertex or glDraw…) should happen only (and only) 
//in the GLUT display callback (or go to a off-screen renderbuffer/texture through a framebuffer object).
//
//If you're a newbie and you're using GLUT there's a simple rule that will keep you from falling into most pitfalls: 
//There must be no calls to function starting with gl… (that doesn't mean glut… or glew…, just gl and then uppercase)
//in any function other than the display callback function (I know that most, if not all tutorials out there violate
//this rule; don't let them fool you). If you follow that rule you'll find that a surprisingly large fraction of OpenGL
//newbie problems vanish*/