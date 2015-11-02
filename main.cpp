#include <fstream>
#include <cstdlib>
#include <vector>
#include <map>
#include <string.h>  // for memcmp()
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL.h>
#include <iostream>

#include "Planet.h"

using namespace std;

GLFWwindow * window;

// universe for all objects
glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 sun_ModelMatrix = glm::mat4(1.0f);

glm::vec3 lightPosition=glm::vec3(0,0,0);
glm::vec3 position = glm::vec3( 20, 20, 20 );
float horizontalAngle = 3.92;
float verticalAngle = 2.35f - 3.14f;
float initialFoV = 45.0f;
float speed = 3.f; // 3 units / second
float mouseSpeed = 0.005f;
bool goForward =false;

GLuint MVPID;
GLuint RenderID;
GLuint MyTextureSamplerID;
GLuint modelMatrixID;
GLuint viewMatrixID;
GLuint lightPositionID;



GLuint loadShaders(const char * vertex_file_path,const char * fragment_file_path)
{
    //A shader object is used to maintain the source code strings that define a shader.
    GLuint VertexShaderID=glCreateShader(GL_VERTEX_SHADER);  //creates an empty shader object and returns a non-zero value by which it can be referenced
    GLuint FragmentShaderID=glCreateShader(GL_FRAGMENT_SHADER);
    std::cout<<"The vertex shader ID= "<<VertexShaderID<<std::endl;
    cout<<"The fragment shader ID= "<<FragmentShaderID<<std::endl;
    
    // open shader file to get its source code
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path,std::ios::in);
    if(VertexShaderStream.is_open()){
        std::string line="";
        while(getline(VertexShaderStream,line)){
            VertexShaderCode+=line+"\n";
        }
        VertexShaderStream.close();
    }
    else{
        std::cout<<"Fatal Error! can't open "<<vertex_file_path<<std::endl;
        return -1;
    }
    
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path,std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string line="";
        while(getline(FragmentShaderStream,line)){
            FragmentShaderCode+=line+"\n";
        }
        FragmentShaderStream.close();
    }
    else{
        std::cout<<"Fatal Error! can't open "<<fragment_file_path<<std::endl;
        return -1;
    }
    
    GLint Result=GL_FALSE;
    int infoLogLength;
    
    //    std::cout<<"Compiling shader "<<vertex_file_path<<std::endl;
    char const * VertexSourcePointer=VertexShaderCode.c_str();
    glShaderSource(VertexShaderID,1,&VertexSourcePointer,NULL); //sets the source code in shader to the source code in the array of strings specified by string
    glCompileShader(VertexShaderID); //compiles the source code strings that have been stored in the shader object specified by shader.
    
    glGetShaderiv(VertexShaderID,GL_COMPILE_STATUS,&Result);  //returns in Result the value of a parameter for a specific shader object.
    glGetShaderiv(VertexShaderID,GL_INFO_LOG_LENGTH,&infoLogLength);  // similiar, but returns  in infoLogLength.
    char VertexShaderErrorMessage[(const int )infoLogLength+1];
    glGetShaderInfoLog(VertexShaderID,infoLogLength,NULL,VertexShaderErrorMessage);    //returns the information log for the specified shader object.
    //    std::cout<<"The state of compiling vertex shader is: "<<VertexShaderErrorMessage<<std::endl;
    
    //    std::cout<<"Compiling shader "<<fragment_file_path<<std::endl;
    char const * FragmentSourcePointer=FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID,1,&FragmentSourcePointer,NULL);
    glCompileShader(FragmentShaderID);
    
    glGetShaderiv(FragmentShaderID,GL_COMPILE_STATUS,&Result);
    glGetShaderiv(FragmentShaderID,GL_INFO_LOG_LENGTH,&infoLogLength);
    char FragmentShaderErrorMessage[(const int )infoLogLength+1];
    glGetShaderInfoLog(FragmentShaderID,infoLogLength,NULL,FragmentShaderErrorMessage);
    //    std::cout<<"The state of compiling fragment shader is: "<<FragmentShaderErrorMessage<<std::endl;
    
    std::cout<<"Linking program..."<<std::endl;
    GLuint ProgramID=glCreateProgram();   //creates an empty program object and returns a non-zero value by which it can be referenced.
    glAttachShader(ProgramID,VertexShaderID);  //  attach order doesn't matter
    glAttachShader(ProgramID,FragmentShaderID);  //Multiple shader objects of the same type may not be attached to a single program object.
    glLinkProgram(ProgramID);   // once the link is successful, then each shader can do its work
    
    glGetProgramiv(ProgramID,GL_LINK_STATUS,&Result);     // similiar to glGetShaderiv()
    glGetProgramiv(ProgramID,GL_INFO_LOG_LENGTH,&infoLogLength);
    char LinkErrorMessage[(const int )infoLogLength+1];
    glGetProgramInfoLog(ProgramID,infoLogLength,NULL,LinkErrorMessage);
    //    std::cout<<"The state of linking program is: "<<LinkErrorMessage<<std::endl;
    
    glDeleteShader(VertexShaderID);  //frees the memory and invalidates the name associated with the shader object specified by shader
    glDeleteShader(FragmentShaderID);  // undoes all effect by calling glCreateShader()
    
    return ProgramID;
}

