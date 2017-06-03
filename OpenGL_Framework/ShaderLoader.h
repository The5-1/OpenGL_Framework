#pragma once

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
void DestroyShaders(GLuint ProgramId);