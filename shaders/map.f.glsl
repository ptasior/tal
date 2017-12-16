varying vec2 f_texcoord;
varying vec3 f_vnorm;
varying float f_alpha;
varying float f_z;
uniform sampler2D texture0;
uniform sampler2D texture1;
//uniform sampler2D texture2;

void main(void)
{
	float cosTheta = clamp(dot(f_vnorm, vec3(1,1,1)), 0.5, 1.0);

	vec4 outCol;
	if(f_z < 0.48)
		outCol = texture2D(texture1, f_texcoord);
	else if(f_z > 0.58)
		outCol = texture2D(texture0, f_texcoord);
	else
		outCol =
			texture2D(texture0, f_texcoord) *(f_z-0.47)*10.0 +
			texture2D(texture1, f_texcoord) * (1.0-((f_z-0.47)*10.0));

	outCol[3] = 1.0;

	vec4 color = vec4(1.0) * cosTheta;
	color[3] = f_alpha; // Revert Alpha
	gl_FragColor = outCol * color;
}

