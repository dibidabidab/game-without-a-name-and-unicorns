precision mediump float;

layout(location = 0) out float col;

in vec2 v_quad_pos;
flat in float v_age;

void main()
{

    col = max(1.f - distance(v_quad_pos * 10.f, vec2(0)) - (v_age * v_age) * -.5, 0.f) * .5;


    col += (1.f - distance(v_quad_pos * vec2(3.5, 1.), vec2(0))) * .01 * max(0., min(v_quad_pos.y * 5., 1.));

    col *= min(v_age * 14., 1.);
}
