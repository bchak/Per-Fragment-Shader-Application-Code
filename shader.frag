#version 410

out vec4 vFragColor;

uniform vec4	diffuseColor;
uniform vec4    ambientColor;
uniform vec4    specularColor;

in vec3 vVaryingNormal;
in vec3 vVaryingLightDir;


void main(void)
{ 

    vFragColor = diffuseColor;
    
 }  
     
