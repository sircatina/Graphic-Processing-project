//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

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
#include "SkyBox.hpp"


#include <iostream>

int glWindowWidth = 800;
int glWindowHeight = 600;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;
glm::vec3 lightPosition;
GLuint lightPositionLoc;
GLuint lightLoc;

glm::vec3 spotLightPosEye1;
glm::vec3 spotLightPosEye2;
glm::vec3 spotLightPosEye3;

glm::vec3 targetPos1;
glm::vec3 targetPos2;
glm::vec3 targetPos3;

glm::vec3 lightPosEyeWaterTower;
glm::vec3 lightColorPunctiformWaterTower;
GLuint lightColorPunctiformWaterTowerLoc;

glm::mat4 modelElice;
glm::mat3 normalMatrixElice;
GLuint modelLocElice;
GLuint normalMatrixLocELice;
float angleElice = 0.0f;

glm::mat4 modelPD;
glm::mat3 normalMatrixPD;
GLuint modelLocPD;
GLuint normalMatrixLocPD;
float anglePD = 0.0f;

glm::mat4 modelBird;
glm::mat3 normalMatrixBird;
GLuint modelLocBird;
GLuint normalMatrixLocBird;

gps::Camera myCamera(glm::vec3(170.0f, 20.0f, -10.0f), glm::vec3(0.0f, 0.0f, -10.0f)); //vec3(112.42f, 20.791f, 3.00947f);
float cameraSpeed = 0.5f;

bool pressedKeys[1024];
float angleY = 0.0f;
GLfloat lightAngle;

gps::Model3D scena;
gps::Model3D cub;
gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D bird;
gps::Model3D elice;
gps::Model3D rain;
gps::Model3D poartaD;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;


GLuint shadowMapFBO;
GLuint depthMapTexture;

bool showDepthMap;

gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

float lastX = glWindowWidth / 2.0f;
float lastY = glWindowHeight / 2.0f;
float translationX = 0.0f; // Translation amount on the X-axis
bool firstMouse = true;
bool mouseButtonPressed = false;

GLfloat yaw = -90;
GLfloat pitch;

float distanceFromLookAtPoint = 2.5f;
GLfloat isNight = 0.000f;
GLfloat isNightLoc;
GLfloat fogDensity = 0.000f;
GLfloat fogDensityLoc;
GLfloat fogIncrease = 0.00005f;

GLfloat r1 = 0.0f, r2 = 0.0f;
bool rAux = false;
bool itsRaining = false;

int conditionAnimation = 0;

float delta = 0.0f;
float movementSpeed = 0.09f;
double lastTimeStamp = glfwGetTime();
GLfloat angle = 0.0f;

bool open = false;

int birdDirection = 1;
glm::vec3 initialPositionBird = glm::vec3(-10.9227f, 0.2106f, 3.79639f);
// Variabilă globală pentru a reține deplasarea curentă a raței
float birdMovementOffset = -17.32672f;
float birdMovementSpeed = 0.1f; // Viteza de deplasare a raței
int cameraAnimation = 0;
float preview;


GLenum glCheckError_(const char* file, int line) {
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

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	glWindowWidth = width;
	glWindowHeight = height;
	lastX = width / 2.0f;
	lastY = height / 2.0f;

	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glViewport(0, 0, retina_width, retina_height);

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	double yaw, pitch;
	glfwGetCursorPos(window, &yaw, &pitch);
	pitch = -pitch;
	myCamera.rotate(pitch / 4, yaw / 4);
}

