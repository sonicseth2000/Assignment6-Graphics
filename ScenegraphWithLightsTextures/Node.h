/*Seth Davis and Mike Poulsen
11/7/14
IT356
Assignment 4
*/

#ifndef NODE_H
#define NODE_H

#include <string>
#include <map>
#include <stack>
#include <iostream>
using namespace std;
#include "Light.h"
#include "Ray.h"
#include "HitRecord.h"
#include <glm/glm.hpp>

class Scenegraph;

class Node
{
protected:
    string name;
    Scenegraph *scenegraph;
	bool bbDraw;
	glm::vec3 minBounds,maxBounds;
	vector<Light> myLight;
public:
    Node(Scenegraph *graph,string name="")
    {
		this->parent = NULL;
        scenegraph = graph;
        setName(name);
		bbDraw = false;
		hasCamera=false;
    }

    virtual ~Node()
    {
	}

	virtual Node *getNode(string name)
	{
		if (this->name == name)
			return this;

		return NULL;
	}

	virtual bool intersect(Ray r, HitRecord &h, stack<glm::mat4>& modelView)=0;
    virtual void draw(stack<glm::mat4>& modelView)=0;
	virtual void drawBB(stack<glm::mat4>& modelView)=0;
	virtual void updateBB()=0;
	virtual Node *clone()=0;
	void setParent(Node *parent)
	{
		this->parent = parent;
	}

    void setName(const string& name)
    {
        this->name = name;
    }

	void setBBDraw(bool d)
	{
		bbDraw = d;
	}

	glm::vec3 getMinBounds()
	{
		return minBounds;
	}

	glm::vec3 getMaxBounds()
	{
		return maxBounds;
	}

	void setCamera(){
		hasCamera=true;
	}

	void addLight(const Light& l)
	{
		cout << "Light added in node " << name << endl;
		myLight.push_back(l);
	}

	//virtual vector<Light> getLight(stack<glm::mat4>& modelview){
	//	return getLight(myLight, modelview);
	//}

	/*vector<Light> getLight(){
		vector<Light> l;
		l = getLight(l);
		return l;
	}*/
	virtual void getLights(vector<Light>& l, stack<glm::mat4>& modelview)=0;

protected:
	Node *parent;
	bool hasCamera;
};

#endif // NODE_H
