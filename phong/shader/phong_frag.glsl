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

vec4 ceil_color(vec4 c, float lv) {
    float d = 1.0 / lv;
    return vec4(ceil(c.r * d), ceil(c.g * d), ceil(c.b * d), c.a) / d;
}

vec4 directional_light() {
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

    // diffuse = ceil_color(diffuse, 0.334);
    // specular = ceil_color(specular, 0.334);

    color = ambient + (diffuse + specular);// * (2.0);

    return color;
}

void main() {
    vec4 color = directional_light();

    float level = 3.0;



    gl_FragColor = vec4(color.rgb, 1.0);
}