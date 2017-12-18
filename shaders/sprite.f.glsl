varying vec2 f_texcoord;
uniform sampler2D mytexture;

void main(void)
{
	vec4 col = texture2D(mytexture, f_texcoord);
	if(col.a == 0.0f)
		discard;
	gl_FragColor = col;
}

