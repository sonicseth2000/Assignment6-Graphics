/*Seth Davis and Mike Poulsen
11/7/14
IT356
Assignment 5
*/
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include "Scenegraph.h"
#include <stack>
#include "TransformNode.h"
#include <fstream>
#include <sstream>
using namespace std;
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include "HitRecord.h"
#include "Material.h"



Scenegraph::Scenegraph()
{
	cameraNode = NULL;
    root = NULL;
}

void Scenegraph::makeScenegraph(Node *r)
{
    if (root!=NULL)
    {
        delete root;
        root = NULL;
    }
    this->root = r;
}



Scenegraph::~Scenegraph()
{
    if (root!=NULL)
    {
        delete root;
        root = NULL;
    }

	/*for (map<string,Texture *>::iterator it=textures.begin();it!=textures.end();it++)
	{
		delete it->second;
	}*/
}

void Scenegraph::initShaderProgram(GLint shaderProgram)
{
	program2 = shaderProgram;

   modelviewLocation = glGetUniformLocation(shaderProgram,"modelview");
   objectColorLocation = glGetUniformLocation(shaderProgram,"vColor");

   	normalMatrixLocation = glGetUniformLocation(shaderProgram,"normalmatrix");
numLightsLocation = glGetUniformLocation(shaderProgram,"numLights");
mat_ambientLocation = glGetUniformLocation(shaderProgram,"material.ambient");
mat_diffuseLocation = glGetUniformLocation(shaderProgram,"material.diffuse");
mat_specularLocation = glGetUniformLocation(shaderProgram,"material.specular");
mat_shininessLocation = glGetUniformLocation(shaderProgram,"material.shininess");
textureLocation = glGetUniformLocation(shaderProgram,"image");

}


float* Scenegraph::raytrace(int w,int h,stack<glm::mat4>& modelView){
	image.create(w,h);
	int i=400;
	int j=300;
	getLights(modelView);
	//for(int i=0; i<w; i++){
	//	for(int j=0; j<h; j++){
	for(int i=0; i<w; i++){
	for(int j=0; j<h; j++){
			Ray r;
			float x= (float)i-w/2.0f;
			float y= (float)j-h/2.0f;
			float z= ((float)-h/2.0f) / tan( (120.0f*3.14159f/180)/2.0f );

			r.setPosition(glm::vec4(0,0,0,1));
			r.setDirection(glm::normalize(glm::vec4(i-(w/2.0f),j-(h/2.0f),z,0)));

			glm::vec3 color;
			
			if(raycast(r,modelView,color,false,5)){
				image.setPixel(i,h-j-1,sf::Color(color.x,color.y,color.z));
			}else{
				image.setPixel(i,h-j-1,sf::Color::Black);
			}
		}
	}
	image.saveToFile("test.png");
	float* f = new float[1];
	cout<<"Image Created"<<endl;
	return f;
}

bool Scenegraph::raycast(Ray r, stack<glm::mat4>& modelView, glm::vec3& color, bool shadow, int maxBounce){
	HitRecord h;
	h.setTime(INT_MAX);

	//color.x=0;
	//return root->raycast(r, modelView, color);
	//if(root!=NULL){
	bool hit = root->intersect(r, h, modelView);
	if(hit){
		//light.clear();
		//root->getLights(light, modelView);
		color = shade(h.getIntersection(),light,h.getNormal(),h.getMaterial(), shadow, modelView);
	}

	glm::vec3 color_t;

	if(h.getMaterial().getReflection()>0 && maxBounce>0){
		Ray normal;
		normal.setDirection(glm::normalize(h.getNormal()));
		normal.setPosition(r.getPosition() + glm::normalize(r.getDirection()));
		Ray reflectRay;
		reflectRay.setDirection(glm::normalize(glm::reflect(glm::normalize(r.getDirection()),normal.getDirection())));
		reflectRay.setPosition(h.getIntersection()+.01f*normal.getDirection());
		raycast(reflectRay,modelView,color_t,false,maxBounce-1);
	}

	color = h.getMaterial().getAbsorption()*color + h.getMaterial().getReflection()*color_t;
	return hit;
}




