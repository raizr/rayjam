#version 100
precision mediump float;
// Input vertex attributes (from vertex shader)
varying vec2 fragTexCoord;
varying vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform float direction;
uniform float seconds;
uniform vec2 size;

float rand (in vec2 uv) {
    return fract(sin(dot(uv,vec2(12.4124,48.4124))) * 48512.41241);
}

const vec2 O = vec2(0., 1.);

float noise (in vec2 uv) {
	vec2 b = floor(uv);
	return mix(mix(rand(b),rand(b+O.yx),.5),mix(rand(b+O),rand(b+O.yy),.5),.5);
}

#define DIR_RIGHT -1.
#define DIR_LEFT 1.
#define DIRECTION DIR_LEFT

#define LAYERS 8
#define SPEED 0.1
#define SIZE 5.

void main() {
    vec2 uv;
    uv.x = fragTexCoord.x / SIZE;
    uv.y = fragTexCoord.y / SIZE;
    float stars = 0.;
    float fl, s;
    for (int layer = 0; layer < LAYERS; layer++) {
        fl = float(layer);
        s = (400. - fl * 20.);
        stars += step(.1, pow(noise(mod(vec2(uv.x*s + seconds * SPEED - fl * 100., uv.y * s + + seconds * SPEED - fl * 100.), size.x)), 18.)) * (fl / float(LAYERS));
    }
    vec3 vColor = vec3( 0.1, 0.2, 0.4 ) *  clamp(fragTexCoord.y, -0.5, 0.5);
    gl_FragColor = vec4(vColor + vec3(stars), 1.0 );
}
