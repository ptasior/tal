attribute vec3 coord3d;
attribute vec2 texcoord;
varying vec2 f_texcoord;
uniform mat4 position;
uniform mat4 mvp;

void main(void)
{
	mat4 bb = transpose(mvp);
	bb[0][3] = position[0][3];
	bb[1][3] = position[1][3];
	bb[2][3] = position[2][3];
	bb[3] = vec4(0,0,0,1);

	gl_Position = mvp * position * bb* vec4(coord3d, 1.0);
	f_texcoord = texcoord;
}

