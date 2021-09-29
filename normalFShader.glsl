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
float cnoise(vec3 P);


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
        result += 0.05 * CalcPointLight(pointLights[i], normal, fs_in.FragPos, viewDir, texCoords);
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

    //calc perlin noise for turbulance
    //vec3 offset = vec3(cnoise(fs_in.FragPos));
    //vec3 offset = vec3(cnoise(result));
    //result = result + 2 * abs(offset);

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
        float power = 3 - density * 4;
        result.x *= pow(result.x,power);
        result.y *= pow(result.y,power);
        result.z *= pow(result.z,power);
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


//	Classic Perlin 3D Noise 
//	by Stefan Gustavson
//
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}
vec3 fade(vec3 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}

float cnoise(vec3 P){
  vec3 Pi0 = floor(P); // Integer part for indexing
  vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
  Pi0 = mod(Pi0, 289.0);
  Pi1 = mod(Pi1, 289.0);
  vec3 Pf0 = fract(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.yy, Pi1.yy);
  vec4 iz0 = Pi0.zzzz;
  vec4 iz1 = Pi1.zzzz;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 / 7.0;
  vec4 gy0 = fract(floor(gx0) / 7.0) - 0.5;
  gx0 = fract(gx0);
  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, vec4(0.0));
  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 / 7.0;
  vec4 gy1 = fract(floor(gx1) / 7.0) - 0.5;
  gx1 = fract(gx1);
  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, vec4(0.0));
  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x); 
  return 2.2 * n_xyz;
}