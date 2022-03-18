#version 330 core
uniform  sampler2D  diffuseTex;

in  Vertex    {
	vec2  texCoord;
 } IN;

out  vec4  fragColor;
void  main()   {     
fragColor   = texture(diffuseTex , IN.texCoord );
} 