int initWindow()
{
    if (!glfwInit()) {
        std::cout<<"glfw init failed!"<<std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window=glfwCreateWindow(1200,800, "My SolarSystem!", NULL, NULL);
    if (window==NULL) {
        std::cout<<"create window failed!"<<std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental=true;
    if (glewInit()!=GLEW_OK) {
        std::cout<<"Failed to init glew"<<std::endl;
        return -1;
    }
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    // to see the orbital clearly , need change the point size
    glEnable(GL_PROGRAM_POINT_SIZE);
    return 0;
}

void computeMatricesFromInputs()
{
    static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);
    lastTime = currentTime;
    
    static double old_xpos,old_ypos;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);


    // 通过鼠标来确定前进角度
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS) 
    {
//        cout<<"mouse:xpos="<<xpos<<",ypos="<<ypos<<endl;
        horizontalAngle += mouseSpeed * float(old_xpos - xpos);
        verticalAngle   += mouseSpeed * float(old_ypos - ypos );
//        std::cout<<"The vertical angle="<<verticalAngle<<",The horizontal angle="<<horizontalAngle<<std::endl;
    }
    // 通过jkli按键来确定前进角度
    if(glfwGetKey (window, GLFW_KEY_J) ==GLFW_PRESS)
    {
//        verticalAngle = 0;
        horizontalAngle -= 0.01;
//        std::cout<<"The vertical angle="<<verticalAngle<<",The horizontal angle="<<horizontalAngle<<std::endl;
    }

    if(glfwGetKey (window, GLFW_KEY_L) ==GLFW_PRESS)
    {
//        verticalAngle = 0;
        horizontalAngle += 0.01;
//        std::cout<<"The vertical angle="<<verticalAngle<<",The horizontal angle="<<horizontalAngle<<std::endl;
    }

    if(glfwGetKey (window, GLFW_KEY_I) ==GLFW_PRESS)
    {
//        horizontalAngle = 0;
        verticalAngle += 0.01;
//        std::cout<<"The vertical angle="<<verticalAngle<<",The horizontal angle="<<horizontalAngle<<std::endl;
    }

    if(glfwGetKey (window, GLFW_KEY_K) ==GLFW_PRESS)
    {
//        horizontalAngle = 0;
        verticalAngle -= 0.01;
//        std::cout<<"The vertical angle="<<verticalAngle<<",The horizontal angle="<<horizontalAngle<<std::endl;
    }

    old_xpos=xpos;
    old_ypos=ypos;
    
    glm::vec3 direction(
                        cos(verticalAngle) * sin(horizontalAngle),
                        sin(verticalAngle),
                        cos(verticalAngle) * cos(horizontalAngle)
                        );
    
    glm::vec3 right = glm::vec3(
                                sin(horizontalAngle - 3.14f/2.0f),
                                0,
                                cos(horizontalAngle - 3.14f/2.0f)
                                );
    
    glm::vec3 up = glm::cross( right, direction );

    // Move forward
    if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS || goForward)
    {
        position += direction * deltaTime * speed;
//        cout<<"your space position is:\n\txpos="<<position.x<<",ypos="<<position.y<<",zpos="<<position.z<<endl;
    }
    // Move backward
    if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS)
    {
        position -= direction * deltaTime * speed;
//        cout<<"your space position is:\n\txpos="<<position.x<<",ypos="<<position.y<<",zpos="<<position.z<<endl;
    }
    // Strafe right
    if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS)
    {
        position += right * deltaTime * speed;
//        cout<<"your space position is:\n\txpos="<<position.x<<",ypos="<<position.y<<",zpos="<<position.z<<endl;
    }
    // Strafe left
    if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS)
    {
        position -= right * deltaTime * speed;
//        cout<<"your space position is:\n\txpos="<<position.x<<",ypos="<<position.y<<",zpos="<<position.z<<endl;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE ) == GLFW_PRESS)
    {
        goForward = (goForward)? false:true;
    }

    if( glfwGetKey(window, GLFW_KEY_TAB ) == GLFW_PRESS )
    {
        position = glm::vec3( 10, 10, 10 );
        up = glm::vec3(0.0, 1.0, 0.0);
        horizontalAngle = 3.92;
        verticalAngle = -0.79;

//        cout<<endl;
//        cout<<"使用空间跳转成功！"<<endl;
//        cout<<"space postion reset!"<<endl;
    }

//    cout<<"Now you are at:("<<position.x<<","<<position.y<<","<<position.z<<")"<<endl;
    
    
    float FoV = initialFoV ;//- 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.
    ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
    ViewMatrix       = glm::lookAt(
                                   position,           // Camera is here
                                   position+direction, // and looks here : at the same position, plus "direction"
                                   up                  // Head is up (set to 0,-1,0 to look upside-down)
                                   );
    
}

