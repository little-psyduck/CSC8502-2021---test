#version 330 core

uniform sampler2D diffuseTex_ground;
uniform sampler2D bumpTex_ground;

uniform sampler2D diffuseTex_weed;
uniform sampler2D bumpTex_weed;

uniform float isDepth;

//

in Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 world_position;

	float depth;
} IN;

out vec4 frag_color[2];

void main() {

	mat3 TBN = mat3(normalize(IN.tangent),normalize(IN.binormal),normalize(IN.normal));

	vec4 diffuse;
	vec3 bump_normal;

	//
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
	//

	bump_normal   = normalize(TBN * normalize(bump_normal * 2.0 - 1.0));

	frag_color[0] = diffuse;
	frag_color[1] = vec4(bump_normal.xyz * 0.5 + 0.5, 1.0);

	frag_color[0].a = 1.0;
	frag_color[1].a = 1.0;

	if (isDepth == 1.0) 
		frag_color[0].rgb = vec3(IN.depth, 0, 0);
}