glm::vec3 Scenegraph::shade(glm::vec4 fPosition, vector<Light>& lights, glm::vec4 fNormal, Material& material, bool shadow, stack<glm::mat4>& modelView) {
	glm::vec3 lightVec,viewVec,reflectVec;
	glm::vec3 normalView;
	glm::vec3 ambient,diffuse,specular;
	float nDotL,rDotV;

	glm::vec4 fColor = glm::vec4(0,0,0,1);


    for (int i=0;i<lights.size();i++)
    {
		bool shadHit=false;
		if(shadow){
			glm::vec4 s = fPosition;
			glm::vec4 d = lights[i].getPosition()-fPosition;
			Ray r;
			r.setPosition(s+.005f*d);
			r.setDirection(d);
			HitRecord h;
			h.setTime(INT_MAX);
			bool hit = root->intersect(r,h,modelView);
			float t = h.getTime();
			ambient = material.getAmbient().xyz() * light[i].getAmbient();
			if(t>=0 && t<=1){
				fColor = fColor + glm::vec4(0,0,0,1.0);
				shadHit = true;
			}
		}

		if(!shadHit){
	//cout<<lights[i].getPosition().x<<" "<<lights[i].getPosition().y<<" "<<lights[i].getPosition().z<<endl;
			if (lights[i].getPosition().w != 0)
				lightVec = glm::normalize(lights[i].getPosition().xyz() - fPosition.xyz());
			else
				lightVec = glm::normalize(-lights[i].getPosition().xyz());

			glm::vec3 tNormal = fNormal.xyz();
			normalView = glm::normalize(tNormal.xyz());
			nDotL = glm::dot(normalView,lightVec);

			viewVec = -fPosition.xyz();
			viewVec = glm::normalize(viewVec);

			reflectVec = glm::reflect(-lightVec,normalView);
			reflectVec = glm::normalize(reflectVec);

			rDotV = glm::max(glm::dot(reflectVec,viewVec),0.0f);

			ambient = material.getAmbient().xyz() * light[i].getAmbient();
			diffuse = material.getDiffuse().xyz() * light[i].getDiffuse() * glm::max(nDotL,0.0f);
			if (nDotL>0)
				specular = material.getSpecular().xyz() * light[i].getSpecular() * pow(rDotV,material.getShininess());
			else
				specular = glm::vec3(0,0,0);
			fColor = fColor + glm::vec4(ambient+diffuse+specular,1.0);
		}

    }
    //fColor = fColor * texture2D(image,fTexCoord.st);
	int r = fColor.x* 255/lights.size();
	int g = fColor.y* 255/lights.size();
	int b = fColor.z* 255/lights.size();
	int a = fColor.a* 255/lights.size();

	if(r > 255)
	r = 255;
	if(g > 255)
	g = 255;
	if(b > 255)
	b = 255;

	return glm::vec3(r,g,b);
}





void Scenegraph::draw(stack<glm::mat4>& modelView)
{
	//vector<Light> light;
	getLights(modelView);
	light;
	//cout<<light[0].getPosition().x<<" "<<light[0].getPosition().y<<" "<<light[0].getPosition().z<<endl;;


    for (int i=0;i<light.size();i++)
    {
		glm::vec4 pos = light[i].getPosition();

		//cout << pos[0] << " " << pos[1] << " " << pos[2] << " " << pos[3] << endl;

        stringstream name;

        name << "light[" << i << "].ambient";

        lightLocation[i].ambientLocation = glGetUniformLocation(program2,name.str().c_str());

        name.clear();//clear any bits set
        name.str(std::string());

        name << "light[" << i << "].diffuse";

        lightLocation[i].diffuseLocation = glGetUniformLocation(program2,name.str().c_str());

        name.clear();//clear any bits set
        name.str(std::string());

        name << "light[" << i << "].specular";

        lightLocation[i].specularLocation = glGetUniformLocation(program2,name.str().c_str());

        name.clear();//clear any bits set
        name.str(std::string());

        name << "light[" << i << "].position";

        lightLocation[i].positionLocation = glGetUniformLocation(program2,name.str().c_str());

        name.clear();//clear any bits set
        name.str(std::string());

    }

	glUniform1i(numLightsLocation,1);

	for (int i=0;i<light.size();i++)
    {
		//cout<<"aaa"<<endl;
        glUniform3fv(lightLocation[i].ambientLocation,1,glm::value_ptr(light[i].getAmbient()));
        glUniform3fv(lightLocation[i].diffuseLocation,1,glm::value_ptr(light[i].getDiffuse()));
        glUniform3fv(lightLocation[i].specularLocation,1,glm::value_ptr(light[i].getSpecular()));
        glUniform4fv(lightLocation[i].positionLocation,1,glm::value_ptr(light[i].getPosition()));
    
		//glm::vec4 pos = light[i].getPosition();

		//cout << pos[0] << " " << pos[1] << " " << pos[2] << " " << pos[3] << endl;
	}



	if (root!=NULL)
    {
        root->draw(modelView);
    }

	if (root!=NULL)
	{
		root->updateBB();
		root->drawBB(modelView);
	}
}

