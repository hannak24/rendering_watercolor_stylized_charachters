#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform sampler2D paper;
uniform bool bloom;
uniform float exposure = 1.0;
uniform float bleed = 1.0;

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
        bloomColor = mix(bloomColor, texture(paper, TexCoords).rgb, 0.2);
        hdrColor += bleed * bloomColor; // additive blending
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
    // also gamma correct while we're at it       
    //result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}