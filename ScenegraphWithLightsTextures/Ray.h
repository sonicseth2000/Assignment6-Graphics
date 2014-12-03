#ifndef _RAY_H_
#define _RAY_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>
#include <string>
using namespace std;

class Ray
{
public:
	void setPosition(glm::vec4 temp)
	{position=temp;}
	glm::vec4 getPosition()
	{return position;}
	void setDirection(glm::vec4 temp)
	{direction=temp;}
	glm::vec4 getDirection()
	{return direction;}

private:
	glm::vec4 position;
	glm::vec4 direction;
};
#endif