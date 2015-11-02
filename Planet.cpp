//
// Created by zieng on 11/1/15.
//

#include "Planet.h"

Planet::Planet(string obj_path, string texture_path,double scale, bool is_fixed)
{
//    MVPID = 0;
//    RenderID = 0;
//    MyTextureSamplerID = 0;
//    modelMatrixID = 0;
//    viewMatrixID = 0;
//    lightPositionID = 0;
    isFixedStar = is_fixed;
    self_ModelMatrix = glm::mat4(1.0f);
    velocity = 1;

    lastTime = glfwGetTime();



    vector<glm::vec3> vertices,normals;
    vector<glm::vec2> uvs;

    if (load_OBJ(obj_path.c_str(),vertices,uvs,normals) == false)
    {
        cout<<"res=false"<<endl;
        return;
    }

    texture = load_texture(texture_path.c_str());
    if(texture == 0)
    {
        cout<<"load textre failed"<<endl;
        return ;
    }

    if(scale==0)
    {
        cout<<"the scale of an object can't be zero"<<endl;
        return ;
    }

    if(is_fixed)
        self_ModelMatrix = glm::mat4(1.0f);

    destroyed = false;

    scale = (scale < 0)?-scale:scale;
    for(int i=0;i<vertices.size();i++)
        vertices[i] *= scale;

    VBO_indexer(vertices, uvs, normals, indexed_vertices, indexed_uvs, indexed_normals, indices);

    glGenBuffers(1,&vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1,&uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,uvBuffer);
    glBufferData(GL_ARRAY_BUFFER,indexed_uvs.size()* sizeof(glm::vec2),&indexed_uvs[0],GL_STATIC_DRAW);

    glGenBuffers(1,&normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,normalBuffer);
    glBufferData(GL_ARRAY_BUFFER,indexed_normals.size()* sizeof(glm::vec3),&indexed_normals[0],GL_STATIC_DRAW);


    glGenBuffers(1,&elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size() * sizeof(unsigned short), &indices[0],GL_STATIC_DRAW);
}

Planet::~Planet()
{
    if(!destroyed)
        destroy();
}

//void Planet::set_uniform(GLuint MVPID_, GLuint RenderID_, GLuint MyTextureSamplerID_, GLuint modelMatrixID_,
//                         GLuint viewMatrixID_, GLuint lightPositionID_)
//{
//    MVPID = MVPID_;
//    RenderID = RenderID_;
//    MyTextureSamplereID = MyTextureSamplereID_;
//    modelMatrixID = modelMatrixID_;
//    viewMatrixID = viewMatrixID_;
//    lightPositionID = lightPositionID_;
//}

bool Planet::load_OBJ(const char *objPath, std::vector<glm::vec3> &out_vertex, std::vector<glm::vec2> &out_uv, std::vector<glm::vec3> &out_normal)
{
    vector<unsigned int > vertexIndices,uvIndices,normalIndices;
    vector<glm::vec3> temp_vertices;
    vector<glm::vec2> temp_uvs;
    vector<glm::vec3> temp_normals;

    FILE * file=fopen(objPath, "rb");
    if (file==NULL)
    {
        std::cout<<"open obj file failed!"<<std::endl;
        return false;
    }
    while (1)
    {
        char lineHeader[128];
        if (fscanf(file, "%s",lineHeader)==EOF)
        {
            break;
        }

        if (strcmp(lineHeader, "v")==0)
        {
            glm::vec3 newVertex;
            fscanf(file, "%f %f %f\n",&newVertex.x,&newVertex.y,&newVertex.z);
            temp_vertices.push_back(newVertex);
        }
        else if (strcmp(lineHeader, "vt")==0)
        {
            glm::vec2 newUV;
            fscanf(file, "%f %f\n",&newUV.x,&newUV.y);
            newUV.y=-newUV.y;
            temp_uvs.push_back(newUV);
        }
        else if (strcmp(lineHeader, "vn")==0)
        {
            glm::vec3 newNormal;
            fscanf(file, "%f %f %f\n",&newNormal.x,&newNormal.y,&newNormal.z);
            temp_normals.push_back(newNormal);
        }
        else if (strcmp(lineHeader, "f")==0)
        {
            std::string vertex1,vertex2,vertex3;
            unsigned int vertexIndex[3],uvIndex[3],normalIndex[3];
            int matches=fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d",&vertexIndex[0],&uvIndex[0],&normalIndex[0],
                               &vertexIndex[1],&uvIndex[1],&normalIndex[1],
                               &vertexIndex[2],&uvIndex[2],&normalIndex[2]);
            if (matches!=9)
            {
                std::cout<<"format mis-match"<<std::endl;
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
        else
        {
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }

    }
    for( unsigned int i=0; i<vertexIndices.size(); i++ )
    {

        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];

        // Get the attributes thanks to the index
        glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
        glm::vec2 uv = temp_uvs[ uvIndex-1 ];
        glm::vec3 normal = temp_normals[ normalIndex-1 ];

        // Put the attributes in buffers
        out_vertex.push_back(vertex);
        out_uv     .push_back(uv);
        out_normal .push_back(normal);
    }

    return true;

}

