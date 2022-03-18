#version 330 core
uniform  sampler2D  diffuseTex;

uniform vec3 camera_position;

uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;


in Vertex    {
	vec3 world_position;
	vec4 colour;
	vec3 normal;
	vec2 texCoord;
 } IN;

out  vec4  fragColor;

void  main() {
	vec3 incident = normalize(lightPos - IN.world_position);
	vec3 view_direction = normalize(camera_position - IN.world_position);
	vec3 half_direction = normalize(incident + view_direction);

	vec4 diffuse = texture(diffuseTex, IN.texCoord);
	if (diffuse.a < 0.1)
		discard;

	float lambert = max(dot(incident, IN.normal), 0.0);
	float dis = length(lightPos - IN.world_position);
	float attenuation = 1.0 - clamp(dis / lightRadius, 0.0, 1.0);

	float spec_factor = clamp(dot(half_direction, IN.normal), 0.0, 1.0);
	spec_factor = pow(spec_factor, 60.0);

	vec3 surface = (diffuse.rbg * lightColour.rbg);
	fragColor.rbg = surface * lambert * attenuation;
	fragColor.rbg += (lightColour.rbg * spec_factor) * attenuation * 0.2;
	fragColor.rbg += surface * 0.3;
}