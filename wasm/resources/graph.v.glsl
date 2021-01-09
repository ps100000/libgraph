in float val;
uniform float width;
uniform float inverse_x_scale;
uniform float height;
uniform float y_scale;
uniform float y_max;
uniform vec2 origin;

void main(void) {
	float value = val < y_max ? val : y_max;
	gl_Position = vec4(
		(float(gl_VertexID) / inverse_x_scale + origin.x) / width - 0.5,
		(value * y_scale + origin.y) / height - 0.5,
		0,
		0.5);
}
