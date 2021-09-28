#version 330 core
out vec4 FragColor;


in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    mat3 TBN;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform sampler2D texture_height;
uniform sampler2D shadowMap;
uniform samplerCube skybox;

uniform float diluteAreaVariable;
uniform float cangiante;
uniform float dilution;
uniform float density;


uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 cameraPos;
uniform float height_scale;
 



struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;    
    float shininess;
}; 


uniform Material material;


struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cangiante;
    float dilution;
};  

uniform DirLight dirLight;

uniform float normalFlag;
uniform float parralaxFlag;
uniform float modelFlag;


struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cangiante;
    float dilution;
};  
#define NR_POINT_LIGHTS 4  
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct SpotLight {
    vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;

    float cangiante;
    float dilution;
};
uniform SpotLight spotLight;


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec2 texCoords);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords);
vec3 CalcSpotLight (SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords);
float ShadowCalculation(vec4 fragPosLightSpace, float bias);
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);


void main()
{   
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec2 texCoords = fs_in.TexCoords;
    vec3 TangentViewPos = transpose(fs_in.TBN) * viewPos;
    vec3 TangentFragPos = transpose(fs_in.TBN) * fs_in.FragPos;
    if(parralaxFlag == 1.0f){       
        viewDir   = normalize(TangentViewPos - TangentFragPos);
        texCoords = ParallaxMapping(fs_in.TexCoords,  viewDir);
        if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
            discard;
    }
    if(normalFlag == 1.0f){
        normal = texture(texture_normal, texCoords).rgb;
        normal = normal * 2.0 - 1.0;
        viewDir   = normalize(TangentViewPos - TangentFragPos);
    }
    vec3 result = vec3(0.0);
    result = CalcDirLight(dirLight, normal, viewDir, texCoords);
    // phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += 0.15 * CalcPointLight(pointLights[i], normal, fs_in.FragPos, viewDir, texCoords);
    // phase 3: Spot light
    //result += CalcSpotLight(spotLight, normal, fs_in.FragPos, viewDir, texCoords);

    FragColor = vec4(result, 1.0);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec2 texCoords)
{
    float areaOfDilution;

    // diffuse shading
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    if(normalFlag == 1.0f){
            lightDir =  normalize(fs_in.TBN * (lightPos - fs_in.FragPos));
    }
    float diff = max(dot(lightDir, normal), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, texCoords));
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005); 
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, bias); 
    vec3 result = ambient + (1.0 - shadow) * (diffuse) + specular;

    if(modelFlag == 0){
        return result;
    }

    //calc light dilution
    float ratio = 1.0;
    vec3 I = normalize(fs_in.FragPos - cameraPos);
    if (normalFlag == 1.0f){
        vec3 fragPos = fs_in.TBN * fs_in.FragPos;
        I = normalize(fragPos - cameraPos);
    }
    vec3 R = refract(I, normal, ratio);

    areaOfDilution = (dot(lightDir,normal) + (diluteAreaVariable - 1))/diluteAreaVariable;
    vec3 cangianteColor = result + areaOfDilution * light.cangiante;
    result = light.dilution * areaOfDilution *(cangianteColor - texture(skybox, R).rgb) + cangianteColor;


    //implement density control
    if(density < 0.5){
        for(int i=0; i < floor(3 - density * 4); i++){
            result *= result;
        }
    }
    else{
        result = (density - 0.5) * (texture(skybox, R).rgb - result) + result;
    }
    
    return result;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords)
{
    float areaOfDilution;

    vec3 lightDir = normalize(light.position - fragPos);
    if(normalFlag == 1.0f){
            lightDir = fs_in.TBN * normalize(light.position - fs_in.FragPos);
    }
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, texCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    vec3 result = ambient + diffuse + specular;

    if(modelFlag == 0){
        return result;
    }

    //calc light dilution
    float ratio = 1.0;
    vec3 I = normalize(fs_in.FragPos - cameraPos);
    if (normalFlag == 1.0f){
        vec3 fragPos = fs_in.TBN * fs_in.FragPos;
        I = normalize(fragPos - cameraPos);
    }
    vec3 R = refract(I, normal, ratio);

    areaOfDilution = (dot(lightDir,normal) + (diluteAreaVariable - 1))/diluteAreaVariable;
    vec3 cangianteColor = result + areaOfDilution * light.cangiante;
    result = light.dilution * areaOfDilution *(cangianteColor - texture(skybox, R).rgb) + cangianteColor;

    //implement density control
    if(density < 0.5){
        for(int i=0; i < floor(3 - density * 4); i++){
            result *= result;
        }
    }
    else{
        result = (density - 0.5) * (texture(skybox, R).rgb - result) + result;
    }
    
    return result;
} 

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords)
{

    float areaOfDilution;

    // diffuse shading
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    if(normalFlag == 1.0f){
            lightDir = normalize(fs_in.TBN * (light.position - fs_in.FragPos));
    }
    float diff = max(dot(light.direction, normal), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, texCoords));
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005); 
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, bias); 
   
    
    // spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    ambient  *= intensity; 
    diffuse  *= intensity;
    specular *= intensity;
    
    // attenuation
    float distance    = length(light.position - fs_in.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation;   
    vec3 result = (ambient + (1.0 - shadow) * (diffuse) + specular);
    
    if(modelFlag == 0){
        return result;
    }

    //calc light dilution
    float ratio = 1.0;
    vec3 I = normalize(fs_in.FragPos - cameraPos);
    if (normalFlag == 1.0f){
        vec3 fragPos = fs_in.TBN * fs_in.FragPos;
        I = normalize(fragPos - cameraPos);
    }
    vec3 R = refract(I, normal, ratio);

    areaOfDilution = (dot(lightDir,normal) + (diluteAreaVariable - 1))/diluteAreaVariable;
    vec3 cangianteColor = result + areaOfDilution * light.cangiante;
    result = light.dilution * areaOfDilution *(cangianteColor - texture(skybox, R).rgb) + cangianteColor;

    //implement density control
    if(density < 0.5){
        for(int i=0; i < floor(3 - density * 4); i++){
            result *= result;
        }
    }
    else{
        result = (density - 0.5) * (texture(skybox, R).rgb - result) + result;
    }


    return (result);
} 

float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0; 
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir){
    // number of depth layers
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy * height_scale; 
    vec2 deltaTexCoords = P / numLayers;
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(texture_height, currentTexCoords).r;
  
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(texture_height, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }

        // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(texture_height, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords; 
}