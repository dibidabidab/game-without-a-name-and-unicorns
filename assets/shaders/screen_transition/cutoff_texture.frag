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
}
