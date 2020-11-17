#version 400
layout(location=0) in vec3 vertex_position;
layout(location=1) in vec3 vertex_colour;

uniform mat4 matrix; //our matrix

out vec3 colour;

void main() {
  gl_Position = matrix * vec4(vertex_position, 1.0);  
  //gl_Position = vec4(vertex_position, 1.0);

  colour = vertex_colour;
}
