#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;

uniform sampler2D skyboxTex;
uniform sampler2D dt_terrain;

in Vertex{
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void) {
	vec3 diffuse_terrain = texture(diffuseTex, IN.texCoord).xyz;

	vec3 light = texture(diffuseLight, IN.texCoord).xyz;
	vec3 specular = texture(specularLight, IN.texCoord).xyz;

	fragColour.xyz = diffuse_terrain * 0.3; // ambient

	fragColour.xyz += light * 0.5; // lambert
	fragColour.xyz += specular * 0.2; // Specular

	fragColour.a = 1.0;

	vec3 depth_terrain = texture(dt_terrain, IN.texCoord).xyz;

	if (depth_terrain.r <= 0.2) {fragColour.rgb += texture(skyboxTex, IN.texCoord).xyz;}
}