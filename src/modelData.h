#pragma once

// #include "vertex.h"
#include "glm\glm.hpp"

#define TIP_COLOR glm::make_vec3(tipColor)// glm::vec3{+0.5f, +0.7f, +0.0f}
#define BASE_COLOR glm::make_vec3(baseColor)// glm::vec3{+0.0f, +0.3f, +0.0f}

class ModelData
{
public:
	glm::vec3* vertices{};
	glm::vec3 position{};
	float* tipColor{new float[3]{+0.3f, +0.7f, +0.0f}};
	float* baseColor{new float[3]{+0.0f, +0.0f, +0.0f}};
	int vertexCount{};
	glm::vec3 v1{+1.0f, +1.0f, +0.0f};
	glm::vec3 v2{-1.0f, +1.0f, +0.0f};
	glm::vec3 v3{-1.0f, -1.0f, +0.0f};


	ModelData(glm::vec3 pos)
		: position{pos}
		, vertexCount{12}
	{
		vertices=new glm::vec3[2*vertexCount]{

			glm::vec3{+0.0f, +1.0f, +0.0f},
			TIP_COLOR,

			glm::vec3{+1.0f, -0.0f, -1.0f},
			BASE_COLOR,
			
			glm::vec3{-1.0f, -0.0f, -1.0f},
			BASE_COLOR,





			glm::vec3{+0.0f, +1.0f, +0.0f},
			TIP_COLOR,

			glm::vec3{-1.0f, -0.0f, -1.0f},
			BASE_COLOR,

			glm::vec3{-1.0f, -0.0f, +1.0f},
			BASE_COLOR,





			glm::vec3{+0.0f, +1.0f, +0.0f},
			TIP_COLOR,
			
			glm::vec3{+1.0f, -0.0f, +1.0f},
			BASE_COLOR,

			glm::vec3{+1.0f, -0.0f, -1.0f},
			BASE_COLOR,





			glm::vec3{+0.0f, +1.0f, +0.0f},
			TIP_COLOR,

			glm::vec3{-1.0f, -0.0f, +1.0f},
			BASE_COLOR,

			glm::vec3{+1.0f, -0.0f, +1.0f},
			BASE_COLOR,
		};
	}

	void refresh()
	{
		// std::cout << tipColor[0] << ' ' << tipColor[1] << ' ' << tipColor[2] << '\n';
		vertices[1] =glm::make_vec3(tipColor);
		vertices[3] =glm::make_vec3(baseColor);
		vertices[5] =vertices[3];

		vertices[7] =vertices[1];
		vertices[9] =vertices[3];
		vertices[11]=vertices[3];

		vertices[13]=vertices[1];
		vertices[15]=vertices[3];
		vertices[17]=vertices[3];

		vertices[19]=vertices[1];
		vertices[21]=vertices[3];
		vertices[23]=vertices[3];
	}

	int getVertexBufferSize()
	{
		return vertexCount * sizeof(glm::vec3) * 2;
	}

	~ModelData()
	{
		delete vertices;
		delete tipColor;
	}
};