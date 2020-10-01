#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SOIL2/SOIL2.h>
#include "Utils.h"

// Reads in .glsl text files for the vertex and fragment shaders
std::string readShaderSource(const char *filePath)
{
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);
	std::string line = "";

	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

// Create vertex and fragment shader
GLuint createShaderProgram(const char *vp, const char *fp)
{
	// Used for checking errors
	GLint vertCompiled;
	GLint fragCompiled;
	GLint linked;

	// Create the vertex and fragment shaders
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vertShaderStr = readShaderSource(vp);
	std::string fragShaderStr = readShaderSource(fp);

	const char *vertShaderSrc = vertShaderStr.c_str();
	const char *fragShaderSrc = fragShaderStr.c_str();

	// Load, check for error, and Complie the vertex and fragment shader
	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);
	glCompileShader(vShader);

	// Check vertex shader for errors
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1)
	{
		std::cout << "Vertex Compilation Failed" << std::endl;
		printShaderLog(vShader);
	}
	
	glCompileShader(fShader);

	// Check fragment shader for errors
	checkOpenGLError();
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1)
	{
		std::cout << "Fragment Compilation Failed" << std::endl;
		printShaderLog(fShader);
	}

	// Create a program and then attach and link the vertex and fragment shader
	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);
	glLinkProgram(vfProgram);

	// Check for linking errors
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1)
	{
		std::cout << "Linking Failed" << std::endl;
		printProgramLog(vfProgram);
	}

	return vfProgram;
}

// Function to load in texture
GLuint LoadTexture(const char *texImagePath)
{
	GLuint textureID;

	textureID = SOIL_load_OGL_texture(texImagePath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (textureID == 0) { std::cout << "Could not find texture file" << texImagePath << std::endl; }

	return textureID;
}

// Check for errors and output the result
void printShaderLog(GLuint shader)
{
	int len = 0;
	int chWrittn = 0;
	char *log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0)
	{
		log = (char *)malloc(len);
		glGetShaderInfoLog(shader,len,&chWrittn,log);
		std::cout << "Shader Info Log: " << log << std::endl;
		free(log);
	}
}
void printProgramLog(int prog)
{
	int len = 0;
	int chWrittn = 0;
	char *log;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0)
	{
		log = (char *)malloc(len);
		glGetProgramInfoLog(prog, len, &chWrittn, log);
		std::cout << "Program Info Log: " << log << std::endl;
		free(log);
	}
}
bool checkOpenGLError()
{
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		std::cout << "glError: " << glErr << std::endl;
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
}

// mat4 formulas for translate, rotate, and scale
// Builds and returns translation matrix
glm::mat4 buildTranslate(float x, float y, float z)
{
	glm::mat4 trans = glm::mat4(1.0, 0.0, 0.0, 0.0,
		                        0.0, 1.0, 0.0, 0.0,
		                        0.0, 0.0, 1.0, 0.0,
		                        x, y, z, 1.0);
	return trans;
}
// Builds and returns a matrix that performs rotation around the x-axis
glm::mat4 buildRotateX(float rad)
{
	glm::mat4 xRot = glm::mat4(1.0, 0.0, 0.0, 0.0,
		                       0.0, cos(rad), -sin(rad), 0.0,
		                       0.0, sin(rad), cos(rad), 0.0,
		                       0.0, 0.0, 0.0, 1.0);
	return xRot;
}
// Builds and returns a matrix that performs rotation around the y-axis
glm::mat4 buildRotateY(float rad)
{
	glm::mat4 yRot = glm::mat4(cos(rad), 0.0, sin(rad), 0.0,
		                       0.0, 1.0, 0.0, 0.0,
		                       -sin(rad), 0.0, cos(rad), 0.0,
		                       0.0, 0.0, 0.0, 1.0);
	return yRot;
}
// Builds and returns a matrix that performs a rotation around the z-axis
glm::mat4 buildRotateZ(float rad)
{
	glm::mat4 zRot = glm::mat4(cos(rad), -sin(rad), 0.0, 0.0,
		                       sin(rad), cos(rad), 0.0, 0.0,
		                       0.0, 0.0, 1.0, 0.0,
		                       0.0, 0.0, 0.0, 1.0);
	return zRot;
}
// Builds and returns a scale matrix
glm::mat4 buildScale(float x, float y, float z)
{
	glm::mat4 scale = glm::mat4(x, 0.0, 0.0, 0.0,
		                        0.0, y, 0.0, 0.0,
		                        0.0, 0.0, z, 0.0,
		                        0.0, 0.0, 0.0, 1.0);
	return scale;
}


