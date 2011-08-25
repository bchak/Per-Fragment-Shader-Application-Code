#version 410

out vec4 vFragColor;

uniform vec4	diffuseColor;
uniform vec4    ambientColor;
uniform vec4    specularColor;

in vec3 vVaryingNormal;
in vec3 vVaryingLightDir;


void main(void)
{ 

   // Dot product to compute diffuse intensity
    float diff = max(0.0, dot(normalize(vVaryingNormal), normalize(vVaryingLightDir)));
    
    //vVaryingColor = vec4(1.0, 0.0, 0.0, 1.0);
    
    // Multiply the intensity by diffuse color
   // vVaryingColor.rgb = diff * diffuseColor.rgb;
   // vVaryingColor.a = diffuseColor.a;
   vFragColor = diff * diffuseColor;
    
    //add ambient
    vFragColor += ambientColor;
    
    // specular light
    vec3 vReflection = normalize(reflect(-normalize(vVaryingLightDir),normalize(vVaryingNormal)));
    float spec = max(0.0, dot(normalize(vVaryingNormal),vReflection));
    if(diff !=0) {
      float fSpec = pow(spec,83.2);
      vFragColor.rgb += vec3(fSpec, fSpec, fSpec);
     }
 }  
     