void Planet::VBO_indexer(vector<glm::vec3> &in_vertices, vector<glm::vec2> &in_uvs, vector<glm::vec3> &in_normals,
                         vector<glm::vec3> &out_vertices, vector<glm::vec2> &out_uvs, vector<glm::vec3> &out_normals,
                         vector<unsigned short> &indices)
{
    map<PackedVertex,unsigned short> indexedVertices;

    for (int i=0; i<in_vertices.size(); i++) {
        PackedVertex p={in_vertices[i],in_uvs[i],in_normals[i]};
        bool Indexed=false;
        unsigned short index;

        map<PackedVertex, unsigned short>::iterator it=indexedVertices.find(p);
        if (it!=indexedVertices.end()) {
            Indexed=true;
            index=it->second;
        }

        if (Indexed) {
            indices.push_back(index);
        }
        else{
            out_vertices.push_back(in_vertices[i]);
            out_uvs.push_back(in_uvs[i]);
            out_normals.push_back(in_normals[i]);
            unsigned short newIndex=(unsigned short)out_vertices.size()-1;
            indices.push_back(newIndex);
            indexedVertices[p]=newIndex;
        }
    }
}

GLuint Planet::load_texture(const char *image_path)
{
    GLuint Texture;
    int image_channels;
    int image_width_d, image_height_d;
    unsigned char * image_buffer;

    glGenTextures(1,&Texture);
    glBindTexture(GL_TEXTURE_2D, Texture);
    image_buffer = SOIL_load_image(image_path, &image_width_d, &image_height_d, &image_channels, 4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width_d, image_height_d, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_buffer);
    SOIL_free_image_data(image_buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    /* check for an error during the load process */
    if( 0 == Texture )
    {
        printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
    }

    cout<<"Texture load successfully! texture="<<Texture<<endl;

    return Texture;
}

//void Planet::draw(glm::mat4 ViewMatrix,glm::mat4 ProjectionMatrix,glm::vec3 lightPosition)
//{
//    int choose=2;
//
//    glm::mat4 MVP=ProjectionMatrix * ViewMatrix * self_ModelMatrix;
//
//    glActiveTexture(GL_TEXTURE0);   //selects which texture unit subsequent texture state calls will affect. The initial value is GL_TEXTURE0.
//    glBindTexture(GL_TEXTURE_2D, texture);   //bind a named texture to a texturing target
//    glUniform1i(MyTextureSamplerID,0);    //  0-->GL_TEXTURE0
//    glUniformMatrix4fv(MVPID,1,GL_FALSE,&MVP[0][0]);
//    glUniformMatrix4fv(modelMatrixID,1,GL_FALSE,&self_ModelMatrix[0][0]);
//    glUniformMatrix4fv(viewMatrixID,1,GL_FALSE,&ViewMatrix[0][0]);
//    glUniform3f(lightPositionID,lightPosition.x,lightPosition.y,lightPosition.z);
//    glUniform1i(RenderID,choose);
//
//    // vertex position
//    glEnableVertexAttribArray(0);
//    glBindBuffer(GL_ARRAY_BUFFER,vertexBuffer);
//    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
//    // texture uv
//    glEnableVertexAttribArray(2);
//    glBindBuffer(GL_ARRAY_BUFFER,uvBuffer);
//    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,0,(void *)0);
//    // vertex normal
//    glEnableVertexAttribArray(3);
//    glBindBuffer(GL_ARRAY_BUFFER,normalBuffer);
//    glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,0,(void *)0);
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,elementBuffer);
//    glDrawElements(GL_TRIANGLES, (GLuint)indices.size(), GL_UNSIGNED_SHORT, (void *)0);
//
//    glDisableVertexAttribArray(0);   // AttribArray 必须在调用glDrawArrays之后才能关闭
//    glDisableVertexAttribArray(2);
//    glDisableVertexAttribArray(3);
//}

