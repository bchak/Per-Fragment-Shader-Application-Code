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
    // surface normal 
    vVaryingNormal = vNormal;

    // light source
    vVaryingLightDir = vLightPosition;
    
    // transform geometry
    gl_Position = mvpMatrix * vVertex;    
 }