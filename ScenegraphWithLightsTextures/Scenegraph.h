/*Seth Davis and Mike Poulsen
11/7/14
IT356
Assignment 4
*/

#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H

#include <map>
#include <stack>
#include <vector>
#include <iostream>
using namespace std;
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/gl.h>
#include "Object.h"
#include "Node.h"
#include "HitRecord.h"
#include "Texture.h"
#include "Ray.h"

	typedef struct
    {
        GLint positionLocation;
        GLint ambientLocation;
        GLint diffuseLocation;
        GLint specularLocation;
    } LightLocation;

class Scenegraph
{    
    friend class LeafNode;
	friend class GroupNode;
	friend class TransformNode;
public:
    Scenegraph();
    ~Scenegraph();
    void makeScenegraph(Node *root);
    void initShaderProgram(GLint shaderProgram);
    void draw(stack<glm::mat4>& modelView);
	void setCamera(Node* camera);
	float* raytrace(int w,int h,stack<glm::mat4>& modelView);
	void addInstance(Object *in)
	{
		instances.push_back(in);
	}

	void animate(float t);

	Object *getInstance(string name)
	{
		int i;

		i = 0;
		while ((i<instances.size()) && (instances[i]->getName()!=name))
			i++;

		if (i<instances.size())
		{
			return instances[i];
		}
		else
		{
			return NULL;
		}
	}

	void addTexture(Texture *tex)
	{
		cout << "Texture " << tex->getName() << " added" << endl;
		texs.push_back(tex);
	}

	Texture *getTexture(string name)
	{
		//cout << "Texture " << name << " looked up" << endl;
		for(int i=0; i<texs.size(); i++)
		{
			if(texs[i]->getName()==name)
				return texs[i];
		}
		return NULL;
	}

private:
    Node *root;
	vector<Object *> instances;
	GLint objectColorLocation;//,modelviewLocation;
	vector<Texture*> texs;
	std::map<string,Texture*> tmap;
	vector<Light> light;
	LightLocation lightLocation[10];
	    GLint projectionLocation,modelviewLocation,normalMatrixLocation;
    GLint numLightsLocation;
    GLint mat_ambientLocation,mat_diffuseLocation,mat_specularLocation,mat_shininessLocation;
	GLint program2;
		GLuint textureID;
    GLint textureLocation;
	void getLights(stack<glm::mat4>& modelView);
	Node* cameraNode;
	bool raycast(Ray r, stack<glm::mat4>& modelView, glm::vec3& color, bool shadow, int maxBounce); 
	glm::vec3 Scenegraph::shade(glm::vec4 pt, vector<Light>& lights, glm::vec4 normal, Material& mat, bool shadow, stack<glm::mat4>& modelView);
	sf::Image image;
};

#endif // SCENEGRAPH_H
