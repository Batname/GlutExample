// https://www.packtpub.com/books/content/rendering-stereoscopic-3d-models-using-opengl
// http://paulbourke.net/stereographics/stereorender/

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>


#include "Camera.h"
#include "FileSystem.h"
#include "Shader.h"

#include <iostream>
#include <cmath>

class App
{
public:
	App();
	~App();

	void Start();

public:
	static App* app;

private:
	void LoadCubes();
	void LoadLight();
	void LoadDebugPoint();

	void RenderCubes();
	void RenderLight();
	void RenderDebugPoint();

private:
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void processInput(GLFWwindow *window);
	static unsigned int loadTexture(const char *path);
	void MainRender(bool IsLeftEye);
private:
	// settings
	unsigned int SCR_WIDTH;
	unsigned int SCR_HEIGHT;

	// current widows size
	int CurrentWidth;
	int CurrentHeight;

	// camera
	Camera* camera;
	float lastX;
	float lastY;
	bool firstMouse = true;

	float scaleFarPlane = 100.f;
	float scaleNearPlane = 0.01f;
	float ParallaxScale = 2.f;
	float VirtualCameraOffsetZ = 200.f;
	float ScreenWidth = 3840.f;
	float ScreenHight = 2160.f;
	float ScreenSizeInch = 65.f;

	float NearPlane = 0.1f;
	float FarPlane = 10000.f;

	// timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	// lighting
	glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);

	unsigned int diffuseMap;
	unsigned int specularMap;

	Shader* lightingShader;
	Shader* lampShader;
	Shader* DebugPointShader;

