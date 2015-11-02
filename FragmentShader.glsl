#version 410

in vec3 FragmentColor;
in vec2 UV;
in vec3 position_worldspace;
in vec3 normal_cameraspace;
in vec3 eyeDirection_cameraspace;
in vec3 lightDirection_cameraspace;

out vec3 color;

uniform sampler2D MyTextureSampler;
uniform int choose;
uniform vec3 lightPosition_worldspace;

void main()
{
    
    if (choose==0) 
    {
        color=FragmentColor;
    }
    else if(choose == 1)
    {
        color=texture(MyTextureSampler,UV).rgb;
    }
    else
    {
        vec3 lightColor=vec3(1,1,1);
        float lightPower=50.f;

        // mateiral properties
        vec3 materialDiffuseColor=texture(MyTextureSampler,UV).rgb;
        vec3 materialAmbientColor=vec3(0.5,0.5,0.5) * materialDiffuseColor;
        vec3 materialSpecularColor=vec3(0.4,0.3,0.3);

        //cosTheta
        float d=length(lightPosition_worldspace - position_worldspace);
        vec3 n=normalize( normal_cameraspace);
        vec3 l=normalize( lightDirection_cameraspace );
        float cosTheta = clamp( dot(n,l),0,1);   //constrain a value to [0,1]

        //cosAlpha
        vec3 E=normalize( eyeDirection_cameraspace );   // E的方向是从物体指向视点camera
        
        // R 是远离表面的向量，即正常的反射的向量
        vec3 R=reflect(-l,n);
        float cosAlpha=clamp(dot(E,R),0,1);
        
        color=materialAmbientColor
            + materialDiffuseColor * lightColor * lightPower * cosTheta/(d*d)
            + materialSpecularColor * lightColor * lightPower * pow(cosAlpha,5)/(d*d);
    }
}
