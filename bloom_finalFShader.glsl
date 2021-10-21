#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
in vec3 viewDir1;


uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform sampler2D paper;
uniform sampler2D texture_height;


uniform bool bloom;
uniform float exposure = 1.0;
uniform float bleed = 1.0;
uniform float darkEdge = 1.0;
uniform float density = 0.8942;
uniform float granulation = 0.22;
uniform float finalTremor = 0.0125;

uniform float height_scale;
const float levels = 7.0;

float max(vec3 v);
vec3 powV(vec3 v, float coef);
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

void main()
{             
    const float gamma = 2.2;
    vec3 viewDir   = normalize(TangentViewPos - TangentFragPos);
    //vec3 viewDir   = viewDir1;
    //vec2 texCoords = ParallaxMapping(TexCoords,  viewDir);
    //vec2 texCoords = TexCoords;
    vec2 texCoords = TexCoords + (texture(texture_height, TexCoords).rg - 0.4) * finalTremor;
    //if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
       //discard;
    vec3 hdrColor = texture(scene, texCoords).rgb;      
    vec3 bloomColor = texture(bloomBlur, texCoords).rgb;
    if(bloom)
        //hdrColor += 1.5 * bloomColor; // additive blending
        //hdrColor += 2 * bloomColor; // additive blending
        //bloomColor = pow(bloomColor, vec3(1.0 / gamma));
        //hdrColor = bloomColor;
        hdrColor = bleed * (bloomColor - hdrColor) + hdrColor;
        //bloomColor = mix(bloomColor, texture(paper, TexCoords).rgb, bleed * 0.2);
        //hdrColor += bleed * bloomColor; // additive blending
        //if ((hdrColor.r < bloomColor.r) & (hdrColor.g < bloomColor.g) & (hdrColor.b < bloomColor.b))
        //if (hdrColor.r < 0.2){
            //if (hdrColor.g < 0.2){
                //if (hdrColor.b < 0.2){
                   //hdrColor = bloomColor;
               // }
           // } 
        //}
           
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    //vec3 result = hdrColor;
    //result = powV(result,1+darkEdge * max(bloomColor));
    //result = result * (result - (1-texture(paper, TexCoords).rgb)) + powV((vec3(1.0)-result)* result,1 + (granulation * density)); 
    float W = 0.6;
    float P = 0.1;
    //result = (1-W) + W * result - P * texture(paper, TexCoords).rgb;
    result = mix(result,texture(paper, TexCoords).rgb,granulation);
    //result = texture(texture_height, TexCoords).rgb;
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(0.0,0.0,0.0,0.0);
}

float max(vec3 v){
    float max = v.r;
    if(v.g > v.r){
        max = v.g;
        if(v.b > max)
            max = v.b;
    }
    else{
        if(v.b > max)
            max = v.b;
    }
    return max;
}

vec3 powV(vec3 v, float coef){
    vec3 vec;
    vec.x = pow(v.r, coef);
    vec.y = pow(v.g, coef);
    vec.z = pow(v.b, coef);
    return vec;
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
  
    //while(currentLayerDepth < currentDepthMapValue)
    //{
        // shift texture coordinates along direction of P
        //currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        //currentDepthMapValue = texture(texture_height, currentTexCoords).r;  
        // get depth of next layer
        //currentLayerDepth += layerDepth;  
    //}

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