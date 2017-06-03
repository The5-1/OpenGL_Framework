#pragma once

bool loadIndexedFaceObj(const char * path, std::vector<glm::vec4> & out_vertices, std::vector<int> & out_faces);
bool loadSimpleObj(const char * path, std::vector<glm::vec3> & out_vertices);

bool loadOBJ(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
);



bool loadAssImp(
	const char * path,
	std::vector<unsigned short> & indices,
	std::vector<glm::vec3> & vertices,
	std::vector<glm::vec2> & uvs,
	std::vector<glm::vec3> & normals
);