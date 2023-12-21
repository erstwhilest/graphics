#pragma once

// stl includes
#include <iostream>
#include <cmath>
#include <Windows.h>
#include <cstdlib>
#include <ctime>
#include <random>

// render includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>
#include <ft2build.h>
#include FT_FREETYPE_H 

// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// project files
#include "shader.h"
#include "camera.h"
#include "modelData.h"
#include "textmanager.h"

void framebuffer_size_wrapper(GLFWwindow* window, int width, int height);
void key_wrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_wrapper(GLFWwindow* window, int button, int action, int mods);
void mouse_wrapper(GLFWwindow* window, double xposIn, double yposIn);
void scroll_wrapper(GLFWwindow* window, double xoffset, double yoffset);

class Application
{
public:
// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera{glm::vec3(-100.0f, 75.0f, -100.0f)};

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool mouseFocus = false;
bool render = true;
bool renderNormals = true;

float bladeHeight{80.0f};

float bladeSpacing{4.0f};
float lastBladeSpacing{bladeSpacing};
float bladeSize{2.0f};

double easeTime{};
Shader ourShader;
Shader ourNormalShader;

TextManager textmanager{};

ImGuiIO* ioptr{};

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


unsigned int VAO;
unsigned int VBO;
unsigned int instanceVBO;
unsigned int randomVBO;
unsigned int normalVBO;

#define BUFFER_COUNT 5
unsigned int buffers[BUFFER_COUNT];

GLFWwindow* window{};

double fpsLimit{1/100000.f};
double lastTime{};
double currentTime{};
double deltaTime2{};
unsigned int fps{};

float ambientStrength{1};
float diffuseStrength{0};
float constantStrength{1};
float linearStrength{0.007f};
float quadraticStrength{0.0002f*1000};

float lightPos[3]{+0.0f, +10.0f, +0.0f};
float lightColor[3]{+1.0f, +1.0f, +1.0f};

ModelData model;

void draw()
{
	// render
		// ------
		
		glClearColor(model.baseColor[0], model.baseColor[1], model.baseColor[2], 1.0f);//0.25f, 0.25f, 0.25f
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		if (render)
		{
			glm::mat4 lightTransform = glm::mat4(1.0f);
			// lightTransform = glm::scale(lightTransform, glm::vec3(5.0f));
			// activate shader
			ourShader.use();

			ourShader.setVec3("baseColor", glm::make_vec3(model.baseColor));
			ourShader.setVec3("tipColor", glm::make_vec3(model.tipColor));

			ourShader.setVec3("lightColor", glm::make_vec3(lightColor));
			ourShader.setVec3("lightPos", camera.Position);
			// ourShader.setVec3("lightPos", glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
			ourShader.setMat4("lightModel", lightTransform);
			ourShader.setFloat("ambientStrength", ambientStrength);
			ourShader.setFloat("diffuseStrength", diffuseStrength);
			ourShader.setFloat("constant", constantStrength);
			ourShader.setFloat("lienar", linearStrength);
			ourShader.setFloat("quadratic", quadraticStrength/1000);
			ourShader.setFloat("minCutHeight", 100.0f);
			// ourShader.setVec3("playerPos", camera.Position);


			// pass projection matrix to shader (note that in this case it could change every frame)
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 100000.0f);
			ourShader.setMat4("projection", projection);

			// camera/view transformation
			glm::mat4 view = camera.GetViewMatrix();
			ourShader.setMat4("view", view);
			ourShader.setFloat("time", glfwGetTime());

			glBindVertexArray(VAO);
			glm::mat4 modelTransform = glm::mat4(1.0f);
			modelTransform = glm::scale(modelTransform, glm::vec3(bladeSize, 1.0f, bladeSize));
			modelTransform = glm::scale(modelTransform, glm::vec3(1.0f, bladeHeight, 1.0f));
			ourShader.setMat4("model", modelTransform);
			glDrawArraysInstanced(GL_TRIANGLES, 0, model.vertexCount, model.triangleCount);

			// text rendering
			textmanager.RenderText("FPS:"+std::to_string(fps), 25.0f, 25.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
		}

		

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

public:

Application()
	: model{glm::vec3( 0.0f,  0.0f,  0.0f), bladeSpacing}
{
	// random initialize
	std::srand(std::time(nullptr));

	// glfw initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	// glfw window creation
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);

	// check if window is created
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(1);
	}

