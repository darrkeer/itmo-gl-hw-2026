#version 330 core

uniform vec2 show_size;
uniform vec2 shift;
uniform vec2 resolution;
uniform int iterations;

out vec4 out_col;

const float PI = 3.14159265359;
const float ALPHA = PI * 2. / 5.;
const float K = (3. - sqrt(5)) / 2.;
const float H_TO_CENTER = (1. + sqrt(5)) / 2.;

vec2 rotate(vec2 p, float r) {
    float c = cos(r);
    float s = sin(r);
    return vec2(p.x * c - p.y * s, p.x * s + p.y * c);
}

vec2 rotate_from(vec2 start, vec2 p, float r) {
    vec2 tmp = p - start;
    return rotate(tmp, r) + start;
}

vec2 get_center(vec2 a, vec2 b, vec2 c, vec2 d, vec2 e) {
    return (a + b + c + d + e) / 5.;
}

vec2 get_center(vec2[5] pentagon) {
    return get_center(pentagon[0], pentagon[1], pentagon[2], pentagon[3], pentagon[4]);
}

vec2 get_next_pentagon_spawn_point(vec2 center, vec2 top_point) {
    float h = top_point.y - center.y; // x coords are the same
    return top_point - vec2(0, h * K);
}

float cross2(vec2 a, vec2 b) {
    return a.x * b.y - a.y * b.x;
}

bool inside_triangle(vec2 a, vec2 b, vec2 c, vec2 p) {
    float d1 = cross2(b - a, p - a);
    float d2 = cross2(c - b, p - b);
    float d3 = cross2(a - c, p - c);

    bool hasNeg = (d1 < 0.0) || (d2 < 0.0) || (d3 < 0.0);
    bool hasPos = (d1 > 0.0) || (d2 > 0.0) || (d3 > 0.0);

    return !(hasNeg && hasPos);
}

bool inside_pentagon(vec2 a, vec2 b, vec2 c, vec2 d, vec2 e, vec2 p) {
    return inside_triangle(a, b, c, p) 
        || inside_triangle(a, c, d, p)
        || inside_triangle(a, d, e, p);
}

bool inside_pentagon(vec2[5] pentagon, vec2 p) {
    return inside_pentagon(pentagon[0], pentagon[1], pentagon[2], pentagon[3], pentagon[4], p);
}

vec2[5] get_pentagon(vec2 center, vec2 a, int shift) {
    vec2 b = rotate_from(center, a, ALPHA);
    vec2 c = rotate_from(center, a, 2 * ALPHA);
    vec2 d = rotate_from(center, a, 3 * ALPHA);
    vec2 e = rotate_from(center, a, 4 * ALPHA);

    vec2 points[5] = vec2[5](a, b, c, d, e);
    return vec2[5](
        points[0 + shift],
        points[(1 + shift) % 5],
        points[(2 + shift) % 5],
        points[(3 + shift) % 5],
        points[(4 + shift) % 5]
    );
}

vec2[5] get_pentagon(vec2 center, vec2 a) {
    return get_pentagon(center, a, 0);
}

bool inside_fractal(vec2[5] pentagon, vec2 p) {
    for (int i=0; i<iterations; ++i) {
        vec2 a = pentagon[0];
        vec2 b = pentagon[1];
        vec2 c = pentagon[2];
        vec2 d = pentagon[3];
        vec2 e = pentagon[4];

        vec2 center = get_center(pentagon);
        vec2 center_a = get_next_pentagon_spawn_point(center, a);
        vec2 center_b = rotate_from(center, center_a, ALPHA);
        vec2 center_c = rotate_from(center, center_a, 2 * ALPHA);
        vec2 center_d = rotate_from(center, center_a, 3 * ALPHA);
        vec2 center_e = rotate_from(center, center_a, 4 * ALPHA);
        
        vec2[5] pentagon_a = get_pentagon(center_a, a);
        vec2[5] pentagon_b = get_pentagon(center_b, b, 4);
        vec2[5] pentagon_c = get_pentagon(center_c, c, 3);
        vec2[5] pentagon_d = get_pentagon(center_d, d, 2);
        vec2[5] pentagon_e = get_pentagon(center_e, e, 1);

        // middle
        float delta = length(a - center) * K;
        vec2 middle_point = center + normalize(a - center) * (-delta);
        vec2[5] pentagon_m = get_pentagon(center, middle_point);

        if (inside_pentagon(pentagon_a, p)) {
            pentagon = pentagon_a;
        } else if (inside_pentagon(pentagon_b, p)) {
            pentagon = pentagon_b;
        } else if (inside_pentagon(pentagon_c, p)) {
            pentagon = pentagon_c;
        } else if (inside_pentagon(pentagon_d, p)) {
            pentagon = pentagon_d;
        } else if (inside_pentagon(pentagon_e, p)) {
            pentagon = pentagon_e;
        } else if (inside_pentagon(pentagon_m, p)) {
            pentagon = pentagon_m;
        } else {
            return false;
        }
    }
    return true;
}

void main() {
    vec3 col_a = vec3(1.0, 0.0, 0.0);
    vec3 col_b = vec3(0, 0, 0);

    vec2 uv = gl_FragCoord.xy * show_size / resolution + shift;

    vec2 p[5] = vec2[5](
        vec2(0, 5),
        rotate(vec2(0, 5), ALPHA),
        rotate(vec2(0, 5), 2 * ALPHA),
        rotate(vec2(0, 5), 3 * ALPHA),
        rotate(vec2(0, 5), 4 * ALPHA)
    );

    if (inside_fractal(p, uv)) {
        out_col = vec4(col_a, 1);
    } else {
        out_col = vec4(col_b, 1);
    }
}
