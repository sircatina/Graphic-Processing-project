#version 410 core

in vec3 fNormal;
in vec3 fPosition;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

// Lighting
uniform vec3 lightDir;
uniform vec3 lightColor;

// Texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

vec3 ambient;
float ambientStrength = 0.7f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

uniform float fogDensity;
uniform float isNight;

uniform vec3 spotLightPosEye1;
uniform vec3 spotLightPosEye2;
uniform vec3 spotLightPosEye3;
uniform vec3 targetPos1;
uniform vec3 targetPos2;
uniform vec3 targetPos3;

uniform vec3 punctiformLightPosEyeWaterTower;
uniform vec3 punctiformLightColorWaterTower;

void computeLightComponents()
{		
    vec3 cameraPosEye = vec3(0.0f); // In eye coordinates, the viewer is situated at the origin
    
    // Transform normal
    vec3 normalEye = normalize(fNormal);	
    
    // Compute light direction
    vec3 lightDirN = normalize(lightDir);
    
    // Compute view direction 
    vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
        
    // Compute ambient light
    ambient = ambientStrength * lightColor;
    
    // Compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
    
    // Compute specular light
    vec3 reflection = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
    specular = specularStrength * specCoeff * lightColor;
}

float computeFog() { 
    float fragmentDistance = length(fPosEye.xyz);
    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2)); 
    return clamp(fogFactor, 0.0f, 1.0f); 
}

vec3 compuneNightLight(){
    float ambientStrengthNight = 0.01; // Reducerea iluminării ambientale
    float diffuseStrengthNight = 0.01; // Reducerea iluminării difuze
    float specularStrengthNight = 0.01; // Reducerea iluminării specular

    vec3 ambientColor = ambient * ambientStrengthNight; // Reducerea iluminării ambientale
    vec3 diffuseColor = diffuse * diffuseStrengthNight; // Reducerea iluminării difuze
    vec3 specularColor = specular * specularStrengthNight; // Reducerea iluminării specular

    vec3 resultColor = ambientColor + diffuseColor + specularColor;
    return min(resultColor, vec3(1.0)); 
}

vec3 spotLight(vec3 spotLightPosEye, vec3 targetPos) {
    vec3 spotDirection = normalize(spotLightPosEye - targetPos);
    vec3 lightDirection = normalize(spotLightPosEye - fPosition);

    float cutOff = radians(55.0f);
    float outerCutOff = radians(40.0f);

    float theta = dot(lightDirection, normalize(spotDirection));
    float epsilon = cutOff - outerCutOff;
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0f, 1.0f);

    ambient = 0.001 * vec3(1.0f, 1.0f, 1.0f);

    diffuse = (max(dot(lightDirection, normalize(spotDirection)), 0.0f) * vec3(1.0f, 1.0f, 1.0f)) * intensity;

    vec3 reflectDir = reflect(-lightDirection, spotDirection);
    float specCoeff = pow(max(dot(lightDirection, normalize(spotDirection)), 0.0f), shininess);
    vec3 specularSpot = vec3((specularStrength * specCoeff * lightColor)) * intensity;

    return (min((ambient + diffuse) * texture(diffuseTexture, fTexCoords).rgb + specularSpot * texture(specularTexture, fTexCoords).rgb, vec3(1.0)));
}
vec3 punctiformLightWaterTower() {
    vec3 normalEye = normalize(normalMatrix * fNormal);
    //compute light direction
    vec3 lightDirN = normalize(punctiformLightPosEyeWaterTower - fPosition);

    //constants
    float constant = 1.0f;
    float linear = 1.7f;
    float quadratic = 3.8f;

    //compute distance to light
    float dist = length(punctiformLightPosEyeWaterTower - fPosition);

    //compute attentuation
    float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));

    //compute ambient light
    float ambientStrength2 = 50.2f;
    vec3 ambientContrib = att * ambientStrength2 * lightColor;

    //compute diffuse light 
    vec3 diffuseContrib = att * max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    vec3 viewDir = normalize(-fPosition);

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec3 specularContrib = att * specularStrength * specCoeff * lightColor;

    // Combine ambient, diffuse, and specular without the alpha channel
    vec3 combinedColor = (ambientContrib + diffuseContrib) * texture(diffuseTexture, fTexCoords).rgb + specularContrib * texture(specularTexture, fTexCoords).rgb;

    // Clamp the combined color to avoid over-brightness
    return min(combinedColor, vec3(1.0));
}

float computeShadow(){
  vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
  normalizedCoords = normalizedCoords * 0.5 + 0.5;
  if (normalizedCoords.z > 1.0f)
    return 0.0f;

  float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
  float currentDepth = normalizedCoords.z;
  float bias = 0.005f;
  float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;


  return shadow;
}
void main() 
{
    computeLightComponents();
    
    ambient *= texture(diffuseTexture, fTexCoords).rgb;
    diffuse *= texture(diffuseTexture, fTexCoords).rgb;
    specular *= texture(specularTexture, fTexCoords).rgb;
    float fogFactor = computeFog(); 
    vec3 color;
    if (isNight > 0.1f)
    {
        color = compuneNightLight() + punctiformLightWaterTower() + spotLight(spotLightPosEye1, targetPos1)+ spotLight(spotLightPosEye2, targetPos2)+ spotLight(spotLightPosEye3, targetPos3) ;
    }
    else{
	float shadow = computeShadow();
        color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
        }
    vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f); 
    
    fColor = mix(fogColor, vec4(color, 1.0f), fogFactor); 
}
