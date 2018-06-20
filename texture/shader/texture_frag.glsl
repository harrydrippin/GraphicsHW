uniform sampler2D u_texid;
uniform sampler2D u_texid2;

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
varying vec2 v_texcoord;

vec4 phong_shading(vec4 col) {
    vec4 color = vec4(0);

    vec3 world_vertex = v_vertex;
    vec3 world_normal = normalize(v_normal);

    vec3 light = normalize(u_light_vector);
    vec3 light_incident = -light;
    vec3 light_reflect = reflect(light_incident, world_normal);

    vec3 view_position = (u_view_matrix * vec4(0, 0, 0, 1)).xyz;
    vec3 view_direction = normalize(view_position - world_vertex);

    vec4 ambient = u_material_ambient * u_light_ambient;
    vec4 diffuse = max(dot(world_normal, light), 0.0) * u_material_diffuse * u_light_diffuse;
    vec4 specular = pow(max(dot(light_reflect, view_direction), 0.0), u_material_shininess) * u_material_specular * u_light_specular;

    color = ambient + (col * (diffuse + 0.5) + specular);

    return color;
}

void main() {
    float p = 25.0;
    // vec2 xy = v_texcoord;//vec2(floor(v_texcoord.x * p) / p, floor(v_texcoord.y * p) / p);

    // vec4 texcolor = texture2D(u_texid, xy) * 0.75 + texture2D(u_texid2, v_texcoord) * 0.25;
    vec4 color = phong_shading(vec4(0.5, 0.5, 0.5, 1.0));

    gl_FragColor = color;
}