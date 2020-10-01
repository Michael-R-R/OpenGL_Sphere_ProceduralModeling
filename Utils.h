#ifndef Utils
#define Utils

// Used to read in .glsl txt files for vertex and fragment shaders
std::string readShaderSource(const char *filePath);

// Check for errors
void printShaderLog(GLuint shader);
void printProgramLog(int prog);
bool checkOpenGLError();

// Create the vertex and fragment shaders
GLuint createShaderProgram(const char *vp, const char *fp);

// Function to load textures
GLuint LoadTexture(const char *texImagePath);

// Transforms
glm::mat4 buildTranslate(float x, float y, float z);
glm::mat4 buildRotateX(float rad);
glm::mat4 buildRotateY(float rad);
glm::mat4 buildRotateZ(float rad);
glm::mat4 buildScale(float x, float y, float z);

#endif // !Utils


