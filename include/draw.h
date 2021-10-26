#pragma once
#include "precompiled.h"
#include "tiny_obj_loader.h"

class Draw {
private:

	GLuint VertexArrayObject;
	GLuint PositionVertexBufferID;
	GLuint NormalVertexBufferID;
	GLuint ColorVertexBufferID;
	GLuint ElementVertexBufferID;
	GLuint AmbColorVertexBufferID;
	GLuint SpcColorVertexBufferID;

	GLuint ShaderProgramID;
	GLuint MatrixID_1;
	GLuint MatrixID_2;
	GLuint MatrixID_3;
	GLuint MatrixID_4;

	std::vector<unsigned int> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec3> indexed_normals;
	std::vector<glm::vec3> indexed_color;
	std::vector<glm::vec3> indexed_color_a;
	std::vector<glm::vec3> indexed_color_s;
	
public:

	Draw()
	{
	}

	~Draw()
	{
		glDeleteBuffers(1, &PositionVertexBufferID);
		glDeleteBuffers(1, &NormalVertexBufferID);
		glDeleteBuffers(1, &ColorVertexBufferID);
		glDeleteBuffers(1, &SpcColorVertexBufferID);
		glDeleteBuffers(1, &AmbColorVertexBufferID);
		glDeleteBuffers(1, &ElementVertexBufferID);
		glDeleteVertexArrays(1, &VertexArrayObject);
		glBindVertexArray(0);
		glUseProgram(0);
		glDeleteProgram(ShaderProgramID);
	}

	void draw_init()
	{
		glGenBuffers(1, &PositionVertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, PositionVertexBufferID);
		glGenBuffers(1, &NormalVertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, NormalVertexBufferID);
		glGenBuffers(1, &ColorVertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, ColorVertexBufferID);
		glGenBuffers(1, &SpcColorVertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, SpcColorVertexBufferID);
		glGenBuffers(1, &AmbColorVertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, AmbColorVertexBufferID);
		glGenVertexArrays(1, &VertexArrayObject);
		glBindVertexArray(VertexArrayObject);
		glGenBuffers(1, &ElementVertexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementVertexBufferID);
	}

	bool initShaderProgram() 
	{
		const GLchar* vertexShaderSource =
			"#version 330 core\n"
			"in vec3 positionAttribute;"
			"in vec3 colorAttribute;"
			"in vec3 normalAttribute;"
			"in vec3 ambcolorAttribute;"
			"in vec3 spccolorAttribute;"
			"uniform mat4 MVP;"
			"uniform mat4 M;"
			"uniform mat4 V;"
			"uniform mat4 P;"
			"out vec3 passColorAttribute;"
			"out vec3 passambColorAttribute;"
			"out vec3 passspcColorAttribute;"
			"out vec3 lightdir_camera;"
			"out vec3 normal_camera;"
			"out vec3 eyedir_camera;"
			"out float distance;"
			"void main()"
			"{"
			"gl_Position = MVP*vec4(positionAttribute, 1.0);"
			"vec3 position_world = (M*vec4(positionAttribute, 1.0)).xyz;"
			"vec3 position_camera = (V*M*vec4(positionAttribute, 1.0)).xyz;"
			"vec3 eyedir_camera = vec3(0.0,0.0,0.0) - position_camera;"
			"vec3 light_world = (vec4(0.0,-1.5,-2.0,1.0)).xyz;"
			"vec3 light_camera = (V*vec4(light_world,1.0)).xyz;"
			"lightdir_camera = light_camera + eyedir_camera;"
			"normal_camera = (V*M*vec4(normalAttribute, 0.0)).xyz;"
			"distance = length(light_world - position_world);"
			"passColorAttribute = colorAttribute;"
			"passambColorAttribute = ambcolorAttribute;"
			"passspcColorAttribute = spccolorAttribute;"
			"}";

		const GLchar* fragmentShaderSource =
			"#version 330 core\n"
			"in vec3 passColorAttribute;"
			"in vec3 passambColorAttribute;"
			"in vec3 passspcColorAttribute;"
			"in vec3 lightdir_camera;"
			"in vec3 normal_camera;"
			"in vec3 eyedir_camera;"
			"in float distance;"
			"uniform mat4 MVP;"
			"uniform mat4 M;"
			"uniform mat4 V;"
			"uniform mat4 P;"
			"out vec4 fragmentColor;"
			"void main()"
			"{"
			"vec3 n = normalize(normal_camera);"
			"vec3 l = normalize(lightdir_camera);"
			"vec3 e = normalize(eyedir_camera);"
			"float diff = clamp(dot(n, l), 0.0, 1.0);"
			"float reflect = clamp(dot(e, reflect(-l,n)), 0.0, 1.0);"
			"fragmentColor = vec4(0.2*passambColorAttribute + passColorAttribute*(diff)*5.0/(distance*distance) +  passspcColorAttribute*pow(reflect,5)*5.0/(distance*distance), 1.0);"
			"}";

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);