void raining() {
	if (r1 < -35.0f) {
		r1 = 40.0f;
		rAux = true;
	}
	r1 -= 0.9f;
	if (rAux) {
		r2 -= 0.9f;
	}
	if (r2 < -35.0f) {
		r2 = 40.0f;
	}
}
void processMovement()
{
	if (pressedKeys[GLFW_KEY_1]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (pressedKeys[GLFW_KEY_2]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	if (pressedKeys[GLFW_KEY_3]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_POLYGON_SMOOTH);
	}
	if (pressedKeys[GLFW_KEY_4]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POLYGON_SMOOTH);
	}


	// Check for key press to initiate animation
	if (pressedKeys[GLFW_KEY_5]) {
		conditionAnimation = 1;
	}


	if (conditionAnimation == 1) {
		myCamera.rotateAroundCenter(0.08f);
		preview += 0.01;
		if (preview > 10.0f) {
			myCamera.move(gps::MOVE_RIGHT, 20.0f);
			preview = 0.0f;
		}
	}
	// opreste camera animation
	if (pressedKeys[GLFW_KEY_6]) {
		conditionAnimation = 0;
	}
	
	if (pressedKeys[GLFW_KEY_7]) {
		itsRaining = true;
	}
	if (pressedKeys[GLFW_KEY_8]) {
		itsRaining = false;
	}
	/*if (pressedKeys[GLFW_KEY_Q]) {
		angleY -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angleY += 1.0f;
	}*/

	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}
	if (pressedKeys[GLFW_KEY_N]) {
		isNight = !isNight;
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "isNight"), isNight);
	}
	
	if (pressedKeys[GLFW_KEY_F]) {
		fogDensity += fogIncrease;

		if (fogDensity > 0.01f || fogDensity < 0.0f) {
			fogIncrease = -fogIncrease;
		}
		myCustomShader.useShaderProgram();
		glUniform1f(fogDensityLoc, fogDensity);
	}
	if (pressedKeys[GLFW_KEY_N]) {
		isNight = 1.0f;
		myCustomShader.useShaderProgram();
		glUniform1f(isNightLoc, isNight);
	}
	if (pressedKeys[GLFW_KEY_Z]) {
		isNight = 0.0f;
		myCustomShader.useShaderProgram();
		glUniform1f(isNightLoc, isNight);
	}
	if (pressedKeys[GLFW_KEY_P]) {
		open = true;
	}
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
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	//glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}
void initSkyBox() {
	std::vector<const GLchar*>faces;
	faces.push_back("skybox/right.tga");
	faces.push_back("skybox/left.tga");
	faces.push_back("skybox/top.tga");
	faces.push_back("skybox/bottom.tga");
	faces.push_back("skybox/back.tga");
	faces.push_back("skybox/front.tga");
	mySkyBox.Load(faces);
}
void initObjects() {
	scena.LoadModel("objects/scena/scenaFinal.obj");
	lightCube.LoadModel("objects/cube/cube.obj");
	bird.LoadModel("objects/scena/bird-mom/rata.obj");
	elice.LoadModel("objects/scena/elice/elic.obj");
	poartaD.LoadModel("objects/scena/rostei/poarta.obj");
	rain.LoadModel("objects/scena/rain/rain.obj");
	screenQuad.LoadModel("objects/quad/quad.obj");
}

void initShaders() {

	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
	depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag");
	depthMapShader.useShaderProgram();
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	
}

void checkGLError() {
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL error: " << err << std::endl;
	}
}
void initUniforms() {
	myCustomShader.useShaderProgram();
	
	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	modelElice = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLocElice = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	modelBird = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLocBird = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	modelPD = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLocPD = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	normalMatrixElice = glm::mat3(glm::inverseTranspose(view * modelElice));
	normalMatrixLocELice = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");

	normalMatrixBird = glm::mat3(glm::inverseTranspose(view * modelBird));
	normalMatrixLocBird = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");

	normalMatrixPD = glm::mat3(glm::inverseTranspose(view * modelPD));
	normalMatrixLocPD = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	lightPosition = glm::vec3(0.0f, 1.0f, 1.0f);
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightPosition"), 1, glm::value_ptr(lightPosition));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	
	myCustomShader.useShaderProgram();
	fogDensityLoc = glGetUniformLocation(myCustomShader.shaderProgram, "fogDensity");
	glUniform1f(fogDensityLoc, fogDensity);

	spotLightPosEye1 = glm::vec3(-4.2762f, 17.4627f, 3.28647f);
	lightLoc = glGetUniformLocation(myCustomShader.shaderProgram, "spotLightPosEye1");
	// send light dir to shader
	glUniform3fv(lightLoc, 1, glm::value_ptr(spotLightPosEye1));

	targetPos1 = glm::vec3(-4.2762f, 9.4727f, 2.98647f);
	lightLoc = glGetUniformLocation(myCustomShader.shaderProgram, "targetPos1");
	// send light dir to shader
	glUniform3fv(lightLoc, 1, glm::value_ptr(targetPos1));

	spotLightPosEye2 = glm::vec3(19.5422f, 17.3478f, 6.1928f);
	lightLoc = glGetUniformLocation(myCustomShader.shaderProgram, "spotLightPosEye2");
	// send light dir to shader
	glUniform3fv(lightLoc, 1, glm::value_ptr(spotLightPosEye2));

	targetPos2 = glm::vec3(19.3455f, 0.982332f, 6.37494f);
	lightLoc = glGetUniformLocation(myCustomShader.shaderProgram, "targetPos2");
	// send light dir to shader
	glUniform3fv(lightLoc, 1, glm::value_ptr(targetPos2));

	spotLightPosEye3 = glm::vec3(49.6468f, 17.2515f, -0.261957f);
	lightLoc = glGetUniformLocation(myCustomShader.shaderProgram, "spotLightPosEye3");
	// send light dir to shader
	glUniform3fv(lightLoc, 1, glm::value_ptr(spotLightPosEye3));

	targetPos3 = glm::vec3(49.8276f, 0.982332f, -0.474626f);
	lightLoc = glGetUniformLocation(myCustomShader.shaderProgram, "targetPos3");
	// send light dir to shader
	glUniform3fv(lightLoc, 1, glm::value_ptr(targetPos3));
	//3 stalpi
	
	//punctiforma
	lightPosEyeWaterTower = glm::vec3(-38.987f, 16.6361f, 16.8943f); 
	lightLoc = glGetUniformLocation(myCustomShader.shaderProgram, "punctiformLightPosEyeWaterTower");
	// send light dir to shader
	glUniform3fv(lightLoc, 1, glm::value_ptr(lightPosEyeWaterTower));

	lightColorPunctiformWaterTower = glm::vec3(0.0f, 0.0f, 1.0f);
	lightColorPunctiformWaterTowerLoc = glGetUniformLocation(myCustomShader.shaderProgram, "punctiformLightColorWaterTower");
	// send light dir to shader
	glUniform3fv(lightColorPunctiformWaterTowerLoc, 1, glm::value_ptr(lightColorPunctiformWaterTower));

	isNightLoc = glGetUniformLocation(myCustomShader.shaderProgram, "isNight");
	glUniform1f(isNightLoc, isNight);
}

