#version 330 core

uniform sampler2D water_diffuseTex;
uniform sampler2D water_bumpTex;

uniform float isDepth;

in Vertex	{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;

	float depth;
} IN;

out vec4 frag_color[2];

void main(){
	mat3 TBN = mat3(normalize(IN.tangent),normalize(IN.binormal),normalize(IN.normal));

	vec4 diffuse = texture(water_diffuseTex, IN.texCoord);
	vec3 bump_normal = texture(water_bumpTex,IN.texCoord).rgb;
	bump_normal   = normalize(TBN * normalize(bump_normal * 2.0 - 1.0));
	
	frag_color[0] = diffuse;
	frag_color[1] = vec4(bump_normal.xyz * 0.5 + 0.5, 1.0);

	frag_color[0].a = 1.0;
	frag_color[1].a = 1.0;

	if (isDepth == 1.0) 
		frag_color[0].rgb = vec3(IN.depth, 0, 0);
}