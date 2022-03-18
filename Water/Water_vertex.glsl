#version 330 core

uniform mat4 water_modelMatrix;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 water_textureMatrix;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec4 tangent;
in vec2 texCoord;

out Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;

	float depth;
} OUT;

void main(){
	OUT.colour = colour;
	mat3 normalMatrix = transpose(inverse(mat3(water_modelMatrix)));

	vec3 wNormal = normalize(normalMatrix * normalize(normal));
	vec3 wTangent  =  normalize(normalMatrix * normalize(tangent.xyz));

	OUT.normal = wNormal;
	OUT.tangent = wTangent;
	OUT.binormal = cross(wNormal , wTangent) * tangent.w;

	OUT.texCoord = (water_textureMatrix * vec4(texCoord, 0, 1)).xy;

	vec4 world_pos = water_modelMatrix * vec4(position,1);
	OUT.worldPos = world_pos.xyz;

	gl_Position = projMatrix * viewMatrix * world_pos;

	OUT.depth = gl_Position.z / gl_Position.w;
}