void initFBO() {
	//TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
	//generate FBO ID
	
	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

glm::mat4 computeLightSpaceTrMatrix() {
	glm::mat4 lightView = glm::lookAt(glm::mat3(lightRotation) * lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const GLfloat near_plane = -10.1f, far_plane = 600.0f;
	glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

	return lightSpaceTrMatrix;


}
void renderElice(gps::Shader shader) {
	shader.useShaderProgram();

	angleElice += 5.0f;
	glm::vec3 initialPositionElice = glm::vec3(76.6565f, 18.2695f, 17.3324f);
	modelElice = glm::translate(glm::mat4(1.0f), initialPositionElice);
	modelElice = glm::rotate(modelElice, glm::radians(angleElice), glm::vec3(1.0f, 0.0f, 0.0f));
	modelElice = glm::translate(modelElice, -initialPositionElice);
	glUniformMatrix4fv(modelLocElice, 1, GL_FALSE, glm::value_ptr(modelElice));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	elice.Draw(shader);
}
void renderpd(gps::Shader shader) {
	shader.useShaderProgram();
	if (open) {
		anglePD = 70.0f;
	}
	glm::vec3 initialPositionPD = glm::vec3(112.42f, 20.791f, 3.00947f);
	modelPD = glm::translate(glm::mat4(1.0f), initialPositionPD);
	modelPD = glm::rotate(modelPD, glm::radians(anglePD), glm::vec3(0.0f, 1.0f, 0.0f));
	modelPD = glm::translate(modelPD, -initialPositionPD);
	glUniformMatrix4fv(modelLocPD, 1, GL_FALSE, glm::value_ptr(modelPD));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	poartaD.Draw(shader);
}
void renderBird(gps::Shader shader) {
	shader.useShaderProgram();

	birdMovementOffset += birdMovementSpeed;

	// Check if the bird reached the boundaries
	if (birdMovementOffset >= 140.0f || birdMovementOffset <= -17.32672f) {
		// Reverse the direction
		birdMovementSpeed *= -1;

		// Rotate the bird around its own center (initial position)
		modelBird = glm::translate(glm::mat4(1.0f), initialPositionBird);
		modelBird = glm::rotate(modelBird, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelBird = glm::translate(modelBird, -initialPositionBird);
	}
	else {
		// Move the bird along the x-axis without rotation
		modelBird = glm::translate(glm::mat4(1.0f), glm::vec3(birdMovementOffset, initialPositionBird.y, initialPositionBird.z));
	}

	// Set the model matrix in the shader
	glUniformMatrix4fv(modelLocBird, 1, GL_FALSE, glm::value_ptr(modelBird));

	// Calculate and set the normal matrix
	normalMatrixBird = glm::mat3(glm::inverseTranspose(view * modelBird));
	glUniformMatrix3fv(normalMatrixLocBird, 1, GL_FALSE, glm::value_ptr(normalMatrixBird));

	// Draw the bird
	bird.Draw(shader);
}

void renderRain(gps::Shader shader) {
	raining();

	shader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
	model =
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, r1, 0.0f)) *
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	rain.Draw(shader);

	if (rAux) {
		model = glm::mat4(1.0f);
		modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
		model =
			glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, r2, 0.0f)) *
			glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		rain.Draw(shader);
	}
}
void drawObjects(gps::Shader shader, bool depthPass) {

	shader.useShaderProgram();

	model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	scena.Draw(shader);
	renderElice(myCustomShader);
	renderBird(myCustomShader);
	renderpd(myCustomShader);
	if (itsRaining) {
		renderRain(myCustomShader);
	}

}

void renderScene() {

	// depth maps creation pass
	//TODO - Send the light-space transformation matrix to the depth map creation shader and
	//		 render the scene in the depth map
	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	drawObjects(depthMapShader, true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// render depth map on screen - toggled with the M key

	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {

		// final scene rendering pass (with shadows)

		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myCustomShader.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		drawObjects(myCustomShader, false);

		//draw a white cube around the light

		lightShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		model = glm::translate(model, 1.0f * lightDir);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		//lightCube.Draw(lightShader);
		
	}
	mySkyBox.Draw(skyboxShader, view, projection);
	
}

void cleanup() {
	glDeleteTextures(1, &depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char* argv[]) {


	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initSkyBox();
	initShaders();
	glGenFramebuffers(1, &shadowMapFBO);
	initFBO();
	initUniforms();

	glCheckError();

	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
