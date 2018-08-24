varying vec2 f_texcoord;
varying vec3 f_vnorm;
varying vec3 f_coords;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main(void)
{
	float cosTheta = clamp(dot(f_vnorm, vec3(1,1,1)), 0.5, 1.0);

	float mix = -2.0*f_coords.y;

	vec4 outCol;
	if(mix < 0.83)
		outCol = texture2D(texture1, f_texcoord);
	else if(mix > 0.88)
		outCol = texture2D(texture0, f_texcoord);
	else
		outCol = //vec4((mix-0.80)*10.0,0,0,1.0);
			texture2D(texture0, f_texcoord) * (mix-0.82)*20.0 +
			texture2D(texture1, f_texcoord) * (1.0-((mix-0.82)*20.0));

	outCol[3] = 1.0;


	vec4 path =  texture2D(texture2, vec2(f_coords.x/2.0, 1.0-f_coords.z/2.0));
	outCol = outCol*(1.0-path.a) + path*path.a;
	outCol[3] = 1.0;

	vec4 color = vec4(1.0) * cosTheta;

	if(f_coords.y <= -0.5) // Remove Alpha
		discard;

	color[3] = 1.0;
	gl_FragColor = outCol * color;
}

