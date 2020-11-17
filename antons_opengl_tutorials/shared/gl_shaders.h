#pragma once
#include <string>
#include "GL/glew.h"

std::string LoadShader(const char * path);

void _print_shader_info_log(GLuint shader_index);
bool _log_shader_error(GLuint vs);
void _print_programme_info_log(GLuint programme);
bool _log_programme_error(GLuint programme);
void _print_all(GLuint programme);
bool _is_programme_valid(GLuint programme);
bool InitShaderProgram(const char * vertex_shader_path, const char * fagment_shader_path, GLuint & shader_program);
bool LinkShaderProgram(GLuint shader_program);
