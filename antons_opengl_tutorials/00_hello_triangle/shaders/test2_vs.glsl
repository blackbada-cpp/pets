#version 400
in vec3 vp;

void main() {
  gl_Position = vec4(vp.x - 0.6, vp.y, vp.x, 1.0);
}
