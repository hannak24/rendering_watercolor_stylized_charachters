#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BlurColor;

in vec3 v_texCoord3D;
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    mat3 TBN;
} fs_in;

#define NR_POINT_LIGHTS 4

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float cangiante;
    float dilution;
};

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


uniform sampler2D diffuseTexture;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;
uniform sampler2D texture_height;
uniform sampler2D texture_noise;
uniform sampler2D shadowMap;
uniform samplerCube skybox;
uniform float diluteAreaVariable;
uniform float cangiante;
uniform float dilution;
uniform float density;
uniform float turbulance;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 cameraPos;
uniform float height_scale;
uniform float time;
uniform bool water;
uniform float wallFlag = 0.0; 
uniform Material material;
uniform DirLight dirLight;
uniform float normalFlag;
uniform float parralaxFlag;
uniform float modelFlag;
uniform float toonShading;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

const float levels = 7.0;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec2 texCoords);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords);
vec3 CalcSpotLight (SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords);
float ShadowCalculation(vec4 fragPosLightSpace, float bias);
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);
float snoise(vec3 v);
float noise(vec3 p);
float computeNoise(vec3 uvw);

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

    // implement turbulance

    // Perturb the texcoords with three components of noise
    vec3 uvw = v_texCoord3D + 0.1*vec3(snoise(v_texCoord3D + vec3(0.0, 0.0, time)),
    snoise(v_texCoord3D + vec3(43.0, 17.0, time)),
	snoise(v_texCoord3D + vec3(-17.0, -43.0, time)));
    float n = computeNoise(uvw);
    if(modelFlag == 0){
        if(water)
            FragColor = vec4(0.4 * result + 0.2 * vec3(n, n, n)+ 0.6 * vec3(0.2824,0.749,0.7686), 1.0);
        else
            FragColor = vec4(result, 1.0);
    }
    else{
        FragColor = vec4(result, 1.0);
    }
    BlurColor = vec4(result, 1.0);
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
    vec3 ambient, diffuse, specular;
    if(modelFlag == 0){
        if(wallFlag == 0.0){
            ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, texCoords));
            diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, texCoords));
            specular = light.specular * spec * vec3(texture(material.texture_diffuse1, texCoords));
        }
        else{
            ambient  = 0.2 * texture(material.texture_diffuse1, texCoords).rgb;
            diffuse  = 0.7 * texture(material.texture_diffuse1, texCoords).rgb;
            specular = 0.1 * texture(material.texture_diffuse1, texCoords).rgb;
        }
    }
    else{
        ambient  = light.ambient  * vec3(mix(texture(texture_specular, texCoords),texture(material.texture_diffuse1, texCoords),0.65));
        diffuse  = light.diffuse  * diff * vec3(mix(texture(texture_specular, texCoords),texture(material.texture_diffuse1, texCoords),0.65));
        specular = light.specular * spec * vec3(mix(texture(material.texture_specular1, texCoords),texture(material.texture_specular2, texCoords),0.65));
    }

    //apply toon shading if required
    if(toonShading == 1){
        float nDot1 = normal.x * lightDir.x + normal.y * lightDir.y + normal.z * lightDir.z;
        float brightness = max(nDot1,0.0);
        float level = floor(brightness * levels);
        brightness = level / levels;
   
       // combine results
        ambient = 0.45 *(light.ambient * brightness) + 0.6*light.ambient * vec3(texture(material.texture_diffuse1, texCoords));
        diffuse = 0.45 *(light.diffuse *  brightness) + 0.6*light.diffuse *diff * vec3(texture(material.texture_diffuse1, texCoords));
        specular = 0.45 *(light.specular * brightness) + 0.6*light.specular * spec *vec3(texture(material.texture_specular1, texCoords));
    }

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005); 
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, bias); 
    vec3 result = ambient + (1.0 - shadow) * (diffuse) + specular;
    if(modelFlag == 0){
        return result;
    }
    result = mix(texture(texture_specular, texCoords).rgb,result ,turbulance);

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
    //result = light.dilution * areaOfDilution *(cangianteColor - texture(skybox, R).rgb) + cangianteColor; //if skybox applied
    result = light.dilution * areaOfDilution *(cangianteColor - vec3(1.0f, 1.0f, 1.0f)) + cangianteColor;
    
    //implement density control
    if(density < 0.5){
        float power = 3 - density * 4;
        result.x *= pow(result.x,power);
        result.y *= pow(result.y,power);
        result.z *= pow(result.z,power);
    }
    else{
        //result = (density - 0.5) * (texture(skybox, R).rgb - result) + result; //if skybox applied
        result = (density - 0.5) * (vec3(1.0f, 1.0f, 1.0f) - result) + result;
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

    //apply toon shading if needed
    if(toonShading == 1){
        float nDot1 = normal.x * lightDir.x + normal.y * lightDir.y + normal.z * lightDir.z;
        float brightness = max(nDot1,0.0);
        float level = floor(brightness * levels);
        brightness = level / levels;
   
       // combine results
        ambient = 0.45 *(light.ambient * brightness) + 0.6*light.ambient * vec3(texture(material.texture_diffuse1, texCoords));
        diffuse = 0.45 *(light.diffuse *  brightness) + 0.6*light.diffuse *diff * vec3(texture(material.texture_diffuse1, texCoords));
        specular = 0.45 *(light.specular * brightness) + 0.6*light.specular * spec *vec3(texture(material.texture_specular1, texCoords));

        result = ambient + diffuse + specular;
        result = mix(texture(texture_specular, texCoords).rgb,result ,turbulance);
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
    //result = light.dilution * areaOfDilution *(cangianteColor - texture(skybox, R).rgb) + cangianteColor; //if skybox relevant
    result = light.dilution * areaOfDilution *(cangianteColor - vec3(1.0f, 1.0f, 1.0f)) + cangianteColor;

    //implement density control
    if(density < 0.5){
        for(int i=0; i < floor(3 - density * 4); i++){
            result *= result;
        }
    }
    else{
        //result = (density - 0.5) * (texture(skybox, R).rgb - result) + result; //if skybox is relevant
        result = (density - 0.5) * (vec3(1.0f, 1.0, 1.0f) - result) + result;
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

    //apply toon shaiding if required
    if(toonShading == 1){
    float nDot1 = normal.x * lightDir.x + normal.y * lightDir.y + normal.z * lightDir.z;
    float brightness = max(nDot1,0.0);
    float level = floor(brightness * levels);
    brightness = level / levels;
   
    // combine results
    ambient = 0.4 *(light.ambient * brightness) + 0.1 *vec3(mix(texture(texture_specular, texCoords),texture(material.texture_diffuse1, texCoords),0.65));
    diffuse = 0.4 *(light.diffuse *  brightness) * diff + 0.1 *vec3(mix(texture(texture_specular, texCoords),texture(material.texture_diffuse1, texCoords),0.65));
    specular = 0.4 *(light.specular * brightness) * spec + 0.1 *vec3(mix(texture(texture_specular, texCoords),texture(material.texture_specular1, texCoords),0.65));
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
    //result = light.dilution * areaOfDilution *(cangianteColor - texture(skybox, R).rgb) + cangianteColor; //if skybox required
    result = light.dilution * areaOfDilution *(cangianteColor - vec3(1.0f, 1.0f, 1.0f)) + cangianteColor;

    //implement density control
    if(density < 0.5){
        for(int i=0; i < floor(3 - density * 4); i++){
            result *= result;
        }
    }
    else{
        //result = (density - 0.5) * (texture(skybox, R).rgb - result) + result; //if skybox is required
        result = (density - 0.5) * (vec3(1.0f, 1.0, 1.0f) - result) + result;
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


vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 mod30(vec3 x) {
  return x - floor(x * (1.0 / 30.0)) * 30.0;
}

vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod30(vec4 x) {
  return x - floor(x * (1.0 / 30.0)) * 30.0;
}

vec4 permute(vec4 x) {
     return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v)
  { 
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //   x0 = x0 - 0.0 + 0.0 * C.xxx;
  //   x1 = x0 - i1  + 1.0 * C.xxx;
  //   x2 = x0 - i2  + 2.0 * C.xxx;
  //   x3 = x0 - 1.0 + 3.0 * C.xxx;
  vec3 x1 = x0 - i1 + C.xxx;
  vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
  vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

// Permutations
  //i = mod289(i); 
  i = mod289(i);
  vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients: 7x7 points over a square, mapped onto an octahedron.
// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
  float n_ = 0.142857142857; // 1.0/7.0
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
  //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.5 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 105.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
  }


  float computeNoise(vec3 uvw){
    // Six components of noise in a fractal sum
    float n = snoise(uvw - vec3(0.0, 0.0, time));
    n += 0.5 * snoise(uvw * 2.0 - vec3(0.0, 0.0, time*1.4)); 
    n += 0.25 * snoise(uvw * 4.0 - vec3(0.0, 0.0, time*2.0)); 
    n += 0.125 * snoise(uvw * 8.0 - vec3(0.0, 0.0, time*2.8)); 
    n += 0.0625 * snoise(uvw * 16.0 - vec3(0.0, 0.0, time*4.0)); 
    float s = 0.0625 * snoise(uvw * 16.0 - vec3(0.0, 0.0, 4.0));
    n += 0.03125 * snoise(uvw * 32.0 - vec3(0.0, 0.0, time*5.6)); 
    n = n * 0.7;
    return n;
  }
