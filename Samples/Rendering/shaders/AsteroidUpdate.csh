#include "AsteroidShared.glsl"
#include "RavEngine/quat.glsl"

layout(push_constant, std430) uniform UniformBufferObject{
    float fpsScale;
} ubo;

const float kGraivty = 0.5 / 60;
const float scaleDelta = 0.005;

void update(inout ParticleData data, inout float newLife, uint particleID)
{    
    newLife += ubo.fpsScale;

    data.velocity.y -= kGraivty * ubo.fpsScale;

    data.scale -= scaleDelta * ubo.fpsScale;

    data.pos += data.velocity; 
    data.rot = quatAdd(data.rot, eulerToQuat(degToRad(vec3(5,5,5)))); 

    if (newLife > 300)
    {
        // destroy the particle

        newLife = 0;
    }
}