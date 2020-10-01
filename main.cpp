#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <fstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SOIL2/SOIL2.h>
#include "Sphere.h"
#include "Utils.h"

using namespace std;

#define numVAOs 1
#define numVBOs 2

// Variables for the camera location
float cameraX, cameraY, cameraZ;

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// Class objects
Sphere mySphere(48);

// Textures
GLuint mapTexture;

// Variables for the perspective
GLuint mvLoc, projLoc, vLoc, mLoc, tfLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, tMat, mvMat;
// Stacks
stack<glm::mat4> mvStack;

// Forward declarations
void init(GLFWwindow* window);
void setupVertices();
void display(GLFWwindow* window, double currentTime);
void window_reshape_callback(GLFWwindow *window, int newWidth, int newHeight);



int main(void)
{
	// Check to make sure glfw initilized
	if (!glfwInit()) { exit(EXIT_FAILURE); }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Create the window
	GLFWwindow* window = glfwCreateWindow(1200, 800, "CGP_03", NULL, NULL);
	glfwMakeContextCurrent(window);
	// Check to make sure glew initilized
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1); // Enables v-sync

	// Call back when window resized
	glfwSetWindowSizeCallback(window, window_reshape_callback);

	// Initilize the window
	init(window);

	// While loop to keep the window open
	while (!glfwWindowShouldClose(window))
	{
		display(window, glfwGetTime());
		glfwSwapBuffers(window); // Paints the screen
		glfwPollEvents(); // Handles key events like key presses
	}

	// Close the window
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

// Performs single used tasks: reading in shader code
// and building the rendering program, and loading cube vertices
// into the VBO, as well as positions the cube and camera
void init(GLFWwindow* window)
{
	// Program that reads in the vertex and fragment shade .glsl files
	// And creates the vertex and fragment shaders
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");

	mapTexture = Utils::LoadTexture("map.jpg");

	// Initalize the camera and cubes position values
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 5.0f;

	// Pre-Compute the perspective matrix (performance saving)
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472 radians = 60 degrees

	// Initalizes the cube
	setupVertices();
}

// 36 vertices, 12 triangles, makes 2x2x2 cube places at origin
void setupVertices()
{
	vector<int> ind = mySphere.GetIndices();
	vector<glm::vec3> vert = mySphere.GetVertices();
	vector<glm::vec2> tex = mySphere.GetTexCoords();
	vector<glm::vec3> norm = mySphere.GetNormals();

	vector<float> pValues; // vertex positions
	vector<float> tValues; // texture coords
	vector<float> nValues; // normal vectors

	int numIndicies = mySphere.GetNumIndices();
	for (int i = 0; i < numIndicies; i++)
	{
		pValues.push_back((vert[ind[i]]).x);
		pValues.push_back((vert[ind[i]]).y);
		pValues.push_back((vert[ind[i]]).z);

		tValues.push_back((tex[ind[i]]).s);
		tValues.push_back((tex[ind[i]]).t);

		nValues.push_back((norm[ind[i]]).x);
		nValues.push_back((norm[ind[i]]).y);
		nValues.push_back((norm[ind[i]]).z);
	}

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(3, vbo);

	// Put the vertices into buffer #0
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pValues.size() * 4, &pValues[0], GL_STATIC_DRAW);

	// Put the textures coordinates into buffer #1
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, tValues.size() * 4, &tValues[0], GL_STATIC_DRAW);

	// Put the normals into buffer #2
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, nValues.size() * 4, &nValues[0], GL_STATIC_DRAW);

}

// Displays the window
void display(GLFWwindow* window, double currentTime)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
	// Enables backface culling
	glEnable(GL_CULL_FACE);
	// Enables the shaders
	glUseProgram(renderingProgram);

	// Get the uniform variables location for the projection,
	// camera, and cube from the vertex shader
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");

	// Connect to the proj_matrix in the vertex shader
	// pMat is initalized in init() to save on performance
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	// Push view matrix onto the stack
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	mvStack.push(vMat);

	// -----Pyramid == sun-------
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); // Sun position
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0f, -1.0f, 0.0f)); // Sun rotation

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

	// Enables the VBO containing the cube
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// Bind texture coordinates and assign to layout = 1
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// Activate the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mapTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Enabling Anisotropic filtering
	// if statement to check if the gpu supports AF
	if (glewIsSupported("GL_EXT_texture_filter_anisotropic"))
	{
		GLfloat anisoSetting = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoSetting);
		glTexParameterf(GL_TEXTURE_2D, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, anisoSetting);
	}

	// Adjust OpenGL settings and draw model
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Back face culling
	glFrontFace(GL_CCW); 

	// Drawing
	glDrawArrays(GL_TRIANGLES, 0, mySphere.GetNumIndices());

	mvStack.pop(); // Removes the sun axial rotation from the stack

	// Remove moon scale/rot/tran, planet position, sun position, and view matrices from the stack
	mvStack.pop(); mvStack.pop();
}

void window_reshape_callback(GLFWwindow *window, int newWidth, int newHeight)
{
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}