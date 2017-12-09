varying vec2 f_texcoord;
varying vec3 f_vnorm;
varying float f_alpha;
uniform sampler2D mytexture;

void main(void)
{
	float cosTheta = clamp(dot(f_vnorm, vec3(1,1,1)), 0.5, 1.0);

	vec4 color = vec4(1.0) * cosTheta;
	color[3] = f_alpha; // Revert Alpha
	gl_FragColor = texture2D(mytexture, f_texcoord) * color;
}