void Planet::set_position(glm::vec3 p)
{
    position = p;

    theta = 0;

    //todo calculate the model matrix from position

}

void Planet::set_position(glm::mat4 new_model_matrix)
{
    self_ModelMatrix = new_model_matrix;

    //todo calclute the position from modle_matrix

}
void Planet::update()
{
    glm::mat4 m = glm::mat4(1.0f);

    update(m);
}

void Planet::update(glm::mat4 parentModelMatrix)
{
    parent_ModelMatrix = parentModelMatrix;
    float parentX=0,parentY=0,parentZ=0;
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);
    lastTime = currentTime;

//    cout<<"???"<<endl;
//    cout<<"delta time="<<deltaTime<<endl;
    if(isFixedStar)  // fixed star no need to update
        return;

    theta += 1*deltaTime * velocity;

    parentX = parentModelMatrix[3][0];
    parentY = parentModelMatrix[3][1];
    parentZ = parentModelMatrix[3][2];

    position.x = a * glm::cos(theta) + 0;//parentX;
    position.y = b * glm::sin(theta) + 0;//parentY;
    position.z = 0 + parentZ;

    //todo update the model matrix
    glm::vec4 c1,c2,c3,c4;
//    c1 = glm::vec4(1,0,0,position.x);
//    c2 = glm::vec4(0,1,0,position.y);
//    c3 = glm::vec4(0,0,1,position.z);
//    c4 = glm::vec4(0,0,0,1);

    c1 = glm::vec4(1,0,0,0);
    c2 = glm::vec4(0,1,0,0);
    c3 = glm::vec4(0,0,1,0);
    c4 = glm::vec4(position.x,position.y,position.z,1);

    self_ModelMatrix = parentModelMatrix * glm::mat4(c1,c2,c3,c4);
//    self_ModelMatrix = glm::mat4(c1,c2,c3,c4);

}

void Planet::set_fixed(bool b)
{
    isFixedStar = b;
}

void Planet::set_orbital(int aa, int bb)
{
    if(isFixedStar)
    {
        cout<<"Fixed star no need set orbital!"<<endl;
        return ;
    }
    a = aa;
    b = bb;
//    theta = (double) random();
    theta = 0;

    glm::vec3 vertex;
    for(float i=0;i<2*3.11415926539;i+=0.01)
    {
        vertex.x = a * glm::cos(theta+i);
        vertex.y = b * glm::sin(theta+i);
        vertex.z = 0;

        orbital_vertices.push_back(vertex);
    }

    glGenBuffers(1,&orbital_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,orbital_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, orbital_vertices.size() * sizeof(glm::vec3), &orbital_vertices[0], GL_STATIC_DRAW);

    // calcualte the self model matrix
    update();
}

void Planet::set_scale(double s)
{
    if(s==0)
    {
        cout<<"the scale of an object can't be zero"<<endl;
        return;
    }
    s=(s<0)?-s:s;

    for(int i=0;i<indexed_vertices.size();i++)
    {
        indexed_vertices[i] *= s;
    }

    glBindBuffer(GL_ARRAY_BUFFER,vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

}

void Planet::destroy()
{
    glDeleteBuffers(1,&vertexBuffer);
    glDeleteBuffers(1,&uvBuffer);
    glDeleteBuffers(1,&normalBuffer);
    glDeleteTextures(1,&texture);
    destroyed = true;
}

glm::mat4 Planet::get_model_matrix()
{
    return self_ModelMatrix;
}

void Planet::set_velocity(float speed)
{
    velocity = speed;
}

