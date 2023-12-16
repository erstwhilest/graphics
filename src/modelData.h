#pragma once

// #include "vertex.h"
#include "glm\glm.hpp"

#define TIP_COLOR glm::make_vec3(tipColor)// glm::vec3{+0.5f, +0.7f, +0.0f}
#define BASE_COLOR glm::make_vec3(baseColor)// glm::vec3{+0.0f, +0.3f, +0.0f}

class ModelData
{
public:
	glm::vec3* vertices{};
	glm::vec3* normals{};
	glm::vec3* offsets{};
	glm::vec3* random{};
	glm::vec3 position{};
	float* tipColor{new float[3]{+0.3f, +0.7f, +0.0f}};
	float* baseColor{new float[3]{+0.0f, +0.0f, +0.0f}};
	int sideCount{4};
	int vertexCount{};
	glm::vec3 v1{+1.0f, +1.0f, +0.0f};
	glm::vec3 v2{-1.0f, +1.0f, +0.0f};
	glm::vec3 v3{-1.0f, -1.0f, +0.0f};

	int triangleCountX{10};
	int triangleCountY{1};
	int triangleCountZ{10};
	int triangleCount{};

	ModelData(glm::vec3 pos)
		: position{pos}
		, vertexCount{3*sideCount}
		, triangleCount{triangleCountX*triangleCountY*triangleCountZ}
	{

		vertices=new glm::vec3[vertexCount]
		{
			glm::vec3{+0.0f, +1.0f, +0.0f},
			glm::vec3{-1.0f, -0.0f, +1.0f},
			glm::vec3{+1.0f, -0.0f, +1.0f},



			glm::vec3{+0.0f, +1.0f, +0.0f},
			glm::vec3{+1.0f, +0.0f, +1.0f},
			glm::vec3{+1.0f, +0.0f, -1.0f},



			glm::vec3{+0.0f, +1.0f, +0.0f},
			glm::vec3{+1.0f, +0.0f, -1.0f},
			glm::vec3{-1.0f, -0.0f, -1.0f},



			glm::vec3{+0.0f, +1.0f, +0.0f},
			glm::vec3{-1.0f, +0.0f, -1.0f},
			glm::vec3{-1.0f, +0.0f, +1.0f},
		};

		normals=new glm::vec3[sideCount]
		{
			glm::vec3{+0.0f, +0.5f, +0.5f},
			glm::vec3{+0.5f, +0.5f, +0.0f},
			glm::vec3{-0.5f, +0.0f, -0.5f},
			glm::vec3{-0.5f, +0.5f, +0.0f},
		};
	}

	void generateOffsets(float& spacing)
	{
		float randX=0.f;
		float randY=0.f;
		float randZ=0.f;
		for (int i{}; i < triangleCountX; i++)
		{
			for (int j{}; j < triangleCountY; j++)
			{
				for (int k{}; k < triangleCountZ; k++)
				{
					// randX=spacing*((std::rand()%100)/100.f-.5);
					// randY=1+(std::rand()%100/100.f)*2;
					// randZ=spacing*((std::rand()%100)/100.f-.5);
					offsets[i*triangleCountY*triangleCountZ+j*triangleCountZ+k]=glm::vec3{(float)i*spacing+randX,(float)j+randY,(float)k*spacing+randZ};
				}
			}
		}
	}

	void generateRandom()
	{
		float rand1{};
		float rand2{};
		float rand3{};
		for (int i{}; i<triangleCount; i++)
		{
			// phase
			rand1=(std::rand()%100/100.f);

			// amplitude
			rand2=(std::rand()%100)/100.f*1+.5f;

			// freq
			rand3=((1+std::rand()%100)/100.f)/5.f+0.5f;
			random[i]=glm::vec3(rand1, rand2, rand3);
		}
	}

	void refresh()
	{
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
		return sideCount * sizeof(glm::vec3);
	}

	~ModelData()
	{
		delete vertices;
		delete normals;
		delete tipColor;
		delete baseColor;
		delete offsets;
		delete random;
	}
};