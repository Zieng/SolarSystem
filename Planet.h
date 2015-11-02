//
// Created by zieng on 11/1/15.
//

#ifndef SOLARSYSTEM_PLANET_H
#define SOLARSYSTEM_PLANET_H

#include <iostream>
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

using namespace std;

struct PackedVertex
{
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 normal;
    bool operator < (const PackedVertex that) const{
        return memcmp((void *)this, (void *) &that, sizeof(PackedVertex))>0;
    };
};

class Planet
{
public:
    Planet(string obj_path,string texture_path,double scale,bool is_fixe);
    ~Planet();

//    void set_uniform(GLuint MVPID,
//                     GLuint RenderID,
//                     GLuint MyTextureSamplereID,
//                     GLuint modelMatrixID,
//                     GLuint viewMatrixID,
//                     GLuint lightPositionID);

    bool load_OBJ(const char * objPath,
                  std::vector<glm::vec3> & out_vertex,
                  std::vector<glm::vec2> & out_uv,
                  std::vector<glm::vec3> & out_normal);

    void VBO_indexer(vector<glm::vec3> & in_vertices,
                     vector<glm::vec2> & in_uvs,
                     vector<glm::vec3> & in_normals,
                     vector<glm::vec3> & out_vertices,
                     vector<glm::vec2> & out_uvs,
                     vector<glm::vec3> & out_normals,
                     vector<unsigned short> & indices);

    GLuint load_texture(const char * image_path);

    void set_velocity(float speed);
    void set_scale(double s);
    void set_orbital(int aa,int bb);
    void set_fixed(bool b);
    void set_position(glm::vec3 p);
    void set_position(glm::mat4 new_model_matrix);
    void update();
    void update(glm::mat4 parentModelMatrix);
    glm::mat4 get_model_matrix();
    void draw_orbital();

    void destroy();

//    void draw(glm::mat4 ViewMatrix,glm::mat4 ProjectionMatrix,glm::vec3 lightPosition);

    // data
    GLuint texture;

    vector<unsigned short> indices;
    vector<glm::vec3> indexed_vertices;
    vector<glm::vec2> indexed_uvs;
    vector<glm::vec3> indexed_normals;

    vector<glm::vec3> orbital_vertices;

    GLuint vertexBuffer,uvBuffer,normalBuffer,elementBuffer,orbital_vertexBuffer;

    glm::mat4 self_ModelMatrix;
    glm::mat4 parent_ModelMatrix;

    // uniform variable for shader
    static GLuint MVPID;
    static GLuint RenderID;
    static GLuint MyTextureSamplerID;
    static GLuint modelMatrixID;
    static GLuint viewMatrixID;
    static GLuint lightPositionID;

    bool isFixedStar;
    bool destroyed;

    // variable for compute position
    float theta,velocity;
    int a,b;      // a,b for ellipse function
    glm::vec3 position;

    float lastTime;


};


#endif //SOLARSYSTEM_PLANET_H
