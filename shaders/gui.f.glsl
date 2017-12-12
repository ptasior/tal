varying vec2 f_texcoord;
varying vec4 f_color;
varying float f_colorOnly;
uniform sampler2D mytexture;

void main(void)
{
	if(f_colorOnly == 1.0)
		gl_FragColor = f_color;
	else
		gl_FragColor = texture2D(mytexture, f_texcoord) * f_color;
}

