#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "Mesh.hpp"
#include "SkyBox.hpp"

#include <iostream>

// for skybox
std::vector<const GLchar*> faces;

gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

int glWindowWidth = 1700;
int glWindowHeight = 1100;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

// for the movement of the bus
float move1;
float move2;
float move3;

const unsigned int SHADOW_WIDTH = 4096;
const unsigned int SHADOW_HEIGHT = 2048;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat3 lightDirMatrix;
GLuint lightDirMatrixLoc;

//lights
glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

//camera
gps::Camera myCamera(
	glm::vec3(-10.0f, 10.0f, 10.0f),
	glm::vec3(8.0f, 8.0f, 8.0f),
	glm::vec3(0.0f, 0.0f, 0.0f));
float cameraSpeed = 1.0f;

bool pressedKeys[1024];
GLfloat angle;
GLfloat lightAngle;

gps::Model3D teren;
gps::Model3D bus;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

// rotate camera
bool cameraRotate = false;
float angleCamera = 0.0f;

// camera animation (preview)
bool startPreview = false;
float previewAngle;

void previewFunction() {
	if (startPreview) {
		previewAngle += 0.6f;
		myCamera.scenePreview(previewAngle);
	}
}

// lumina punctiforma
int pointinit = 0;
glm::vec3 lightPos1; // pe sperietoare
GLuint lightPos1Loc;
glm::vec3 lightPos2; // pe casa + fan
GLuint lightPos2Loc;

bool mouse = true;
float lastX = 400, lastY = 300;
float yaw = -90.0f, pitch;

GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

// initialize faces for skybox
void initFaces()
{
	faces.push_back("textures/skybox1/right.tga");
	faces.push_back("textures/skybox1/left.tga");
	faces.push_back("textures/skybox1/top.tga");
	faces.push_back("textures/skybox1/bottom.tga");
	faces.push_back("textures/skybox1/back.tga");
	faces.push_back("textures/skybox1/front.tga");
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (mouse)
	{
		lastX = xpos;
		lastY = ypos;
		mouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.2f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	myCamera.rotate(pitch, yaw);
}

void processMovement()
{
	// move camera forward
	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	// move camera backward
	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	// move camera left
	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	// move camera right
	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	// move light
	if (pressedKeys[GLFW_KEY_L]) {

		lightAngle += 0.5f;
		if (lightAngle > 360.0f)
			lightAngle -= 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}

	// move light
	if (pressedKeys[GLFW_KEY_K]) {
		lightAngle -= 0.5f;
		if (lightAngle < 0.0f)
			lightAngle += 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}

	// print camera's coordinates
	if (pressedKeys[GLFW_KEY_I]) {
		printf("x = %f   y = %f   z = %f \n", myCamera.cameraPosition.x, myCamera.cameraPosition.y, myCamera.cameraPosition.z);
		printf("x = %f   y = %f   z = %f \n", myCamera.cameraFrontDirection.x, myCamera.cameraFrontDirection.y, myCamera.cameraFrontDirection.z);
	}

	// move the bus back
	if (pressedKeys[GLFW_KEY_LEFT]) {
		if (move2 < 100)
			move2 -= 0.5;
	}

	// move the bus front 
	if (pressedKeys[GLFW_KEY_RIGHT]) {
		if (move2 < 100)
			move2 += 0.5;
	}

	// start preview
	if (pressedKeys[GLFW_KEY_1]) {
		startPreview = true;
	}

	// stop preview
	if (pressedKeys[GLFW_KEY_2]) {
		startPreview = false;
	}

	// start pointlight
	if (pressedKeys[GLFW_KEY_3]) {
		myCustomShader.useShaderProgram();
		pointinit = 1;
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "pointinit"), pointinit);
	}

	// stop pointlight
	if (pressedKeys[GLFW_KEY_4]) {
		myCustomShader.useShaderProgram();
		pointinit = 0;
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "pointinit"), pointinit);
	}

	// line view
	if (pressedKeys[GLFW_KEY_7]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// point view
	if (pressedKeys[GLFW_KEY_8]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	// normal view
	if (pressedKeys[GLFW_KEY_9]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glm::vec3 cameraPosition = myCamera.getCameraPosition();
	std::cout << "Camera Position: " << cameraPosition.x << ", " << cameraPosition.y << ", " << cameraPosition.z << std::endl;
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); 
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
	glfwWindowHint(GLFW_SAMPLES, 4); 

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL); //glfwGetPrimaryMonitor
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(glWindow); 
	glfwSetKeyCallback(glWindow, keyboardCallback); 
	glfwSetCursorPosCallback(glWindow, mouseCallback); 

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	// for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initFBO()
{
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix()
{
	const GLfloat near_plane = 35.0f, far_plane = 200.0f;
	glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
	glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	glm::mat4 lightView = glm::lookAt(lightDirTr, myCamera.getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));

	return lightProjection * lightView;
}

void initModels()
{
	teren.LoadModel("objects/fotbal.obj");
	bus.LoadModel("objects/bus.obj");
}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
}

void initUniforms()
{
	myCustomShader.useShaderProgram();
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	lightDirMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDirMatrix");

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 2.5f, 0.5f) * 20.0f;
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	// set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	// pointlight
	lightPos1 = glm::vec3(4.8f, 1.77f, -50.75f); // la sperietoare
	lightPos1Loc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos1");
	glUniform3fv(lightPos1Loc, 1, glm::value_ptr(lightPos1));

	lightPos2 = glm::vec3(-30.3f, 3.77f, -42.0f); // la butoaie
	lightPos2Loc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos2");
	glUniform3fv(lightPos2Loc, 1, glm::value_ptr(lightPos2));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// preview
	previewFunction();

	processMovement();

	// 1st step: render the scene to the depth buffer 

	depthMapShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	// draw the scene
	depthMapShader.useShaderProgram();
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(10.0f));
	model = glm::rotate(model, glm::radians(195.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	teren.Draw(depthMapShader);

	//draw the bus
	depthMapShader.useShaderProgram();
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(10.0f));
	model = glm::rotate(model, glm::radians(195.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-28.0f + move2, -0.1f + move1, 14.0f + move3));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	bus.Draw(depthMapShader);

	/////////////////////////////////////////////////

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2nd step: render the scene
	myCustomShader.useShaderProgram();

	// send lightSpace matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));

	// send view matrix to shader
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

	// compute light direction transformation matrix
	lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
	// send lightDir matrix data to shader
	glUniformMatrix3fv(lightDirMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightDirMatrix));

	glViewport(0, 0, retina_width, retina_height);
	myCustomShader.useShaderProgram();

	// bind the depth map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

	// draw the scene
	myCustomShader.useShaderProgram();
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(10.0f));
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(195.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	// compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	teren.Draw(myCustomShader);

	//draw the bus
	myCustomShader.useShaderProgram();
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-515.0f, -27.8941f, -170.931f));
	model = glm::scale(model, glm::vec3(2.0f));
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(105.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-28.0f + move2, -0.1f + move1, 14.0f + move3));
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	// compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	bus.Draw(myCustomShader);

	mySkyBox.Draw(skyboxShader, view, projection);
}

void initSkyBoxShader()
{
	mySkyBox.Load(faces);
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
		glm::value_ptr(view));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
		glm::value_ptr(projection));
}

int main(int argc, const char* argv[]) {

	initOpenGLWindow();
	initOpenGLState();
	initFBO();
	initModels();
	initShaders();
	initUniforms();
	initFaces();
	initSkyBoxShader();

	glCheckError();

	while (!glfwWindowShouldClose(glWindow)) {
		renderScene();
		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}
	// close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}