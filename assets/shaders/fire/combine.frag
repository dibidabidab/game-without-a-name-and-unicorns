precision mediump float;
precision lowp sampler2D;

layout(location = 0) out uint indexedColor;

uniform sampler2D particleMap;
uniform float time;
uniform mat4 inverseProjection;

in vec2 v_texCoords;

vec2 hash(vec2 p)
{
    p = vec2(dot(p, vec2(376., 238.)), dot(p, vec2(324., 983.)));
    return -1. + 2. * vec2(fract(sin(p) * 365.));
}

float noise(in vec2 p)
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

    vec2 i = floor(p + (p.x + p.y) * K1);

    vec2 a = p - i + (i.x + i.y) * K2;
    vec2 o = (a.x > a.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec2 b = a - o + K2;
    vec2 c = a - 1.0 + 2.0 * K2;

    vec3 h = max(0.5 - vec3(dot(a, a), dot(b, b), dot(c, c)), 0.0);

    vec3 n = h * h * h * h * vec3(dot(a, hash(i)), dot(b, hash(i + o)), dot(c, hash(i + 1.)));

    return dot(n, vec3(80.));
}

float fireNoise(vec2 coords)
{
    coords.y -= time * 2.;
    float f = 0.5 * noise(coords);
    coords *= 2.;
    coords.y -= time * 1.;
    f += 0.5 * noise(coords);
    coords *= 2.;
    coords.x += time * 5.;
    coords.y -= time * .5;
    f += 0.25 * noise(coords);
    f = 0.5 + 0.5 * f;
    return f;
}

void main()
{
    float fireIntensity = texture(particleMap, v_texCoords).r;

    vec2 uv = v_texCoords;
    uv = (inverseProjection * vec4(uv * 2., 0, 1)).xy;

    fireIntensity *= .3 + pow(fireNoise(vec2(uv.x * .1, uv.y * .04)), 2.) * .7;

    indexedColor = fireIntensity > .2 ? ((fireIntensity > .45 && fireIntensity < .7) ? 17u : 16u) : 0u;
    if (indexedColor == 0u)
        discard;
}
