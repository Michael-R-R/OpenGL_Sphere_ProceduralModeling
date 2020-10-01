#version 430

in vec2 tc;
in vec2 tc2;

out vec4 color;
out vec4 color2;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

layout(binding = 0) uniform sampler2D samp;

void main(void)
{ 
	color = texture(samp, tc);
}