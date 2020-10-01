#pragma once
#include <cmath>
#include <vector>
#include <glm/glm.hpp>

using namespace std; 

class Sphere
{
private:
	int numVertices;
	int numIndices;
	vector<int> indices;
	vector<glm::vec3> vertices;
	vector<glm::vec2> texCoords;
	vector<glm::vec3> normals;

	void init(int);
	float toRadians(float degrees);


public:
	Sphere(int precision);
	int GetNumVerticies();
	int GetNumIndices();
	vector<int> GetIndices();
	vector<glm::vec3> GetVertices();
	vector<glm::vec2> GetTexCoords();
	vector<glm::vec3> GetNormals();


};

