precision mediump float;

layout(location = 0) out float col;

in vec2 v_quad_pos;
flat in float v_age;

void main()
{

    col = max(1.f - distance(v_quad_pos, vec2(0)) - (v_age * v_age) * .1, 0.f) * .5;
}
