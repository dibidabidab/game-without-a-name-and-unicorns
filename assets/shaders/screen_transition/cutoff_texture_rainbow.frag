precision mediump float;

layout(location = 0) out uint color;

in vec2 v_texCoords;

uniform sampler2D transitionTexture;
uniform float time;

void main()
{
    if (texture(transitionTexture, v_texCoords).r >= time)
        discard;
    color = 1u;

    if (v_texCoords.x + 1. < time * 2.)
        return;

    float parabola = -pow(v_texCoords.x - .5, 2.) * 1.5 + .7;

    float thickness = .02 * min(1., time * 2.);

    float timeInt = 6. - float(int(max(0., -3.5 * (1. - time) -.5 - (v_texCoords.x * 3.5 * time) + time * 5.) * 6.));

    if (v_texCoords.y < parabola - thickness * timeInt)
        color = 1u;
    else if (v_texCoords.y < parabola - thickness * 5.)
        color = 15u;
    else if (v_texCoords.y < parabola - thickness * 4.)
        color = 14u;
    else if (v_texCoords.y < parabola - thickness * 3.)
        color = 13u;
    else if (v_texCoords.y < parabola - thickness * 2.)
        color = 12u;
    else if (v_texCoords.y < parabola - thickness)
        color = 11u;
    else if (v_texCoords.y < parabola)
        color = 10u;
}
