#version 410

// Incoming per vertex... position and normal
in vec4 vVertex;
in vec3 vNormal;

// from app	
uniform vec3	vLightPosition;
uniform mat4	mvpMatrix;
uniform mat4	mvMatrix;
uniform mat3	normalMatrix;

// Color to fragment program
out vec3 vVaryingLightDir;
out vec3 vVaryingNormal;

void main(void) 
 { 
    // surface normal in eye coordinates
    vVaryingNormal = normalMatrix * vNormal;

    // vertex position in eye coordinates
     vec4 vPosition4 = mvMatrix * vVertex;
    vec3 vPosition3 = vPosition4.xyz / vPosition4.w;

    // vector to the light source
    vVaryingLightDir = normalize(vLightPosition - vPosition3);
    
    // transform geometry
    gl_Position = mvpMatrix * vVertex;    
 }