attribute vec3 coord3d;
attribute vec2 texcoord;
attribute vec3 vnorm;

varying vec2 f_texcoord;
varying vec3 f_vnorm;

uniform mat4 position;
uniform mat4 mvp;

void main(void)
{
	gl_Position = mvp * position * vec4(coord3d, 1.0);
	f_texcoord = texcoord;
	f_vnorm = vnorm;
}

