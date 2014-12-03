/*Seth Davis and Mike Poulsen
11/7/14
IT356
Assignment 4
*/

#ifndef GROUPNODE_H
#define GROUPNODE_H

#include <GL/gl.h>
#include <vector>
using namespace std;
#include "Node.h"
#include "Ray.h"
#include "HitRecord.h"
#include <glm/gtc/matrix_transform.hpp>


class GroupNode: public Node
{
protected:
    vector<Node *> children;

public:
    GroupNode(Scenegraph *graph,string name="")
        :Node(graph,name)
    {

    }

    ~GroupNode()
    {
        for (unsigned int i=0;i<children.size();i++)
            delete children[i];
    }

	virtual Node *clone()
	{
		vector<Node *> newc;

		for (int i=0;i<children.size();i++)
		{
			newc.push_back(children[i]->clone());
		}

		GroupNode * newgroup = new GroupNode(scenegraph,name);

		for (int i=0;i<children.size();i++)
		{
			newgroup->addChild(newc[i]);
		}		
		return newgroup;

	}

	virtual Node* getNode(string name)
	{
		if (Node::getNode(name)!=NULL)
		{
			return Node::getNode(name);
		}

		int i=0;
		Node *answer = NULL;

		while ((i<children.size()) && (answer == NULL))
		{
			answer = children[i]->getNode(name);
			i++;
		}
		return answer;
	}

    virtual void draw(stack<glm::mat4> &modelView)
    {
        for (int i=0;i<children.size();i++)
        {
            children[i]->draw(modelView);
        }
    }

	        virtual bool intersect(Ray r, HitRecord &h, stack<glm::mat4>& modelView)
        {
                bool child_bool=false;
                for(int i=0; i<children.size(); i++)
                {
                        if(children[i]->intersect(r,h,modelView))
                        {child_bool=true;}
                }
                return child_bool;
        }

	virtual void drawBB(stack<glm::mat4>& modelView)
	{
		if (bbDraw)
		{
			glm::mat4 bbTransform;

			bbTransform = glm::translate(glm::mat4(1.0),0.5f*(minBounds+maxBounds)) * glm::scale(glm::mat4(1.0),maxBounds-minBounds);
			glm::vec4 color = glm::vec4(1,1,1,1);
			//set the color for all vertices to be drawn for this object
			glUniform3fv(scenegraph->objectColorLocation,1,glm::value_ptr(color));
			glUniformMatrix4fv(scenegraph->modelviewLocation,1,GL_FALSE,glm::value_ptr(modelView.top() * bbTransform));
			scenegraph->getInstance("box")->draw();        		
		}
		for (int i=0;i<children.size();i++)
        {
            children[i]->drawBB(modelView);
        }
	}

	virtual void updateBB()
	{
		int i,j;

		if (children.size()==0)
			return;

		for (i=0;i<children.size();i++)
		{
			children[i]->updateBB();
		}

		minBounds = children[0]->getMinBounds();
		maxBounds = children[0]->getMaxBounds();

		for (i=1;i<children.size();i++)
		{
			for (j=0;j<3;j++)
			{
				if (children[i]->getMinBounds()[j]<minBounds[j])
					minBounds[j] = children[i]->getMinBounds()[j];

				if (children[i]->getMaxBounds()[j]>maxBounds[j])
					maxBounds[j] = children[i]->getMaxBounds()[j];
			}
		}
	}

    void addChild(Node *child)
    {
        children.push_back(child);
		child->setParent(this);
    }

    const vector<Node *> getChildren() const
    {
        return children;
    }

	virtual void getLights(vector<Light>& l, stack<glm::mat4>& modelview){
		for(int i=0;i<myLight.size();i++){
			Light light=myLight[i];
			light.setPosition(glm::vec3(modelview.top()*myLight[i].getPosition()));
			l.push_back(light);
		}
		for(int i=0;i<children.size();i++){
			children[i]->getLights(l, modelview);
		}
		//cout<<l.size()<<endl;
	}

	/*vector<Light> getLight(vector<Light> l){
		for(int i=0;i<myLight.size();i++){
			l.push_back(myLight[i]);
		}
		for(int i=0;i<children.size();i++){
			children[i]->getLight(l);
		}

		return l;
	}*/
};

#endif // GROUP_H
