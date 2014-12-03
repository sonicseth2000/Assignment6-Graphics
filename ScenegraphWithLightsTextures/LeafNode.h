/*Seth Davis and Mike Poulsen
11/7/14
IT356
Assignment 5
*/
#ifndef _LEAF_H_
#define _LEAF_H_
#include "node.h"
#include "Object.h"
#include "Ray.h"
#include "HitRecord.h"
#include "Material.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Texture.h"


class LeafNode : public Node
{
public:

	LeafNode(Object *instanceOf,Scenegraph *graph,string name="")
		:Node(graph,name)
	{
		this->instanceOf = instanceOf;
		//default material
		material.setAmbient(1.0f,0.6f,0.6f);
		material.setDiffuse(1.0f,0.6f,0.6f);
		material.setSpecular(0.2f,0.1f,0.1f);
		material.setShininess(1);
		myTexture=NULL;
	}

	~LeafNode(void)
	{
	}

	Node *clone()
	{
		LeafNode *newclone = new LeafNode(instanceOf,scenegraph,name);
		newclone->setMaterial(material);

		return newclone;
	}




	virtual bool intersect(Ray r, HitRecord &h, stack<glm::mat4>& modelView)
        {
				//r.setDirection(glm::inverse(modelView.top())*r.getDirection());
				//r.setPosition(glm::inverse(modelView.top())*r.getPosition());
				//glm::vec4 v= r.getDirection(); //glm::normalize(r.getDirection());
				//glm::vec4 s=r.getPosition();

			    glm::vec4 v= (glm::inverse(modelView.top())*r.getDirection());
				glm::vec4 s= (glm::inverse(modelView.top())*r.getPosition());
				//glm::vec4 v= r.getDirection(); //glm::normalize(r.getDirection());
				//glm::vec4 s=r.getPosition();

				if(instanceOf->getName().compare("sphere") == 0){
					
			        float a=(v.x*v.x)+(v.y*v.y)+(v.z*v.z);
		            float b=(2*s.x*v.x)+(2*s.y*v.y)+(2*s.z*v.z);
	                float c=(s.x*s.x)+(s.y*s.y)+(s.z*s.z)-1.0f;
					
					if(((b*b)-(4.0f*a*c))<0){
						return false;
					}
					bool hit=false;
					float t = FLT_MAX;

			        if((-b+sqrt((b*b)-(4.0f*a*c)))/(2*a)>=0)
		            {
						  t=(-1.0f*b+sqrt((b*b)-(4*a*c)))/(2*a);



	                       /*//cout<<"a"<<endl;
							float t=(-b+sqrt((b*b)-(4*a*c)))/(2*a);
						   //cout<<"t: "<<t<<" curr:"<<h.getTime()<<endl;
					       glm::vec4 p=r.getPosition()+(glm::mat4(t)*r.getDirection());
				           glm::vec4 n=r.getPosition();
						   //cout<<getColor().x<<getColor().y<<getColor().z<<endl;
						   if(t<h.getTime()){
								h.setIntersection((r.getPosition() + t*r.getDirection()));
								h.setTime(t);
								h.setNormal(glm::normalize(h.getIntersection()));
								//h.setNormal(glm::normalize(modelView.top()*(h.getIntersection())));
								h.setMaterial(material);
							

						   }*/
				            hit=true;
			        }
					if((-10.f*b-sqrt((b*b)-(4.0f*a*c)))/(2*a)>=0 && (-b-sqrt((b*b)-(4.0f*a*c)))/(2*a)<t)
	                {
						  t=(-b-sqrt((b*b)-(4*a*c)))/(2*a);

						   /*float t=(-b+sqrt((b*b)-(4*a*c)))/(2*a);
				           glm::vec4 p=r.getPosition()+(glm::mat4(t)*r.getDirection());
			               glm::vec4 n=r.getPosition();
						   if(t<h.getTime()){
								h.setIntersection(r.getPosition() + t*r.getDirection());
								h.setTime(t);
								h.setNormal(glm::normalize(h.getIntersection()));
								//h.setNormal(glm::normalize(modelView.top()*(h.getIntersection())));
								h.setMaterial(material);
						   }*/
				            hit=true;
			        }
					
					if(hit){
						if(t<h.getTime()){
							glm::vec4 normal = glm::vec4((s.x +(t*v.x)), (s.y +(t*v.y)), (s.z +(t*v.z)), 0.0f);
							h.setNormal(glm::transpose(glm::inverse(modelView.top())) * glm::vec4(normal.x,normal.y,normal.z,0.0f));
							h.setIntersection(modelView.top() * glm::vec4(normal.x,normal.y,normal.z,1.0f));
						    h.setMaterial(material);
							h.setTime(t);
						}
					}

					return hit;
					
				}else{

					if(instanceOf->getName().compare("box") == 0){
						float near = FLT_MAX*-1;
					    float far = FLT_MAX;

						if(v.x != 0.0){
							float xn = (-0.5 - s.x)/v.x;
							float xp = (0.5 - s.x)/v.x;
							float minx = min(xn,xp);
							float maxx =max(xn,xp);
							near = max(near, minx);
							far = min(far, maxx);
						}else{
							if(abs(s.x)>.5)
								return false;
						}

						if(v.y != 0.0){
							float yn = (-0.5 - s.y)/v.y;
							float yp = (0.5 - s.y)/v.y;
							float miny = min(yn,yp);
							float maxy =max(yn,yp);
							near = max(near, miny);
							far = min(far, maxy);
						}else{
							if(abs(s.y)>.5)
								return false;
						}

						if(v.z != 0.0){
							float zn = (-0.5 - s.z)/v.z;
							float zp = (0.5 - s.z)/v.z;
							float minz = min(zn,zp);
							float maxz =max(zn,zp);
							near = max(near, minz);
							far = min(far, maxz);
						}else{
							if(abs(s.z)>.5)
								return false;
						}


						//fixes precision errors
						float error = .005;
						glm::vec4 p = s+near*v;
						if(near<=far && far>=0){
							glm::vec4 p= s+near*v;
							if(near<h.getTime()||h.getTime()<0){
								if(glm::abs(p.x-.5)<error){
									h.setNormal(modelView.top()*glm::vec4(1,0,0,0));
								}else{ 
									if(glm::abs(p.x+.5)<error){
										h.setNormal(modelView.top()*glm::vec4(-1,0,0,0));
									}else{
										if(glm::abs(p.y-.5)<error){
											h.setNormal(modelView.top()*glm::vec4(0,1,0,0));
										}else{
											if(glm::abs(p.y+.5)<error){
												h.setNormal(modelView.top()*glm::vec4(0,-1,0,0));
											}else{
												if(glm::abs(p.z-.5)<error){
													h.setNormal(modelView.top()*glm::vec4(0,0,1,0));
												}else{
													if(glm::abs(p.z+.5)<error){
														h.setNormal(modelView.top()*glm::vec4(0,0,-1,0));
													}
												}
											}
										}
									}
								}
							}
				
						

							h.setMaterial(material);
							h.setIntersection(modelView.top() * glm::vec4(s.x + near*v.x,s.y + near*v.y,s.z + near*v.z,1.0f));
							h.setTime(near);
							
							return true;
						}

						return false;



					}
				}
        }

