// render includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>

// stl includes
#include <iostream>
#include <cmath>
#include <Windows.h>
#include <cstdlib>
#include <ctime>
#include <random>

// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// project files
#include "shader.h"
#include "camera.h"
#include "modelData.h"

void framebuffer_size_wrapper(GLFWwindow* window, int width, int height);
void key_wrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_wrapper(GLFWwindow* window, int button, int action, int mods);
void mouse_wrapper(GLFWwindow* window, double xposIn, double yposIn);
void scroll_wrapper(GLFWwindow* window, double xoffset, double yoffset);

class Application
{

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera{glm::vec3(-100.0f, 100.0f, -100.0f)};

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool mouseFocus = false;
bool render = true;

float bladeHeight{10.0f};

float bladeSpacing{2.0f};
float lastBladeSpacing{bladeSpacing};
float bladeSize{1.0f};

double easeTime{};
Shader ourShader;

ImGuiIO* ioptr{};

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int triangleCountX{500};
int triangleCountY{1};
int triangleCountZ{500};
int triangleCount{triangleCountX*triangleCountY*triangleCountZ};

// glm::vec3* cubePos;

// unsigned int texture1, texture2;
unsigned int VAO;
unsigned int VBO;
unsigned int instanceVBO;
unsigned int randomVBO;

unsigned int buffers[3]={VBO, instanceVBO, randomVBO};

GLFWwindow* window{};

double fpsLimit{1.0f/144.0f};
double lastTime{};
double currentTime{};
double deltaTime2{};


ModelData* model{};
glm::vec3* offsets{};

glm::vec3* random{};

void draw()
{
	ImGui::Render();
	// render
		// ------
		
		glClearColor(model->baseColor[0], model->baseColor[1], model->baseColor[2], 1.0f);//0.25f, 0.25f, 0.25f
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		// activate shader
		ourShader.use();

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 10000.0f);
		ourShader.setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("view", view);

		ourShader.setFloat("time", glfwGetTime());

		if (render)
		{
			glBindVertexArray(VAO);
			glm::mat4 modelTransform = glm::mat4(1.0f);
			// float angle = 3.14f;
			// modelTransform=glm::rotate(modelTransform, glm::radians(angle)+(float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 1.0f));
			modelTransform = glm::scale(modelTransform, glm::vec3(bladeSize, 1.0f, bladeSize));
			modelTransform = glm::scale(modelTransform, glm::vec3(1.0f, bladeHeight, 1.0f));
			ourShader.setMat4("model", modelTransform);
			glDrawArraysInstanced(GL_TRIANGLES, 0, model->vertexCount, triangleCount);
		}

		

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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

void changeSpacing(float spacing)
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
				randX=spacing*((std::rand()%100)/100.f-.5);
				randY=1+(std::rand()%100/100.f)*2;
				randZ=spacing*((std::rand()%100)/100.f-.5);
				offsets[i*triangleCountY*triangleCountZ+j*triangleCountZ+k]=glm::vec3{(float)i*spacing+randX,(float)j+randY,(float)k*spacing+randZ};
			}
		}
	}
}

void generateRandomNumbers()
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

public:

Application()
{
	// random initialize
	std::srand(std::time(nullptr));

	// glfw initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);

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

	// create first model
	model = new ModelData{glm::vec3( 0.0f,  0.0f,  0.0f)};

	// create instance offset list
	offsets = new glm::vec3[triangleCount]
	{
	};
	changeSpacing(bladeSpacing);

	// create instance random numbers
	random=new glm::vec3[triangleCount]
	{
	};
	generateRandomNumbers();

		glGenBuffers(3, buffers);
	VBO=buffers[0];
	instanceVBO=buffers[1];
	randomVBO=buffers[2];

	// create vertex array
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// setup vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, model->getVertexBufferSize(), model->vertices, GL_DYNAMIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3), (void*)(sizeof(glm::vec3)));
	glEnableVertexAttribArray(1);

	// offset attribute
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*triangleCount, offsets, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glVertexAttribDivisor(2, 1);
	glEnableVertexAttribArray(2);

	// random attribute
	glBindBuffer(GL_ARRAY_BUFFER, randomVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*triangleCount, random, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glVertexAttribDivisor(3, 1);
	glEnableVertexAttribArray(3);

	// unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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
			ImGui::ColorPicker3("Tip Color", model->tipColor);
			ImGui::ColorPicker3("Base Color", model->baseColor);
			model->refresh();
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, model->getVertexBufferSize(), model->vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			ImGui::SliderFloat("Blade Height", &bladeHeight, 1, 100);

			ImGui::SliderFloat("Spacing", &bladeSpacing, 0.1, 10);
			if (bladeSpacing!=lastBladeSpacing)
			{
				changeSpacing(bladeSpacing);
				glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3)*triangleCount, offsets);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
			lastBladeSpacing=bladeSpacing;

			ImGui::SliderFloat("Size", &bladeSize, 0.1, 10);
			
			ImGui::End();
		}
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		draw();
		

		glfwPollEvents();
	}
}
void cleanUp()
{

	delete model;
	delete offsets;
	delete random;
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(3, buffers);

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