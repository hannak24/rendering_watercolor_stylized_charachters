#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform sampler2D paper;
uniform bool bloom;
uniform float exposure = 1.0;
uniform float bleed = 1.0;
uniform float darkEdge = 1.0;

float max(vec3 v);
vec3 powV(vec3 v, float coef);

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, TexCoords).rgb;      
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
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
    //vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    vec3 result = hdrColor;
    result = powV(result,1+darkEdge * max(bloomColor));

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