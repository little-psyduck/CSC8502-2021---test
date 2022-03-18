#version 330 core

uniform  mat4  plants_model;
uniform  mat4  viewMatrix;
uniform  mat4  projMatrix;

in vec3 position;
in vec2 texCoord;
in vec3 normal;
in vec4 colour;

out Vertex {
	vec3 world_position;
	vec4 colour;	
	vec3 normal;
	vec2 texCoord;
}OUT;

void  main(void) {
	OUT.colour = colour;
	OUT.texCoord = texCoord;

	mat3 normalMatrix = transpose(inverse(mat3(plants_model)));
	OUT.normal = normalize(normalMatrix * normalize(normal));

	vec4 world_pos_temp = plants_model * vec4(position, 1);
	OUT.world_position = world_pos_temp.xyz;

	mat4 mvp = projMatrix * viewMatrix;
	gl_Position = mvp * world_pos_temp;
}