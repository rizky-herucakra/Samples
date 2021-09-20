$input a_position, a_normal, a_texcoord0, i_data0, i_data1, i_data2, i_data3
$output v_normal, v_texcoord0, v_worldpos

#include "ravengine_shader.glsl"

void main()
{
    a_position.y += sin(u_time[0] * 10 + a_position.x * 5) / 20;

    vs_genmats();
    v_texcoord0 = a_texcoord0;
    vs_store();
}
