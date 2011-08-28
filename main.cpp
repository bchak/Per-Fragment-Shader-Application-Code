/*	This is a small OpenGL 4.1 program that I wrote for learning how to render objects and write shaders.

    Copyright (C) 2011  Jesse 
    http://www.youtube.com/user/GraphicsProgrammer

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "filereader.h"
#include "pawn.h"

#define BUFFER_OFFSET(bytes) ((GLvoid*)(bytes))

int CurrentWidth = 1080,
	CurrentHeight = 720,
	WindowHandle = 0;

GLfloat Theta[3] = {0.0,0.0,0.0};
GLuint mvpMatrix, mvMatrix, normalMatrix, diffuseColor, vLightPosition, ambientColor, specularColor;
GLfloat FOV = 45.0f;

bool WKeyDown = false;
bool XKeyDown = false;

GLuint
	VertexShaderId,
	FragmentShaderId,
	ProgramId,
	VaoId,
	VboId,
	NormalBufferId;

const GLchar* VertexShader = filetobuf("shader.vert");
const GLchar* FragmentShader = filetobuf("shader.frag");

void Initialize(int, char*[]);
void InitWindow(int, char*[]);
void ResizeFunction(int, int);
void RenderFunction(void);
void IdleFunction(void);
void Cleanup(void);
void CreateVBO(void);
void DestroyVBO(void);
void CreateShaders(void);
void DestroyShaders(void);
void SpecialKeys(int,int,int);
void Keyboard(unsigned char, int, int);
void AverageNormals(void);

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
		"OpenGL Version: %s\n",
		glGetString(GL_VERSION)
	);

	CreateShaders();
	CreateVBO();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.12f, 0.12f, 0.12f, 1.0f );
}

void InitWindow(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 1);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
	);

	glutInitWindowSize(CurrentWidth, CurrentHeight);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	WindowHandle = glutCreateWindow("Rendering in OpenGL 4.1 Core");

	if(WindowHandle < 1) {
		fprintf(
			stderr,
			"ERROR: Could not create a new rendering window.\n"
		);
		exit(EXIT_FAILURE);
	}

	glutReshapeFunc(ResizeFunction);
	glutDisplayFunc(RenderFunction);
	glutSpecialFunc(SpecialKeys);
	glutKeyboardFunc(Keyboard);
	glutIdleFunc(IdleFunction);
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// light colors
	GLfloat vEyeLight[] = { 0.0f, 0.0f, 10.0f };
	GLfloat vDiffuseColor[] = { 0.34615f, 0.3143f, 0.0903f, 1.0f };
	GLfloat vAmbientColor[] = { 0.24725f, 0.2245f, 0.0645f, 1.0f };
	GLfloat vSpecularColor[] = { 0.797357f, 0.723991f, 0.208006f, 1.0f };

	// transformations
	glm::mat4 P = glm::perspective(FOV, (GLfloat)CurrentWidth/(GLfloat)CurrentHeight, 0.1f, 100.f);
	glm::mat4 T = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f, 0.0f, -10.0f));
	glm::mat4 Rx = glm::rotate(T,  Theta[0], glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 Ry = glm::rotate(Rx, Theta[1], glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 MV = glm::rotate(Ry, Theta[2], glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat3 N = glm::inverse(glm::transpose(glm::mat3(MV)));
	glm::mat4 MVP = P * MV;

	// uniforms
	glUniform3fv(vLightPosition, 1, vEyeLight);
	glUniform4fv(diffuseColor, 1, vDiffuseColor);
	glUniform4fv(ambientColor, 1, vAmbientColor);
	glUniform4fv(specularColor, 1, vSpecularColor);
	glUniformMatrix4fv(mvpMatrix, 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix4fv(mvMatrix, 1, GL_FALSE, glm::value_ptr(MV));
	glUniformMatrix3fv(normalMatrix, 1, GL_FALSE, glm::value_ptr(N));


	glDrawArrays(GL_TRIANGLES, 0, (3 * numOfFaces));


	glutSwapBuffers();
	glutPostRedisplay();
}

void SpecialKeys(int key, int x, int y)
{

	if(key == GLUT_KEY_UP)
		Theta[0] -= 2.3;

	if(key == GLUT_KEY_DOWN)
		Theta[0] += 2.3;

	if(key == GLUT_KEY_LEFT)
		Theta [2] -= 2.4;

	if(key == GLUT_KEY_RIGHT)
		Theta[2] += 2.5;
}

void Keyboard(unsigned char c, int x, int y)
{
	static bool wire = false;

	switch(c)
	{
		case ' ':
			wire = !wire;
			if(wire)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;

		case 'w':
			WKeyDown = !WKeyDown;
			break;

		case 'x':
			XKeyDown = !XKeyDown;
			break;
	}
}

void IdleFunction(void)
{
	static float variable = 0.5;
	Theta[1] += variable;
	if(WKeyDown)
		FOV += 0.5f;
	if(XKeyDown)
		FOV -= 0.5f;
	glutPostRedisplay();
}

void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}

void Triangle(int a, int b, int c)
{
	static int index = 0;
	static int face = 0;

	//compute the normal
	const glm::vec3 & u = glm::vec3(Vertices[a-1]);
	const glm::vec3 & v = glm::vec3(Vertices[b-1]);
	const glm::vec3 & w = glm::vec3(Vertices[c-1]);
	glm::vec3 norml = glm::normalize(glm::cross(v-w,w-u));

	faceNormals[face] = norml;face++;
	//normals[face * 3] = normals[face * 3 + 1] = normals[face * 3 +2] = norml;
	points[index] = Vertices[a-1]; index++;
	points[index] = Vertices[b-1]; index++;
	points[index] = Vertices[c-1]; index++;
}

void CreateModel(void)
{

	for(int i=0;i<(numOfFaces*3);i++)
	{
		Triangle(indexes[i], indexes[i+1],indexes[i+2]);
		i += 2;
	}
}
// smooth out the normals
void AverageNormals(void)
{

	// for each vertex
	for(int i = 0; i < numOfVertices; i++)
	{
		glm::vec3 avgNormal = glm::vec3(0.0,0.0,0.0);
		// search the triangle list
		for(int j = 0; j < numOfFaces; j++)
		{

			// check all 3 of the vertices
			for(int k = 0; k < 3; k++)
				// if the vertex contributes to any one of the faces
				if(i == (indexes[j*3 + k]-1))
				{
					avgNormal += faceNormals[j];

				}
		}

		avgNormals[i] = glm::normalize(avgNormal);
	}
	// create the final normal array
	for(int n = 0; n < (numOfFaces * 3); n++)
	{
		normals[n] = avgNormals[indexes[n]-1];
	}

	// non averaged normals
	/*for(int i = 0; i < numOfFaces; i++)
	{
	    normals[i*3] = normals[i*3+1] = normals[i*3+2] = faceNormals[i];
	}*/

}