		GLint result;
		GLchar errorLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
		if (!result)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
			std::cerr << "ERROR: vertex shader compile failed\n" << errorLog << std::endl;
			glDeleteShader(vertexShader);
			return false;
		}

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
		if (!result)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
			std::cerr << "ERROR: fragment shader compile failed\n" << errorLog << std::endl;
			return false;
		}

		ShaderProgramID = glCreateProgram();
		glAttachShader(ShaderProgramID, vertexShader);
		glAttachShader(ShaderProgramID, fragmentShader);
		glLinkProgram(ShaderProgramID);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &result);
		if (!result) 
		{
			glGetProgramInfoLog(ShaderProgramID, 512, NULL, errorLog);
			std::cerr << "ERROR: shader program link failed\n" << errorLog << std::endl;
			return false;
		}
		MatrixID_1 = glGetUniformLocation(ShaderProgramID, "MVP");
		MatrixID_2 = glGetUniformLocation(ShaderProgramID, "M");
		MatrixID_3 = glGetUniformLocation(ShaderProgramID, "V");
		MatrixID_4 = glGetUniformLocation(ShaderProgramID, "P");
		glUseProgram(ShaderProgramID);
		glEnable(GL_DEPTH_TEST);
		return true;
	}

	void read_obj(const char* objdir_vec,	const char* objrootdir_vec)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;
		bool res = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, objdir_vec, objrootdir_vec, true);
		if (!err.empty())
		{
			printf("Error while loading obj file: %s\n", err.c_str());
		}
		std::cout << "Vertex num : " << attrib.vertices.size() / 3 << std::endl;
		std::cout << "Normal num : " << attrib.normals.size() / 3 << std::endl;
		std::cout << "Tex num : " << attrib.texcoords.size() / 2 << std::endl;
		for (size_t i = 0; i < shapes.size(); i++)
		{
			std::cout << "Index : " << shapes[i].mesh.indices.size() / 3 << std::endl;
		}

		for (size_t i = 0; i < shapes.size(); i++)
		{
			for (size_t ind = 0; ind < shapes[i].mesh.indices.size(); ind++)
			{
				indices.push_back(shapes[i].mesh.indices[ind].vertex_index);
			}
		}
		for (size_t v = 0; v < attrib.vertices.size() / 3; v++)
		{
			glm::vec3 vertex_tmp;
			vertex_tmp.x = attrib.vertices[3 * v + 0];
			vertex_tmp.y = attrib.vertices[3 * v + 1];
			vertex_tmp.z = attrib.vertices[3 * v + 2];
			indexed_vertices.push_back(vertex_tmp);
		}
		for (size_t n = 0; n < attrib.normals.size() / 3; n++)
		{
			glm::vec3 normal_tmp;
			normal_tmp.x = attrib.normals[3 * n + 0];
			normal_tmp.y = attrib.normals[3 * n + 1];
			normal_tmp.z = attrib.normals[3 * n + 2];
			indexed_normals.push_back(normal_tmp);
		}

		indexed_color.resize(attrib.vertices.size() / 3);
		indexed_color_a.resize(attrib.vertices.size() / 3);
		indexed_color_s.resize(attrib.vertices.size() / 3);
		for (size_t i = 0; i < shapes.size(); i++)
		{
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++)
			{
				size_t fnum = shapes[i].mesh.num_face_vertices[f];
				for (size_t v = 0; v < fnum; v++) {
					tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];
					int material_id = shapes[i].mesh.material_ids[f];
					glm::vec3 color_tmp;
					glm::vec3 color_a;
					glm::vec3 color_s;
					color_tmp.x = materials[material_id].diffuse[0];
					color_tmp.y = materials[material_id].diffuse[1];
					color_tmp.z = materials[material_id].diffuse[2];
					color_a.x = materials[material_id].ambient[0];
					color_a.y = materials[material_id].ambient[1];
					color_a.z = materials[material_id].ambient[2];
					color_s.x = materials[material_id].specular[0];
					color_s.y = materials[material_id].specular[1];
					color_s.z = materials[material_id].specular[2];
					indexed_color[idx.vertex_index] = color_tmp;
					indexed_color_a[idx.vertex_index] = color_a;
					indexed_color_s[idx.vertex_index] = color_s;
				}
				index_offset += fnum;
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, PositionVertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(indexed_vertices[0]), &indexed_vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, ColorVertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, indexed_color.size() * sizeof(indexed_color[0]), &indexed_color[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, AmbColorVertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, indexed_color_a.size() * sizeof(indexed_color_a[0]), &indexed_color_a[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, SpcColorVertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, indexed_color_s.size() * sizeof(indexed_color_s[0]), &indexed_color_s[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, NormalVertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(indexed_normals[0]), &indexed_normals[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementVertexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);

		GLint positionAttribute = glGetAttribLocation(ShaderProgramID, "positionAttribute");
		if (positionAttribute == -1)
		{
			std::cerr << "position attribute setting failed" << std::endl;
		}
		glBindBuffer(GL_ARRAY_BUFFER, PositionVertexBufferID);
		glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(positionAttribute);

		GLint colorAttribute = glGetAttribLocation(ShaderProgramID, "colorAttribute");
		if (colorAttribute == -1)
		{
			std::cerr << "color attribute setting failed" << std::endl;
		}
		glBindBuffer(GL_ARRAY_BUFFER, ColorVertexBufferID);
		glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(colorAttribute);

		GLint ambcolorAttribute = glGetAttribLocation(ShaderProgramID, "ambcolorAttribute");
		if (ambcolorAttribute == -1)
		{
			std::cerr << "ambient color attribute setting failed" << std::endl;
		}
		glBindBuffer(GL_ARRAY_BUFFER, AmbColorVertexBufferID);
		glVertexAttribPointer(ambcolorAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(ambcolorAttribute);

		GLint spccolorAttribute = glGetAttribLocation(ShaderProgramID, "spccolorAttribute");
		if (spccolorAttribute == -1)
		{
			std::cerr << "specular color attribute setting failed" << std::endl;
		}
		glBindBuffer(GL_ARRAY_BUFFER, SpcColorVertexBufferID);
		glVertexAttribPointer(spccolorAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(spccolorAttribute);

		GLint normalAttribute = glGetAttribLocation(ShaderProgramID, "normalAttribute");
		if (normalAttribute == -1)
		{
			std::cerr << "normal attribute setting failed" << std::endl;
		}
		glBindBuffer(GL_ARRAY_BUFFER, NormalVertexBufferID);
		glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(normalAttribute);
	}

	void draw_obj(glm::mat4 MVP, glm::mat4 ModelMatrix, glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix)
	{
		glBindVertexArray(VertexArrayObject);
		glUniformMatrix4fv(MatrixID_1, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(MatrixID_2, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(MatrixID_3, 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(MatrixID_4, 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glDrawElements(GL_TRIANGLES, 3 * indices.size(), GL_UNSIGNED_INT, 0);
	}
};
