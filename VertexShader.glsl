#version 410
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec3 vertexNormal_modelspace;

uniform int choose;
uniform mat4 MVP;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform vec3 lightPosition_worldspace;

out vec3 FragmentColor;
out vec2 UV;
out vec3 position_worldspace;
out vec3 normal_cameraspace;
out vec3 eyeDirection_cameraspace;
out vec3 lightDirection_cameraspace;

void main()
{
    gl_PointSize = 4;
    
    if(choose!=0)
    {
        //带有光线的渲染
        if(choose!=1)
        {
            position_worldspace=(modelMatrix * vec4(vertexPosition_modelspace,1)).xyz;
            
            // cameraspace的坐标原点是camera，所以是从物体到视点camera的向量
            eyeDirection_cameraspace=vec3(0,0,0)-(viewMatrix * modelMatrix * vec4(vertexPosition_modelspace,1)).xyz;

            vec3 lightPosition_cameraspace = ( viewMatrix * vec4(lightPosition_worldspace,1)).xyz;

            //光线到物体的反向向量?
            lightDirection_cameraspace = lightPosition_cameraspace + eyeDirection_cameraspace;
            normal_cameraspace = ( viewMatrix * modelMatrix * vec4(vertexNormal_modelspace,0)).xyz;
        }

        //  带有纹理的渲染
        UV=vertexUV;
    }

    
    gl_Position=MVP*vec4(vertexPosition_modelspace,1);

    FragmentColor=vec3(1,1,1);
}
