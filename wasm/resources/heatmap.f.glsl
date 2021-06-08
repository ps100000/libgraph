in float fs_val;

out vec4 color;

uniform sampler2D gradient;

void main(void) {
	color = texture(gradient, vec2(fs_val, 0.5));
}