	virtual void draw(stack<glm::mat4> &modelView)
    {
		GLuint a;
        if (instanceOf!=NULL)
		{
			 //get the color
            glm::vec4 color = material.getAmbient();
			glUniform3fv(scenegraph->objectColorLocation,1,glm::value_ptr(color));

			glUniformMatrix4fv(scenegraph->modelviewLocation,1,GL_FALSE,glm::value_ptr(modelView.top()));
			glUniformMatrix4fv(scenegraph->normalMatrixLocation,1,GL_FALSE,glm::value_ptr(glm::transpose(glm::inverse(modelView.top()))));

	glUniform3fv(scenegraph->mat_ambientLocation,1,glm::value_ptr(material.getAmbient()));
	glUniform3fv(scenegraph->mat_diffuseLocation,1,glm::value_ptr(material.getDiffuse()));
	glUniform3fv(scenegraph->mat_specularLocation,1,glm::value_ptr(material.getSpecular()));
	glUniform1f(scenegraph->mat_shininessLocation,material.getShininess());

            //set the color for all vertices to be drawn for this object
           // glUniform3fv(scenegraph->objectColorLocation,1,glm::value_ptr(color));
			a = glGetError();
			glUniformMatrix4fv(scenegraph->modelviewLocation,1,GL_FALSE,glm::value_ptr(modelView.top()));
	
			if(myTexture!=NULL)
			{
				glEnable(GL_TEXTURE_2D);
				glActiveTexture(GL_TEXTURE0);
				textureID=myTexture->getTextureID();
				glBindTexture(GL_TEXTURE_2D, textureID);
				glUniform1i(scenegraph->textureLocation,0);
				glDisable(GL_TEXTURE_2D);
			}else{
				myTexture=scenegraph->getTexture("empty");
				glEnable(GL_TEXTURE_2D);
				glActiveTexture(GL_TEXTURE0);
				textureID=myTexture->getTextureID();
				glBindTexture(GL_TEXTURE_2D, textureID);
				glUniform1i(scenegraph->textureLocation,0);
				glDisable(GL_TEXTURE_2D);
			}
					a = glGetError();
			instanceOf->draw();        
			a = glGetError();


		}

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
	}
	
	virtual void updateBB()
	{
		minBounds = instanceOf->getMinimumWorldBounds().xyz();
		maxBounds = instanceOf->getMaximumWorldBounds().xyz();
	}

	glm::vec4 getColor()
	{
		return material.getAmbient();
	}

	/*
		*Set the material of each vertex in this object
		*/
	virtual void setMaterial(Material& mat)
	{
		material = mat;
	}

	/*
		* gets the material
		*/
	Material getMaterial()
	{
		return material;
	}

	void setTexture(Texture *tex)
	{
		cout << "Texture set to " << tex->getName() << endl;
		myTexture=tex;
	}

	virtual void getLights(vector<Light>& l, stack<glm::mat4>& modelview){
		//modelview.push(modelview.top());
        //modelview.top() = modelview.top() * animation_transform * transform;
		for(int i=0;i<myLight.size();i++){
			Light light=myLight[i];
			light.setPosition(glm::vec3(modelview.top()*myLight[i].getPosition()));
			l.push_back(light);
		}
		//modelview.pop();
	}

protected:
	Object *instanceOf;
	Material material;
private:
	Texture *myTexture;
	GLuint textureID;
};
#endif
