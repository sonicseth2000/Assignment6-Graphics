/*Seth Davis and Mike Poulsen
11/7/14
IT356
Assignment 4
*/

#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include "View.h"
#include <GL/glew.h>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <stdio.h>
using namespace std;


//glm headers to access various matrix producing functions, like ortho below in resize
#include <glm/gtc/matrix_transform.hpp>
//the glm header required to convert glm objects into normal float/int pointers expected by OpenGL
//see value_ptr function below for an example
#include <glm/gtc/type_ptr.hpp>

#include "SceneXMLReader.h"

#include <glm\gtx\rotate_vector.hpp>

View::View()
{
    trackballTransform = glm::mat4(1.0);
	time = 0.0;
	camera=1;
	position1=glm::vec3(0,30,0);
	direction1=glm::vec3(0,-1,0);
	up1=glm::vec3(1,0,0);
	camera1=glm::lookAt(position1,glm::vec3(0,0,0),up1);
	position2=glm::vec3(0,0,30);
	direction2=glm::vec3(0,0,1);
	up2=glm::vec3(0,1,0);
	camera2=glm::lookAt(position2,glm::vec3(0,0,0),up2);
	//switchCamera1();
}


void View::goForward()
{
	if(camera==1)
	{
		trackballTransform=glm::scale(glm::mat4(1.0),glm::vec3(1.1)) * trackballTransform;
	}
	else
	{
		trackballTransform=glm::scale(glm::mat4(1.0),glm::vec3(1.1)) * trackballTransform;
	}
}

void View::goBackward()
{
	if(camera==1)
	{
		trackballTransform=glm::scale(glm::mat4(1.0),glm::vec3(0.9)) * trackballTransform;
	}
	else
	{
		trackballTransform=glm::scale(glm::mat4(1.0),glm::vec3(0.9)) * trackballTransform;
	}
}

void View::turnLeft()
{
	if(camera==1)
	{
		direction1=glm::rotate(direction1,-0.01f,glm::vec3(0,1,0));
		trackballTransform=glm::translate(glm::mat4(1.0),glm::vec3(-position1.x,-position1.y,-position1.z)) * trackballTransform;
		trackballTransform=glm::rotate(glm::mat4(1.0),-0.01f,glm::vec3(0,1,0)) * trackballTransform;
		trackballTransform=glm::translate(glm::mat4(1.0),position1) * trackballTransform;
	}
	else
	{
		direction2=glm::rotate(direction1,-0.01f,glm::vec3(0,1,0));
		trackballTransform=glm::translate(glm::mat4(1.0),-position2) * trackballTransform;
		trackballTransform=glm::rotate(glm::mat4(1.0),-0.01f,glm::vec3(0,1,0)) * trackballTransform;
		trackballTransform=glm::translate(glm::mat4(1.0),position2) * trackballTransform;
	}
}

void View::turnRight()
{
	if(camera==1)
	{
		direction1=glm::rotate(direction1,0.01f,glm::vec3(0,1,0));
		trackballTransform=glm::translate(glm::mat4(1.0),-position1) * trackballTransform;
		trackballTransform=glm::rotate(glm::mat4(1.0),0.01f,glm::vec3(0,1,0)) * trackballTransform;
		trackballTransform=glm::translate(glm::mat4(1.0),position1) * trackballTransform;
	}
	else
	{
		direction2=glm::rotate(direction1,0.01f,glm::vec3(0,1,0));
		trackballTransform=glm::translate(glm::mat4(1.0),-position2) * trackballTransform;
		trackballTransform=glm::rotate(glm::mat4(1.0),0.01f,glm::vec3(0,1,0)) * trackballTransform;
		trackballTransform=glm::translate(glm::mat4(1.0),position2) * trackballTransform;
	}
}

/*void View::switchCamera1()
{
	camera=1;
	//camera2=trackballTransform;
	trackballTransform=camera1;
}

void View::switchCamera2()
{
	camera=2;
	//camera1=trackballTransform;
	trackballTransform=camera2;
}*/

