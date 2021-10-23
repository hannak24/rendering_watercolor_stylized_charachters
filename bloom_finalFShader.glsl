#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

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


float max(vec3 v);
vec3 powV(vec3 v, float coef);
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);
vec3 calcSobelEdgeMap(vec2 texCoords);

void main()
{             
    vec2 texCoords = TexCoords + (texture(texture_height, TexCoords).rg - 0.4) * finalTremor;
    vec3 hdrColor = texture(scene, texCoords).rgb;      
    vec3 bloomColor = texture(bloomBlur, texCoords).rgb;
    if(bloom)
        hdrColor = bleed * (bloomColor - hdrColor) + hdrColor;           
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    vec3 Sobel_edge_map = calcSobelEdgeMap(texCoords);
    result = (1-(1-granulation-darkEdge)) + (1-granulation-darkEdge) * result - granulation * texture(paper, TexCoords).rgb - Sobel_edge_map * darkEdge;
    FragColor = vec4(result, 1.0);
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


vec3 calcSobelEdgeMap(vec2 texCoords){
    vec2 offset = 1.0 / textureSize(scene, 0);
    vec2 coord = TexCoords;
    vec3 color00 = texture(scene,vec2(texCoords.x-offset.x,texCoords.y-offset.x)).rgb;
    vec3 color01 = texture(scene,vec2(texCoords.x,texCoords.y-offset.x)).rgb;
    vec3 color02 = texture(scene,vec2(texCoords.x+offset.x,texCoords.y-offset.x)).rgb;
    vec3 color10 = texture(scene,vec2(texCoords.x-offset.x,texCoords.y)).rgb;
    vec3 color11 = texture(scene,vec2(texCoords.x,texCoords.y)).rgb;
    vec3 color12 = texture(scene,vec2(texCoords.x+offset.x,texCoords.y)).rgb;
    vec3 color20 = texture(scene,vec2(texCoords.x-offset.x,texCoords.y+offset.x)).rgb;
    vec3 color21 = texture(scene,vec2(texCoords.x,texCoords.y+offset.x)).rgb;
    vec3 color22 = texture(scene,vec2(texCoords.x+offset.x,texCoords.y+offset.x)).rgb;

    vec3 Sobel_x = (-1)*color00+(-2)*color01+(-1)*color02 +(1)*color20 + (2)*color21 + (1)*color22;
    vec3 Sobel_y = (-1)*color00+(-2)*color10+(-1)*color20 + (1)*color02 + (2)*color12 + (1)*color22;
    vec3 Sobel_edge_map = abs(Sobel_x)+abs(Sobel_y);
    return Sobel_edge_map;
    //return color11;
}