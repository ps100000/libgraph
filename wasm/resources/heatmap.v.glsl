in float i_val;

out float fs_val;

uniform float min_val;
uniform float max_val;
uniform int value_count;
uniform int series_count;
uniform int series_buffer_offset;


uniform float width;
uniform float height;
uniform vec2 origin;

uniform float window_width;
uniform float window_height;

void main(void) {
	fs_val = (i_val - min_val) / (max_val - min_val); 

	int x = gl_VertexID % value_count;
	int y = gl_VertexID / value_count;
	y = (y - series_buffer_offset + series_count) % series_count;
	vec2 coord = vec2(
		float(x) * width / float(value_count),
		float(y) * height / float(series_count));
	vec2 true_coord = coord + origin;

	gl_Position = vec4(
		true_coord.x / window_width - 0.5,
		true_coord.y / window_height - 0.5,
		-0.5, // TODO: z-index in all shaders
		0.5);
}