void draw(Planet & planet)
{
    int choose=2;

    glm::mat4 MVP=ProjectionMatrix * ViewMatrix * planet.self_ModelMatrix;

    glActiveTexture(GL_TEXTURE0);   //selects which texture unit subsequent texture state calls will affect. The initial value is GL_TEXTURE0.
    glBindTexture(GL_TEXTURE_2D, planet.texture);   //bind a named texture to a texturing target
    glUniform1i(MyTextureSamplerID,0);    //  0-->GL_TEXTURE0
    glUniformMatrix4fv(MVPID,1,GL_FALSE,&MVP[0][0]);
    glUniformMatrix4fv(modelMatrixID,1,GL_FALSE,&planet.self_ModelMatrix[0][0]);
    glUniformMatrix4fv(viewMatrixID,1,GL_FALSE,&ViewMatrix[0][0]);
    glUniform3f(lightPositionID,lightPosition.x,lightPosition.y,lightPosition.z);
    glUniform1i(RenderID,choose);

    // vertex position
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,planet.vertexBuffer);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
    // texture uv
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER,planet.uvBuffer);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,0,(void *)0);
    // vertex normal
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER,planet.normalBuffer);
    glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,0,(void *)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,planet.elementBuffer);
    glDrawElements(GL_TRIANGLES, (GLuint)planet.indices.size(), GL_UNSIGNED_SHORT, (void *)0);

    if(!planet.isFixedStar)      // draw orbital
    {
        MVP=ProjectionMatrix * ViewMatrix * planet.parent_ModelMatrix;
        glUniformMatrix4fv(MVPID,1,GL_FALSE,&MVP[0][0]);
        glUniformMatrix4fv(modelMatrixID,1,GL_FALSE,&planet.parent_ModelMatrix[0][0]);

        glBindBuffer(GL_ARRAY_BUFFER,planet.orbital_vertexBuffer);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);

        glDrawArrays(GL_POINTS,0,(GLuint) planet.orbital_vertices.size());
    }

    glDisableVertexAttribArray(0);   // AttribArray 必须在调用glDrawArrays之后才能关闭
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
}

int main(int argc, const char * argv[])
{    
    if (initWindow()!=0) {
        return -1;
    }

    GLuint ProgramID=loadShaders("VertexShader.glsl", "FragmentShader.glsl");
    if (ProgramID==-1) 
    {
        cout<<"ProgramID=-1"<<endl;
        return -1;
    }
    
    glUseProgram(ProgramID);

    MVPID=glGetUniformLocation(ProgramID,"MVP");
    RenderID=glGetUniformLocation(ProgramID,"choose");
    MyTextureSamplerID=glGetUniformLocation(ProgramID,"MyTextureSampler");
    modelMatrixID=glGetUniformLocation(ProgramID,"modelMatrix");
    viewMatrixID=glGetUniformLocation(ProgramID,"viewMatrix");
    lightPositionID=glGetUniformLocation(ProgramID,"lightPosition_worldspace");
    
    GLuint vertexArray;
    glGenVertexArrays(1,&vertexArray);
    glBindVertexArray(vertexArray);


    string objPath = "./object/ball_hd.obj";
    string texturePath = "./texture/sun_3000x1500.png";
    Planet sun = Planet(objPath,texturePath,1.5,true);

    texturePath = "./texture/Metal-Plastic-Texture_1920x1080.png";
    Planet metalPlanet = Planet(objPath,texturePath,0.8, false);
    metalPlanet.set_orbital(15,6);
    metalPlanet.set_velocity(2);

    texturePath = "./texture/earth_low.png";
    Planet earth = Planet(objPath,texturePath,0.5,false);
    earth.set_orbital(10,7);

    texturePath = "./texture/gas_2400x1200.png";
    Planet gasPlanet = Planet(objPath,texturePath,0.9,false);
    gasPlanet.set_orbital(15,20);

    texturePath = "./texture/moon_1500x750.png";
    Planet moon = Planet(objPath,texturePath,0.1, false);
    moon.set_orbital(2,2);   // orbital correspond to the earth
    moon.set_velocity(5);

    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    do
    {
        double currentTime=glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime > 1.0)
        {
            // printf("%f ms/frame\n", 1000.0/double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(ProgramID);
        
        computeMatricesFromInputs();


//        glEnableVertexAttribArray(0);
//        glEnableVertexAttribArray(2);
//        glEnableVertexAttribArray(3);


        earth.update();
        moon.update(earth.get_model_matrix());
        metalPlanet.update();
        gasPlanet.update();

        draw(earth);
        draw(sun);
        draw(moon);
        draw(metalPlanet);
        draw(gasPlanet);

//        sun.draw(ViewMatrix,ProjectionMatrix,lightPosition);




//        glDisableVertexAttribArray(0);   // AttribArray 必须在调用glDrawxxx 之后才能关闭
//        glDisableVertexAttribArray(2);
//        glDisableVertexAttribArray(3);

        glfwSwapBuffers(window);
        glfwPollEvents();
        
    }while (glfwGetKey(window, GLFW_KEY_ESCAPE)!=GLFW_PRESS && glfwWindowShouldClose(window)==0);
    
    sun.destroy();
    earth.destroy();
    glDeleteProgram(ProgramID);
    glDeleteVertexArrays(1,&vertexArray);
    glfwTerminate();

}
