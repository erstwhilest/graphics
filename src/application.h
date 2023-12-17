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

float bladeHeight{1.0f};

float bladeSpacing{1.0f};
float lastBladeSpacing{bladeSpacing};
float bladeSize{1.0f};

double easeTime{};
Shader ourShader;
Shader ourLightShader;

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

unsigned int lightVBO;
unsigned int lightVAO;

GLFWwindow* window{};

double fpsLimit{1.0f/144.0f};
double lastTime{};
double currentTime{};
double deltaTime2{};

float ambientStrength{1};


ModelData model;


void draw()
{
	ImGui::Render();
	// render
		// ------
		
		glClearColor(model.baseColor[0], model.baseColor[1], model.baseColor[2], 1.0f);//0.25f, 0.25f, 0.25f
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		if (render)
		{
			// activate shader
			ourShader.use();

			ourShader.setVec3("baseColor", glm::make_vec3(model.baseColor));
			ourShader.setVec3("tipColor", glm::make_vec3(model.tipColor));
			ourShader.setVec3("lightColor", glm::vec3(+1.0f, +1.0f, +1.0f));
			ourShader.setVec3("lightPos", glm::vec3(+0.0f, +10.0f, +0.0f));
			ourShader.setFloat("ambientStrength", ambientStrength);


			// pass projection matrix to shader (note that in this case it could change every frame)
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 10000.0f);
			ourShader.setMat4("projection", projection);

			// camera/view transformation
			glm::mat4 view = camera.GetViewMatrix();
			ourShader.setMat4("view", view);
			ourShader.setFloat("time", glfwGetTime());

			glBindVertexArray(VAO);
			glm::mat4 modelTransform = glm::mat4(1.0f);
			// float angle = 3.14f;
			// modelTransform=glm::rotate(modelTransform, glm::radians(angle)+(float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 1.0f));
			modelTransform = glm::scale(modelTransform, glm::vec3(bladeSize, 1.0f, bladeSize));
			modelTransform = glm::scale(modelTransform, glm::vec3(1.0f, bladeHeight, 1.0f));
			ourShader.setMat4("model", modelTransform);
			glDrawArraysInstanced(GL_TRIANGLES, 0, model.vertexCount, model.triangleCount);


			ourLightShader.use();

			ourLightShader.setMat4("projection", projection);
			ourLightShader.setMat4("view", view);
			modelTransform = glm::mat4(1.0f);
			modelTransform = glm::scale(modelTransform, glm::vec3(5.0f));
			modelTransform = glm::translate(modelTransform, glm::vec3(0.0f, 10.0f, 0.0f));
			ourLightShader.setMat4("model", modelTransform);
			
			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
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
	: model{glm::vec3( 0.0f,  0.0f,  0.0f)}
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
	ourLightShader = Shader("src/lightvertexshader.glsl", "src/lightfragmentshader.glsl");

	// glGenBuffers(sizeof(buffers)/sizeof(buffers[0]), buffers);
	// EBO = buffers[0];
	// VBO = buffers[1];
	// instanceVBO = buffers[2];
	// randomVBO = buffers[3];
	// normalVBO = buffers[4];
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);


	// normal attribute
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*model.sideCount, model.normals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glVertexAttribDivisor(1, 4);
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


	// LIGHT SOURCE CUBE
	glm::vec3 lightCube[] = 
	{
		glm::vec3{-0.5, +0.5, +0.5},
		glm::vec3{-0.5, -0.5, +0.5},
		glm::vec3{+0.5, -0.5, +0.5},
		glm::vec3{-0.5, +0.5, +0.5},
		glm::vec3{+0.5, -0.5, +0.5},
		glm::vec3{+0.5, +0.5, +0.5},
		
		glm::vec3{+0.5, +0.5, +0.5},
		glm::vec3{+0.5, -0.5, +0.5},
		glm::vec3{+0.5, -0.5, -0.5},
		glm::vec3{+0.5, +0.5, +0.5},
		glm::vec3{+0.5, -0.5, -0.5},
		glm::vec3{+0.5, +0.5, -0.5},
		
		glm::vec3{+0.5, +0.5, -0.5},
		glm::vec3{+0.5, -0.5, -0.5},
		glm::vec3{-0.5, -0.5, -0.5},
		glm::vec3{+0.5, +0.5, -0.5},
		glm::vec3{-0.5, -0.5, -0.5},
		glm::vec3{-0.5, +0.5, -0.5},
		
		glm::vec3{-0.5, +0.5, -0.5},
		glm::vec3{-0.5, -0.5, -0.5},
		glm::vec3{-0.5, -0.5, +0.5},
		glm::vec3{-0.5, +0.5, -0.5},
		glm::vec3{-0.5, -0.5, +0.5},
		glm::vec3{-0.5, +0.5, +0.5},
		
		glm::vec3{-0.5, +0.5, -0.5},
		glm::vec3{-0.5, +0.5, +0.5},
		glm::vec3{+0.5, +0.5, +0.5},
		glm::vec3{-0.5, +0.5, -0.5},
		glm::vec3{+0.5, +0.5, +0.5},
		glm::vec3{+0.5, +0.5, -0.5},
		
		glm::vec3{+0.5, -0.5, +0.5},
		glm::vec3{-0.5, -0.5, -0.5},
		glm::vec3{+0.5, -0.5, -0.5},
		glm::vec3{+0.5, -0.5, +0.5},
		glm::vec3{-0.5, -0.5, +0.5},
		glm::vec3{-0.5, -0.5, -0.5},

	};

	// light
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glGenBuffers(1, &lightVBO);
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lightCube), lightCube, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);


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
			ImGui::ColorPicker3("Tip Color", model.tipColor);
			ImGui::ColorPicker3("Base Color", model.baseColor);
			// model.refresh();
			// glBindBuffer(GL_ARRAY_BUFFER, VBO);
			// glBufferData(GL_ARRAY_BUFFER, model.getVertexBufferSize(), model.vertices, GL_STATIC_DRAW);
			// glBufferSubData(GL_ARRAY_BUFFER, 0, model.getVertexBufferSize(), model.vertices);
			// glBindBuffer(GL_ARRAY_BUFFER, 0);

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
			ImGui::SliderFloat("Ambeint Strength", &ambientStrength, 0.01, 1);
			
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