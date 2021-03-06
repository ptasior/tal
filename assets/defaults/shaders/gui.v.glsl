attribute vec3 coord3d;
attribute vec2 texcoord;
uniform vec4 color;
uniform mat4 position;
uniform mat4 gui_mvp;
uniform int colorOnly;
varying vec2 f_texcoord;
varying vec4 f_color;
varying float f_colorOnly;

void main(void)
{
	gl_Position = gui_mvp * position * vec4(coord3d.x, coord3d.y, 0.0, 1.0);
	f_texcoord = texcoord;
	f_color = color;
	f_colorOnly = float(colorOnly);
}

