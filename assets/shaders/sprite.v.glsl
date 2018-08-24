attribute vec3 coord3d;
attribute vec2 texcoord;
varying vec2 f_texcoord;
uniform mat4 position;
uniform mat4 mvp;

#ifdef GL_ES
	highp mat4 transpose(in highp mat4 inMatrix) {
		highp vec4 i0 = inMatrix[0];
		highp vec4 i1 = inMatrix[1];
		highp vec4 i2 = inMatrix[2];
		highp vec4 i3 = inMatrix[3];

		highp mat4 outMatrix = mat4(
					 vec4(i0.x, i1.x, i2.x, i3.x),
					 vec4(i0.y, i1.y, i2.y, i3.y),
					 vec4(i0.z, i1.z, i2.z, i3.z),
					 vec4(i0.w, i1.w, i2.w, i3.w)
					 );

		return outMatrix;
	}
#endif


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

