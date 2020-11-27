precision mediump float;

out vec3 color;

in vec2 v_texCoords;

uniform sampler2D depthTexture;

void main()
{
    color = vec3(texture(depthTexture, v_texCoords).r);
}