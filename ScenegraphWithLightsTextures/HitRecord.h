#ifndef _HITRECORD_H_
#define _HITRECORD_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>
#include <string>
#include "Material.h"
#include "Texture.h"
using namespace std;
class HitRecord
{
public:
	void setTime(int t)
	{timeHit=t;}
	int getTime()
	{return timeHit;}
	void setIntersection(glm::vec4 temp)
	{intersection=temp;}
	glm::vec4 getIntersection()
	{return intersection;}
	void setNormal(glm::vec4 temp)
	{normal=temp;}
	glm::vec4 getNormal()
	{return normal;}
	void setMaterial(Material mat)
	{myMat=mat;}
	Material getMaterial()
	{return myMat;}
	void setTexture(Texture tex)
	{myText=tex;}
	Texture getTexture()
	{return myText;}
	void setTexCoord(glm::vec4 spot)
	{texCoord=spot;}
	glm::vec4 getTexCoord()
	{return texCoord;}
private:
	int timeHit;
	glm::vec4 intersection;
	glm::vec4 normal;
	Material myMat;
	Texture myText;
	glm::vec4 texCoord;
};
#endif