View::~View()
{
    
}

void View::resize(int w, int h)
{
    //record the new dimensions
    WINDOW_WIDTH = w;
    WINDOW_HEIGHT = h;

    /*
     * This program uses orthographic projection. The corresponding matrix for this projection is provided by the glm function below.
     *The last two parameters are for the near and far planes.
     *
     *Very important: the last two parameters specify the position of the near and far planes with respect
     *to the eye, in the direction of gaze. Thus positive values are in front of the camera, and negative
     *values are in the back!
     **/
   
    while (!proj.empty())
        proj.pop();

	//proj.push(glm::ortho(-200.0f,200.0f,-200.0f*WINDOW_HEIGHT/WINDOW_WIDTH,200.0f*WINDOW_HEIGHT/WINDOW_WIDTH,0.1f,10000.0f));
    proj.push(glm::perspective(120.0f*3.14159f/180,(float)WINDOW_WIDTH/WINDOW_HEIGHT,0.1f,10000.0f));
}

void View::openFile(string filename)
{
	SceneXMLReader reader;
	cout << "Loading...";
	reader.importScenegraph(filename,sgraph);
	cout << "Done" << endl;
}

void View::initialize()
{
    //populate our shader information. The two files below are present in this project.
    ShaderInfo shaders[] =
    {
        {GL_VERTEX_SHADER,"phong-multiple.vert"},
        {GL_FRAGMENT_SHADER,"phong-multiple.frag"},
        {GL_NONE,""} //used to detect the end of this array
    };

    //call helper function, get the program shader ID if everything went ok.
    program = createShaders(shaders);

    //use the above program. After this statement, any rendering will use this above program
    //passing 0 to the function below disables using any shaders
    glUseProgram(program);

	//Light l;


    //l.setAmbient(0.8f,0.8f,0.8f);
    //l.setDiffuse(0.8f,0.8f,0.8f);
   // l.setSpecular(0.8f,0.8f,0.8f);
	//l.setPosition(glm::vec3(20,50,100));
	//light.push_back(l);

	projectionLocation = glGetUniformLocation(program,"projection");

	sgraph.initShaderProgram(program);
	
	string path= "empty.png";
	string name= "empty";

    Texture *tex = new Texture();
	tex->createImage(path);
	tex->setName(name);
	sgraph.addTexture(tex);
}

void View::draw()
{
	time += 0.001;
	sgraph.animate(time);
    /*
     *The modelview matrix for the View class is going to store the world-to-view transformation
     *This effectively is the transformation that changes when the camera parameters chang
     *This matrix is provided by glm::lookAt
     */
	glUseProgram(program);
    while (!modelview.empty())
        modelview.pop();
	GLuint a;

    modelview.push(glm::mat4(1.0));
	modelview.top() = modelview.top() * glm::lookAt(glm::vec3(0,0,80),glm::vec3(0,0,0),glm::vec3(0,1,0)) * trackballTransform;

	glUniformMatrix4fv(projectionLocation,1,GL_FALSE,glm::value_ptr(proj.top()));
	/*
     *Instead of directly supplying the modelview matrix to the shader here, we pass it to the objects
     *This is because the object's transform will be multiplied to it before it is sent to the shader
     *for vertices of that object only.
     *
     *Since every object is in control of its own color, we also pass it the ID of the color
     *in the activated shader program.
     *
     *This is so that the objects can supply some of their attributes without having any direct control
     *of the shader itself.
     */
	a = glGetError();
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	sgraph.raytrace(WINDOW_WIDTH,WINDOW_HEIGHT,modelview);
	//sgraph.raytrace(400,400, modelview);
    sgraph.draw(modelview);
	a = glGetError();
    glFinish();
	a = glGetError();
	glUseProgram(0);
    modelview.pop();
}

void View::mousepress(int x, int y)
{
    prev_mouse = glm::vec2(x,y);
}

