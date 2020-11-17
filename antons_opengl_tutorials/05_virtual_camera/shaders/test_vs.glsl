#version 400
layout(location=0) in vec3 vertex_position;
layout(location=1) in vec3 vertex_colour;

uniform mat4 mvp; 

out vec3 colour;

void main() {
  gl_Position = mvp * vec4(vertex_position, 1.0);  

  colour = vertex_colour;
}