// Geometry
private:
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float CubeVertices[288] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	float DebugPointVertices[12] =
	{
		0.5f,  0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};

	unsigned int DebugPointIndices[6] =
	{
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	// positions all containers
	glm::vec3 cubePositions[10] = {
		glm::vec3(0.0f,  0.05f,  2.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	// positions of the point lights
	glm::vec3 pointLightPositions[4] = {
		glm::vec3(0.7f,  3.2f,  -2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  2.0f, 3.0f)
	};

// GLFW
private:
	GLFWwindow* window;
	GLFWmonitor* PrimaryMonitor;
	int MonitorsCount;
	GLFWmonitor** Monitors;
	GLFWmonitor*  DimencoMonitor;
	const GLFWvidmode* DimencoMonitorMode;


// Screen
private:
	float pixelsize_cm;
	float width_cm;
	float height_cm;

// Eye Tracking data
private:
	glm::vec3 LeftEye;
	glm::vec3 RightEye;
	glm::vec3 MiddleEye;

// Parralax scale
private:
	glm::vec3 LeftEyeScaled;
	glm::vec3 RightEyeScaled;
	float width_cm_scaled;
	float height_cm_scaled;

// Eye coords
private:
	glm::vec3 EyeViewPointOffset_inUnits;
	glm::vec3 FacePosOriginal;
	glm::vec3 EyeSeparation;

// Matrices
private:
	glm::mat4 view;
	glm::mat4 PerspectiveProjection;

// OpenGl buffers
private:
	unsigned int cubeVBO, DebugPointVBO;
	unsigned int lightVAO, cubeVAO, DebugPointVAO;
	unsigned int DebugPointEBO;

};

// Set global pointer
App* App::app = nullptr;

App::App()
{
	// Set Global poiter first
	app = this;


	// start listening UDP packages
	camera = new Camera(glm::vec3(0.0f, 0.0f, 100.0f / 100.f));
	camera->ListenCamerasUDP();

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);

	glfwSwapInterval(0);


#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// read data about monitor
	PrimaryMonitor = glfwGetPrimaryMonitor();
	Monitors = glfwGetMonitors(&MonitorsCount);
	DimencoMonitor = Monitors[MonitorsCount - 1];
	DimencoMonitorMode = glfwGetVideoMode(DimencoMonitor);

	// Setup settings sizes
	SCR_WIDTH = CurrentWidth = DimencoMonitorMode->width;
	SCR_HEIGHT = CurrentHeight = DimencoMonitorMode->height;

	// camera sizes
	lastX = SCR_WIDTH / 2.0f;
	lastY = SCR_HEIGHT / 2.0f;


	// glfw window creation
	// --------------------

	//window = glfwCreateWindow(DimencoMonitorMode->width, DimencoMonitorMode->height, "LearnOpenGL", NULL, NULL); // windowed
	
	// "Windowed full screen" windows
	//glfwWindowHint(GLFW_RED_BITS, DimencoMonitorMode->redBits);
	//glfwWindowHint(GLFW_GREEN_BITS, DimencoMonitorMode->greenBits);
	//glfwWindowHint(GLFW_BLUE_BITS, DimencoMonitorMode->blueBits);
	//glfwWindowHint(GLFW_REFRESH_RATE, DimencoMonitorMode->refreshRate);
	//window = glfwCreateWindow(DimencoMonitorMode->width, DimencoMonitorMode->height, "LearnOpenGL", DimencoMonitor, NULL);

	// Full screen windows
	window = glfwCreateWindow(DimencoMonitorMode->width, DimencoMonitorMode->height, "LearnOpenGL", DimencoMonitor, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	lightingShader = new Shader("../resources/shaders/Main.vertex.glsl", "../resources/shaders/Main.fragment.glsl");
	lampShader = new Shader("../resources/shaders/Lamp.vertex.glsl", "../resources/shaders/Lamp.fragment.glsl");
	DebugPointShader = new Shader("../resources/shaders/DebugPoint.vertex.glsl", "../resources/shaders/DebugPoint.fragment.glsl");

	// Set screen dementions
	pixelsize_cm = (float)((ScreenSizeInch * 2.54) / sqrt(ScreenWidth * ScreenWidth + ScreenHight * ScreenHight));
	width_cm = (float)(ScreenWidth * pixelsize_cm);
	height_cm = (float)(ScreenHight * pixelsize_cm);

	// Load Geometry and textures
	LoadCubes();
	LoadLight();
	LoadDebugPoint();
}

App::~App()
{
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &DebugPointVBO);
	glDeleteVertexArrays(1, &DebugPointVAO);
	glDeleteBuffers(1, &DebugPointEBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();

	// Close cameras udp connection
	camera->CloseCamerasUDP();
}

void App::LoadCubes()
{
	// first, configure the cube's VAO (and VBO)
	glGenBuffers(1, &cubeVBO);
	glGenVertexArrays(1, &cubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), CubeVertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	diffuseMap = loadTexture(FileSystem::getPath("resources/textures/container2.png").c_str());
	specularMap = loadTexture(FileSystem::getPath("resources/textures/container2_specular.png").c_str());
}

void App::LoadLight()
{
	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	// shader configuration
	// --------------------
	lightingShader->use();
	lightingShader->setInt("material.diffuse", 0);
	lightingShader->setInt("material.specular", 1);
}

void App::LoadDebugPoint()
{
	// Bind Debug Point
	glGenVertexArrays(1, &DebugPointVAO);
	glGenBuffers(1, &DebugPointVBO);
	glGenBuffers(1, &DebugPointEBO);
	glBindVertexArray(DebugPointVAO);

	glBindBuffer(GL_ARRAY_BUFFER, DebugPointVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DebugPointVertices), DebugPointVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, DebugPointEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(DebugPointIndices), DebugPointIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void App::processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// do not handle input
	return;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		App::app->camera->ProcessKeyboard(FORWARD, App::app->deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		App::app->camera->ProcessKeyboard(BACKWARD, App::app->deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		App::app->camera->ProcessKeyboard(LEFT, App::app->deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		App::app->camera->ProcessKeyboard(RIGHT, App::app->deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void App::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	App::app->CurrentWidth = width;
	App::app->CurrentHeight = height;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void App::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	return;

	if (App::app->firstMouse)
	{
		App::app->lastX = xpos;
		App::app->lastY = ypos;
		App::app->firstMouse = false;
	}

	float xoffset = xpos - App::app->lastX;
	float yoffset = App::app->lastY - ypos; // reversed since y-coordinates go from bottom to top

	App::app->lastX = xpos;
	App::app->lastY = ypos;

	App::app->camera->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	App::app->camera->ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int App::loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void App::Start()
{
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render for left Eye
		MainRender(true);

		// Render for right Eye
		MainRender(false);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void App::RenderCubes()
{
	// world transformation
	glm::mat4 model;

	// Draw light
	// ------------------------------------------------------------------
	// be sure to activate shader when setting uniforms/drawing objects
	lightingShader->use();
	lightingShader->setVec3("viewPos", camera->Position + EyeViewPointOffset_inUnits);
	lightingShader->setFloat("material.shininess", 32.0f);

	/*
	Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
	the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
	by defining light types as classes and set their values in there, or by using a more efficient uniform approach
	by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
	*/
	// directional light
	lightingShader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	lightingShader->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	lightingShader->setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	lightingShader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
	// point light 1
	lightingShader->setVec3("pointLights[0].position", pointLightPositions[0]);
	lightingShader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	lightingShader->setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	lightingShader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	lightingShader->setFloat("pointLights[0].constant", 1.0f);
	lightingShader->setFloat("pointLights[0].linear", 0.09);
	lightingShader->setFloat("pointLights[0].quadratic", 0.032);
	// point light 2
	lightingShader->setVec3("pointLights[1].position", pointLightPositions[1]);
	lightingShader->setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	lightingShader->setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	lightingShader->setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	lightingShader->setFloat("pointLights[1].constant", 1.0f);
	lightingShader->setFloat("pointLights[1].linear", 0.09);
	lightingShader->setFloat("pointLights[1].quadratic", 0.032);
	// point light 3
	lightingShader->setVec3("pointLights[2].position", pointLightPositions[2]);
	lightingShader->setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	lightingShader->setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	lightingShader->setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	lightingShader->setFloat("pointLights[2].constant", 1.0f);
	lightingShader->setFloat("pointLights[2].linear", 0.09);
	lightingShader->setFloat("pointLights[2].quadratic", 0.032);
	// point light 4
	lightingShader->setVec3("pointLights[3].position", pointLightPositions[3]);
	lightingShader->setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	lightingShader->setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	lightingShader->setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	lightingShader->setFloat("pointLights[3].constant", 1.0f);
	lightingShader->setFloat("pointLights[3].linear", 0.09);
	lightingShader->setFloat("pointLights[3].quadratic", 0.032);
	// spotLight
	//lightingShader->setVec3("spotLight.position", camera->Position + EyeViewPointOffset_inUnits);
	lightingShader->setVec3("spotLight.direction", camera->Front);
	lightingShader->setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	lightingShader->setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	lightingShader->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	lightingShader->setFloat("spotLight.constant", 1.0f);
	lightingShader->setFloat("spotLight.linear", 0.09);
	lightingShader->setFloat("spotLight.quadratic", 0.032);
	lightingShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	lightingShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

	// Global transformation matrix
	lightingShader->setMat4("projection", PerspectiveProjection);
	lightingShader->setMat4("view", view);

	// world transformation
	lightingShader->setMat4("model", model);

	// bind diffuse map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	// bind specular map
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);

	// render containers
	glBindVertexArray(cubeVAO);
	for (unsigned int i = 1; i < 11; i++)
	{
		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model;

		// scale if cube 0;
		if (i == 1)
		{
			model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));
		}

		model = glm::translate(model, cubePositions[i -1]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		lightingShader->setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void App::RenderLight()
{
	// world transformation
	glm::mat4 model;

	// also draw the lamp object(s)
	lampShader->use();
	lampShader->setMat4("projection", PerspectiveProjection);
	lampShader->setMat4("view", view);

	// we now draw as many light bulbs as we have point lights.
	glBindVertexArray(lightVAO);
	for (unsigned int i = 0; i < 4; i++)
	{
		model = glm::mat4();
		model = glm::translate(model, pointLightPositions[i]);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		lampShader->setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void App::RenderDebugPoint()
{
	// world transformation
	glm::mat4 DebugPointModel;
	glm::mat4 DebugPointProjection;
	glm::mat4 DebugPointView;

	// Draw debug Point
	// ------------------------------------------------------------------
	DebugPointShader->use();
	glBindVertexArray(DebugPointVAO);

	glm::vec2 EyeScreenLocation = glm::vec2(MiddleEye.x / width_cm * 2.f, MiddleEye.y / height_cm * 2.f);

	float DebugTrScalar = 0.01f;
	DebugPointModel = glm::scale(DebugPointModel, glm::vec3(DebugTrScalar));
	DebugPointModel = glm::translate(DebugPointModel, glm::vec3(EyeScreenLocation.x / DebugTrScalar, EyeScreenLocation.y / DebugTrScalar, 0.f));
	DebugPointModel = glm::translate(DebugPointModel, glm::vec3(0.35f/2, 0.f, 0.f));


	DebugPointShader->setMat4("projection", DebugPointProjection);
	DebugPointShader->setMat4("view", DebugPointView);
	DebugPointShader->setMat4("model", DebugPointModel);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void App::MainRender(bool IsLeftEye)
{
	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	LeftEye = camera->LeftEye;
	RightEye = camera->RightEye;
	MiddleEye = (LeftEye + RightEye) / 2.f;

	// setup camera paralax planes
	LeftEyeScaled = LeftEye * ParallaxScale;
	RightEyeScaled = RightEye * ParallaxScale;
	width_cm_scaled = width_cm * ParallaxScale;
	height_cm_scaled = width_cm * ParallaxScale;

	float zoff = 0.0f;
	glm::vec3 pa = glm::vec3(-width_cm / 2.0f, -height_cm / 2.0f, -zoff);
	glm::vec3 pb = glm::vec3(width_cm / 2.0f, -height_cm / 2.0f, -zoff);
	glm::vec3 pc = glm::vec3(-width_cm / 2.0f, height_cm / 2.0f, -zoff);

	if (IsLeftEye)
	{
		glViewport(0, 0, CurrentWidth / 2, CurrentHeight);

		// Get perspective matrix here
		PerspectiveProjection = camera->GeneralizedPerspectiveProjection(pa, pb, pc, LeftEyeScaled, NearPlane, FarPlane);
		//std::cout << "LeftEye PerspectiveProjection" << glm::to_string(LeftEye) << "\r\n";
		projection = PerspectiveProjection;

		// 1 unit == 1m == 100 cm
		EyeViewPointOffset_inUnits = LeftEyeScaled / 100.f;
		EyeViewPointOffset_inUnits += glm::vec3(0.f, 0.f, -VirtualCameraOffsetZ / 100.f);
		//std::cout << "EyeViewPoint_inUnits: " << glm::to_string(EyeViewPointOffset_inUnits) << std::endl;
	}
	else
	{
		glViewport(CurrentWidth / 2, 0, CurrentWidth / 2, CurrentHeight);


		// Get perspective matrix here
		PerspectiveProjection = camera->GeneralizedPerspectiveProjection(pa, pb, pc, RightEyeScaled, NearPlane, FarPlane);
		//std::cout << "RightEye PerspectiveProjection" << glm::to_string(RightEye) << "\r\n";
		projection = PerspectiveProjection;

		// 1 unit == 1m == 100 cm
		EyeViewPointOffset_inUnits = RightEyeScaled / 100.f;
		EyeViewPointOffset_inUnits += glm::vec3(0.f, 0.f, -VirtualCameraOffsetZ / 100.f);
		//std::cout << "EyeViewPoint_inUnits: " << glm::to_string(EyeViewPointOffset_inUnits) << std::endl;
	}

	glm::vec3 FacePosOriginal = (camera->LeftEye - camera->RightEye) / 2.f;
	glm::vec3 EyeSeparation;
	EyeSeparation.x = (camera->RightEye.x - camera->LeftEye.x) / 2.f;
	EyeSeparation.y = (camera->RightEye.y - camera->LeftEye.y) / 2.f;
	EyeSeparation.z = (camera->RightEye.z - camera->LeftEye.z) / 2.f;

	//http://paulbourke.net/stereographics/stereorender/
	view = camera->GetViewMatrix(EyeViewPointOffset_inUnits);

	RenderDebugPoint();
	RenderCubes();
	RenderLight();

}




int main()
{
	App OpenGLApp = App();
	OpenGLApp.Start();

	return 0;
}

