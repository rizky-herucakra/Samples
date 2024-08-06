
layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec2 inUV;

layout(binding = 0) uniform sampler2D diffuseSampler; 

layout(push_constant) uniform UniformBufferObject{
    float time;
} ubo;

//  Function from Iñigo Quiles
//  https://www.shadertoy.com/view/MsS3Wc
vec3 hsb2rgb( in vec3 c ){
    vec3 rgb = clamp(abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),
                             6.0)-3.0)-1.0,
                     0.0,
                     1.0 );
    rgb = rgb*rgb*(3.0-2.0*rgb);
    return c.z * mix(vec3(1.0,1.0,1.0), rgb, c.y);
}

// step function w/ floor
float stepf(float value,float steps){
    return remap(floor(remap(value,0.0,1.0,0.0,steps)),0.0,steps,0.0,1.0);
}

// step function w/ ceiling 
float stepc(float value, float steps){
    return remap(ceil(remap(value,0.0,1.0,0.0,steps)),0.0,steps,0.0,1.0);
}

// used to control position on the rainbow
float hsbinfcufve(float x){
    return pow(x,1.5);
}

// used to control brightness
float b(float x){
    return 1.0 - pow(x,3.0);
}

// prevent wraparound
float clampInRange(float x){
    return ((x >= 0.0 && x <= 1.0) ? x : 0.0);
}

vec3 calcMain(vec2 uv){
    // adjust the number of stripes
    const float stripes = 6.0;

    // calculate where to sample 
    float coord = hsbinfcufve(stepf(1.0-uv.y,stripes));
    
    // calculate color & brightness
    return hsb2rgb(vec3(coord,1.0,b(coord)));
    
}

vec3 calcAlt(vec2 uv){
     // green, white, blue
     vec3 col1 = vec3(0.24,0.54,0.44);
     vec3 col2 = vec3(1.0,1.0,1.0);
     vec3 col3 = vec3(0.22,0.13,0.45);
     
     float diff = 0.4;
     
     const float steps = 2.0;

     // gradient + step function
     vec3 colorTop = mix(col1,col2,stepf(1.0-remap(uv.y,1.0-diff,1.0,0.0,1.0),steps));
     vec3 colorBot = mix(col2,col3,stepc(1.0-remap(uv.y,0.0,diff,0.0,1.0),steps));
     
     return uv.y < 0.5 ? colorBot : colorTop;
}

LitOutput frag()
{
    LitOutput fs_out;

    vec2 uv = 1 - inUV;  // model UVs are upside-down

    // "rounded square wave" blending function
    float factor = (atan(sin(3*ubo.time)/0.1))/3+0.5;
    
    vec3 resCol = mix(calcMain(uv),calcAlt(uv),factor);
  
    fs_out.color = vec4(resCol,1);
    fs_out.normal = inNormal;
    fs_out.roughness = 0.5;
	fs_out.specular = 0.2;
	fs_out.metallic = 0;
	fs_out.ao = 0;

    return fs_out;
}