void Scenegraph::getLights(stack<glm::mat4>& modelView){
	light.clear();
	root->getLights(light, modelView);
	//cout<<l.size()<<endl;
}

void Scenegraph::setCamera(Node* camera){
	cameraNode=NULL;
	cameraNode = camera;
}

void Scenegraph::animate(float time)
{
	/*//animate upper merry go round
	     TransformNode* mgr = (TransformNode*)root->getNode("merry_go_round_t");
		 mgr->setAnimationTransform(glm::rotate(glm::mat4(1.0),0.5f*sin(time*10.0f),glm::vec3(0.0f,0.0f,1.0f)) * glm::rotate(glm::mat4(1.0),5.0f*time,glm::vec3(0.0f,1.0f,0.0f)));
		 //animate the 4 sets of chairs
		 TransformNode* c1 = (TransformNode*)root->getNode("chairs1");
		 c1->setAnimationTransform(glm::translate(glm::mat4(1.0),glm::vec3(-3.5f,0.0f,0.0f)) * glm::rotate(glm::mat4(1.0),15.0f*time,glm::vec3(0.0f,1.0f,0.0f))*glm::translate(glm::mat4(1.0),glm::vec3(3.5f,0.0f,0.0f)));
		 TransformNode* c2 = (TransformNode*)root->getNode("chairs2");
		 c2->setAnimationTransform(glm::translate(glm::mat4(1.0),glm::vec3(3.5f,0.0f,0.0f)) * glm::rotate(glm::mat4(1.0),15.0f*time,glm::vec3(0.0f,1.0f,0.0f))*glm::translate(glm::mat4(1.0),glm::vec3(-3.5f,0.0f,0.0f)));
		 TransformNode* c3 = (TransformNode*)root->getNode("chairs3");
		 c3->setAnimationTransform(glm::translate(glm::mat4(1.0),glm::vec3(0.0f,0.0f,3.5f)) * glm::rotate(glm::mat4(1.0),15.0f*time,glm::vec3(0.0f,1.0f,0.0f))*glm::translate(glm::mat4(1.0),glm::vec3(0.0f,0.0f,-3.5f)));
		 TransformNode* c4 = (TransformNode*)root->getNode("chairs4");
		 c4->setAnimationTransform(glm::translate(glm::mat4(1.0),glm::vec3(0.0f,0.0f,-3.5f)) * glm::rotate(glm::mat4(1.0),15.0f*time,glm::vec3(0.0f,1.0f,0.0f))*glm::translate(glm::mat4(1.0),glm::vec3(0.0f,0.0f,3.5f)));
		 

		TransformNode* s1 = (TransformNode*)root->getNode("seat1");
        s1->setAnimationTransform(glm::rotate(glm::mat4(1.0),5.0f*time,glm::vec3(0.0f,1.0f,0.0f))*glm::translate(glm::mat4(1.0),glm::vec3(0.0f,10.0f*sin(time*5.0f),0.0f)));

        TransformNode* s2 = (TransformNode*)root->getNode("seat2");
        s2->setAnimationTransform(glm::rotate(glm::mat4(1.0),5.0f*time,glm::vec3(0.0f,1.0f,0.0f)) *glm::translate(glm::mat4(1.0),glm::vec3(0.0f,5.0f*sin(time*10.0f),0.0f)));

        TransformNode* s3 = (TransformNode*)root->getNode("seat3");
        s3->setAnimationTransform(glm::rotate(glm::mat4(1.0),5.0f*time,glm::vec3(0.0f,1.0f,0.0f)) *glm::translate(glm::mat4(1.0),glm::vec3(0.0f,20.0f*sin(time*1.0f),0.0f)));

        TransformNode* s4 = (TransformNode*)root->getNode("seat4");
        s4->setAnimationTransform(glm::rotate(glm::mat4(1.0),5.0f*time,glm::vec3(0.0f,1.0f,0.0f)) *glm::translate(glm::mat4(1.0),glm::vec3(0.0f,10.0f*sin(time*10.0f),0.0f)));*/
}

