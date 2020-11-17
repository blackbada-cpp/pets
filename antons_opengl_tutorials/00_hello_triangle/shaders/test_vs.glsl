#version 400
in vec3 vp;

void main() {
  gl_Position = vec4(vp.x, vp.y - 0.6, vp.x, 1.0);
}
