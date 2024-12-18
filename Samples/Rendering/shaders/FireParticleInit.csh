#include "sprite_particle_layout_transparent.glsl"

#include "utility.glsl"

ParticleData init(ParticleInitData initData)
{  
    uint particleID = initData.particleID;

    ParticleData data;
    data.scale = vec2(0.1,1);
    vec4 pos = vec4(
        rand(vec2(particleID,particleID)),
        rand(vec2(particleID * 2,particleID * 2)),
        rand(vec2(particleID / 2,particleID / 2)),
        1
    );

    pos = initData.emitterModel * pos;

    data.pos = pos.xyz;

    data.animationFrame = 0;

    data.alpha = 1;

    return data;
}