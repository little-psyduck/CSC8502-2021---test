#version 330 core

uniform sampler2D diffuseTex_ground;
uniform sampler2D bumpTex_ground;

uniform sampler2D diffuseTex_weed;
uniform sampler2D bumpTex_weed;
//
uniform vec3 camera_position;

uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;

in Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 world_position;
} IN;

out vec4 frag_color;

void main() {
	vec3 incident = normalize(lightPos - IN.world_position);
	vec3 view_direction = normalize(camera_position - IN.world_position);
	vec3 half_direction = normalize(incident + view_direction);

	mat3 TBN = mat3(normalize(IN.tangent),normalize(IN.binormal),normalize(IN.normal));

	vec4 diffuse;
	vec3 bump_normal;

	if (IN.world_position.y < 150.0f) {
		diffuse = texture(diffuseTex_ground, IN.texCoord);
		bump_normal = texture(bumpTex_ground, IN.texCoord).rgb;
	}
	else if (IN.world_position.y >= 150.0f && IN.world_position.y <= 180.0f) {
		diffuse = mix(texture(diffuseTex_weed, IN.texCoord),
			texture(diffuseTex_ground, IN.texCoord), 0.25f);

		bump_normal = mix(texture(bumpTex_weed, IN.texCoord),
			texture(bumpTex_ground, IN.texCoord), 0.5f).rgb;
	}
	else if (IN.world_position.y > 180.0f) {
		diffuse = texture(diffuseTex_weed, IN.texCoord);
		bump_normal = texture(bumpTex_weed, IN.texCoord).rgb;
	}

	bump_normal   = normalize(TBN * normalize(bump_normal * 2.0 - 1.0));

	float lambert = max(dot(incident, bump_normal),0.0);
	float dis = length(lightPos - IN.world_position);
	float attenuation = 1.0 - clamp(dis / lightRadius, 0.0, 1.0);

	float spec_factor = clamp(dot(half_direction, bump_normal), 0.0, 1.0);
	spec_factor = pow(spec_factor, 60.0);

	vec3 surface = (diffuse.rgb * lightColour.rgb);
	frag_color.rgb = surface * lambert * attenuation;
	frag_color.rgb += (lightColour.rbg * spec_factor) * attenuation * 0.2;
	frag_color.rgb += surface * 0.3;
}