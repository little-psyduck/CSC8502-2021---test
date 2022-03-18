#version 330 core

uniform mat4 modelMatrix; 
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec2 texCoord;

out Vertex {
	vec3 world_position;
	vec4 colour;
	vec3 normal;
	vec2 texCoord;
} OUT;

void main() {
	OUT.colour = colour;
	OUT.texCoord = texCoord;

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	OUT.normal = normalize(normalMatrix * normalize(normal));
	
	vec4 world_pos = modelMatrix * vec4(position,1);
	OUT.world_position = world_pos.xyz;

	gl_Position = (projMatrix * viewMatrix) * world_pos;
}