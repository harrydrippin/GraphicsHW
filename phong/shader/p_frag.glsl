uniform mat4 u_view_matrix;

uniform vec3 u_light_vector;
uniform vec4 u_light_ambient;
uniform vec4 u_light_diffuse;
uniform vec4 u_light_specular;

uniform vec4 u_material_ambient;
uniform vec4 u_material_diffuse;
uniform vec4 u_material_specular;
uniform float u_material_shininess;

varying vec3 v_vertex;
varying vec3 v_normal;

vec4 phong() {
    vec4 color = vec4(0);

    vec3 n = normalize(v_normal);
    vec3 l = normalize(u_light_vector);

    vec3 v_pos = vec3(u_view_matrix * vec4(0, 0, 0, 1));
    vec3 v = normalize(v_pos - v_vertex);
    vec3 r = reflect(-l, n);

    vec3 h = normalize(l + v);

    color += u_light_ambient * u_material_ambient;
    color += max(dot(l, n), 0.0) * u_light_diffuse * u_material_diffuse;
    color += pow(max(dot(r, v), 0.0), u_material_shininess) * u_light_specular * u_material_specular;

    return color;
}

void main() {
    gl_FragColor = phong();
}