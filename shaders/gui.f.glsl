varying vec2 f_texcoord;
varying vec4 f_color;
uniform sampler2D mytexture;

void main(void)
{
	gl_FragColor = texture2D(mytexture, f_texcoord) * f_color;
}