	// glfw callback configuration
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_wrapper);
	glfwSetCursorPosCallback(window, mouse_wrapper);
	glfwSetScrollCallback(window, scroll_wrapper);
	glfwSetKeyCallback(window, key_wrapper);
	glfwSetMouseButtonCallback(window, mouse_button_wrapper);

	// glfw mouse capture
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


	// imgui configuration
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ioptr=&io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(1);
	}

	glEnable(GL_DEPTH_TEST);

	// glad transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// glad face culling
	glEnable(GL_CULL_FACE);

	// glad anti aliasing, samples set by glfw above
	glEnable(GL_MULTISAMPLE);

	// glfw vsync off
	glfwSwapInterval(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// build and compile our shader program
	ourShader = Shader("src/vertexshader.glsl", "src/fragmentshader.glsl");
	ourNormalShader = Shader("src/normvertexshader.glsl", "src/normfragmentshader.glsl", "src/normgeometryshader.glsl");

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &instanceVBO);
	glGenBuffers(1, &randomVBO);
	glGenBuffers(1, &normalVBO);

	// create vertex array
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// position attribute
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, model.getVertexBufferSize(), model.vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3)*2, (void*)0);
	glEnableVertexAttribArray(0);

	// normal attribute
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, model.getVertexBufferSize(), model.vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3)*2, (void*)(sizeof(glm::vec3)));
	// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	// glVertexAttribDivisor(1, 1);
	glEnableVertexAttribArray(1);
	
	// offset attribute
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*model.triangleCount, model.offsets, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glVertexAttribDivisor(2, 1);
	glEnableVertexAttribArray(2);

	// random attribute
	glBindBuffer(GL_ARRAY_BUFFER, randomVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*model.triangleCount, model.random, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glVertexAttribDivisor(3, 1);
	glEnableVertexAttribArray(3);

	textmanager.init();

	// unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
	{
		camera.MovementSpeed+=300.0f;
	}
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
	{
		camera.MovementSpeed-=300.0f;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	ioptr->AddMouseButtonEvent(button, action);

	if (!ioptr->WantCaptureMouse)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			mouseFocus=action;
			firstMouse=true;
		}
		if (action)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (mouseFocus)
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


void run()
{
	lastTime=glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{
			ImGui::Begin("Settings");
			ImGui::Checkbox("Render", &render);
			ImGui::Checkbox("Render Normals", &renderNormals);
			if (ImGui::TreeNode("Grass Color"))
			{
				ImGui::ColorPicker3("Tip Color", model.tipColor);
				ImGui::ColorPicker3("Base Color", model.baseColor);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Grass Settings"))
			{
				ImGui::SliderFloat("Blade Height", &bladeHeight, 1, 100);

				ImGui::SliderFloat("Spacing", &bladeSpacing, 0.1, 10);
				if (bladeSpacing!=lastBladeSpacing)
				{
					model.generateOffsets(bladeSpacing);
					glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
					glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*model.triangleCount, model.offsets, GL_DYNAMIC_DRAW);
					// glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3)*model.triangleCount, model.offsets);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
				}
				lastBladeSpacing=bladeSpacing;

				ImGui::SliderFloat("Size", &bladeSize, 0.1, 10);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Light Settings"))
			{
				ImGui::SliderFloat("Ambeint Strength", &ambientStrength, 0, 1);
				ImGui::SliderFloat("Diffuse Strength", &diffuseStrength, 0, 2);
				ImGui::SliderFloat("Constant Strength", &constantStrength, 1, 1);
				ImGui::SliderFloat("Linear Strength", &linearStrength, 0.0014, 0.07);
				ImGui::SliderFloat("Quadratic Strength", &quadraticStrength, 0.001, 1);

				ImGui::ColorPicker3("Light Color", lightColor);
				ImGui::TreePop();
			}
			
			ImGui::End();
		}
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		deltaTime2+=deltaTime;

		ImGui::Render();
		if (deltaTime > fpsLimit)
		{
			processInput(window);
			draw();
			lastFrame=currentFrame;
		}

		if (deltaTime2 > 1.0f / 5)
		{
			fps=static_cast<int>(1/deltaTime);
			deltaTime2=0;
		}

		glfwPollEvents();
	}
}

void cleanUp()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(BUFFER_COUNT, buffers);

	glfwTerminate();
}
};

static Application* callback{};

void framebuffer_size_wrapper(GLFWwindow* window, int width, int height)
{
	if (callback)
	{
		callback->framebuffer_size_callback(window, width, height);
	}
}

void key_wrapper(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (callback)
	{
		callback->key_callback(window,  key,  scancode,  action,  mods);
	}
}

void mouse_button_wrapper(GLFWwindow* window, int button, int action, int mods)
{
	if (callback)
	{
		callback->mouse_button_callback(window,  button,  action,  mods);
	}
}

void mouse_wrapper(GLFWwindow* window, double xposIn, double yposIn)
{
	if (callback)
	{
		callback->mouse_callback(window,  xposIn,  yposIn);
	}
}

void scroll_wrapper(GLFWwindow* window, double xoffset, double yoffset)
{
	if (callback)
	{
		callback->scroll_callback(window,  xoffset,  yoffset);
	}
}