void CreateVBO(void)
{
	CreateModel();
	AverageNormals();
    glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);

	glGenBuffers(1, &VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(normals),normals);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//glGenBuffers(1, &NormalBufferId);
	//glBindBuffer(GL_ARRAY_BUFFER, NormalBufferId);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
}

void DestroyVBO(void)
{

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glDeleteBuffers(1, &NormalBufferId);
	glDeleteBuffers(1, &VboId);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}

void CreateShaders(void)
{
	VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShaderId, 1, &VertexShader, NULL);
	glCompileShader(VertexShaderId);

	FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShaderId, 1, &FragmentShader, NULL);
	glCompileShader(FragmentShaderId);

	ProgramId = glCreateProgram();

	glAttachShader(ProgramId, VertexShaderId);
	glAttachShader(ProgramId, FragmentShaderId);

	glLinkProgram(ProgramId);
	glUseProgram(ProgramId);

	// create uniforms
	diffuseColor = glGetUniformLocation(ProgramId, "diffuseColor");
	vLightPosition = glGetUniformLocation(ProgramId, "vLightPosition");
	ambientColor = glGetUniformLocation(ProgramId, "ambientColor");
	specularColor = glGetUniformLocation(ProgramId, "specularColor");
	mvpMatrix = glGetUniformLocation(ProgramId, "mvpMatrix");
	mvMatrix = glGetUniformLocation(ProgramId, "mvMatrix");
	normalMatrix = glGetUniformLocation(ProgramId, "normalMatrix");

	// create attributes
	GLuint vPosition = glGetAttribLocation(ProgramId, "vVertex");
	glEnableVertexAttribArray(vPosition);
	GLuint vNormal = glGetAttribLocation(ProgramId, "vNormal");
	glEnableVertexAttribArray(vNormal);
}

void DestroyShaders(void)
{

	glUseProgram(0);

	glDetachShader(ProgramId, VertexShaderId);
	glDetachShader(ProgramId, FragmentShaderId);

	glDeleteShader(FragmentShaderId);
	glDeleteShader(VertexShaderId);

	glDeleteProgram(ProgramId);

}