void View::mousemove(int x, int y)
{
    int dx,dy;

    dx = x - prev_mouse.x;
    dy = (y) - prev_mouse.y;

	if ((dx==0) && (dy==0))
		return;

    //(-dy,dx) gives the axis of rotation

    //the angle of rotation is calculated in radians by assuming that the radius of the trackball is 300
    float angle = sqrt((float)dx*dx+dy*dy)/300;
   
    prev_mouse = glm::vec2(x,y);
	

    trackballTransform = glm::rotate(glm::mat4(1.0),angle,glm::vec3(-dy,dx,0)) * trackballTransform;
	if (camera==1)
		direction1=glm::rotate(direction1,angle,glm::vec3(-dy,dx,0));
	else
		direction2=glm::rotate(direction2,angle,glm::vec3(-dy,dx,0));
}

/*
 *This is a helper function that will take shaders info as a parameter, compiles them and links them
 *into a shader program.
 *
 *This function is standard and should not change from one program to the next.
 */

GLuint View::createShaders(ShaderInfo *shaders)
{
    ifstream file;
    GLuint shaderProgram;
    GLint linked;

    ShaderInfo *entries = shaders;

    shaderProgram = glCreateProgram();


    while (entries->type !=GL_NONE)
    {
        file.open(entries->filename.c_str());
        GLint compiled;


        if (!file.is_open())
            return false;

        string source,line;


        getline(file,line);
        while (!file.eof())
        {
            source = source + "\n" + line;
            getline(file,line);
        }
        file.close();


        const char *codev = source.c_str();


        entries->shader = glCreateShader(entries->type);
        glShaderSource(entries->shader,1,&codev,NULL);
        glCompileShader(entries->shader);
        glGetShaderiv(entries->shader,GL_COMPILE_STATUS,&compiled);

        if (!compiled)
        {
            printShaderInfoLog(entries->shader);
            for (ShaderInfo *processed = shaders;processed->type!=GL_NONE;processed++)
            {
                glDeleteShader(processed->shader);
                processed->shader = 0;
            }
            return 0;
        }
        glAttachShader( shaderProgram, entries->shader );
        entries++;
    }

    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram,GL_LINK_STATUS,&linked);

    if (!linked)
    {
        printShaderInfoLog(entries->shader);
        for (ShaderInfo *processed = shaders;processed->type!=GL_NONE;processed++)
        {
            glDeleteShader(processed->shader);
            processed->shader = 0;
        }
        return 0;
    }

    return shaderProgram;
}

void View::printShaderInfoLog(GLuint shader)
{
    int infologLen = 0;
    int charsWritten = 0;
    GLubyte *infoLog;

    glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&infologLen);
    //	printOpenGLError();
    if (infologLen>0)
    {
        infoLog = (GLubyte *)malloc(infologLen);
        if (infoLog != NULL)
        {
            glGetShaderInfoLog(shader,infologLen,&charsWritten,(char *)infoLog);
            printf("InfoLog: %s\n\n",infoLog);
            free(infoLog);
        }

    }
//	printOpenGLError();
}

void View::getOpenGLVersion(int *major,int *minor)
{
    const char *verstr = (const char *)glGetString(GL_VERSION);
    if ((verstr == NULL) || (sscanf_s(verstr,"%d.%d",major,minor)!=2))
    {
        *major = *minor = 0;
    }
}

void View::getGLSLVersion(int *major,int *minor)
{
    int gl_major,gl_minor;

    getOpenGLVersion(&gl_major,&gl_minor);
    *major = *minor = 0;

    if (gl_major==1)
    {
        /* GL v1.x can only provide GLSL v1.00 as an extension */
        const char *extstr = (const char *)glGetString(GL_EXTENSIONS);
        if ((extstr!=NULL) && (strstr(extstr,"GL_ARB_shading_language_100")!=NULL))
        {
            *major = 1;
            *minor = 0;
        }
    }
    else if (gl_major>=2)
    {
        /* GL v2.0 and greater must parse the version string */
        const char *verstr = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
        if ((verstr==NULL) || (sscanf_s(verstr,"%d.%d",major,minor) !=2))
        {
            *major = 0;
            *minor = 0;
        }
    }
}
