#version 330 core

uniform vec2 show_size;
uniform vec2 shift;
uniform vec3 params;
uniform float boldness;

out vec4 out_col;

float func(float x) {
        return params.x * x * x + params.y * x + params.z;
}

void main() {
        vec3 col_a = vec3(1.0, 0.0, 0.0);
        vec3 col_b = vec3(0, 0, 0);
        vec2 resolution = vec2(640.0, 480.0);

        vec2 uv = gl_FragCoord.xy * show_size / resolution + shift;

        if (abs(func(uv.x) - uv.y) < boldness) {
                out_col = vec4(col_a, 1.0);
        } else {
                out_col = vec4(col_b, 1.0);
        }
}
