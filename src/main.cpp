//GLEW
#define GLEW_STATIC
#include <GL\glew.h>
//GLFW
#include <GLFW\glfw3.h>
#include <iostream>
#include "myShader.h"
#include "camara.h"
#include "object.h" 
//para textura
#include <SOIL.h>
//para vector
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

using namespace glm;
using namespace std;
const GLint WIDTH = 800, HEIGHT = 800;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
vec3 movement;
GLfloat radiansX,radiansY;
GLint LightOption=1;
Camera myCamera({ 0,0,3 }, { 0,0,-1 }, 0.05, 45);

void MouseScroll(GLFWwindow* window, double xScroll, double yScroll) {
	myCamera.MouseScroll(window, xScroll, yScroll);
};

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

int main() {
	
	//initGLFW
	if (!glfwInit())
		exit(EXIT_FAILURE);

	//set GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	//create a window
	GLFWwindow* window;
	window = glfwCreateWindow(WIDTH, HEIGHT, "Primera ventana", nullptr, nullptr);
	if (!window) {
		cout << "Error al crear la ventana" << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	//set GLEW and inicializate
	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit()) {
		cout << "Error al inicializar glew" << endl;
		glfwTerminate();
		return NULL;
	}

	//set function when callback
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, MouseScroll);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//GLFW_CURSOR_DISABLED

	//set windows and viewport
	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
	glViewport(0, 0, screenWidth, screenHeight);

	//fondo	
	
	//cargamos los shader
	glEnable(GL_DEPTH_TEST);
	Shader ReceiveShader("./src/ReceiveVertex.vertexshader", "./src/ReceiveFragment.fragmentshader");

	Shader DireccionShader("./src/DirVertex.vertexshader", "./src/DirFragment.fragmentshader");
	Shader PointShader("./src/PointVertex.vertexshader", "./src/PointFragment.fragmentshader");
	Shader FocalShader("./src/FocalVertex.vertexshader", "./src/FocalFragment.fragmentshader");

	
	Object cubA({ 0.3f,0.3f,0.3f }, { 0.f,0.f,0.0f }, { 0.f,0.3f,0.1f }, Object::cube);
	Object cubB({ 0.1,0.1,0.1 }, { -1.f,0.f,0.0f }, { 0.f,-0.3f,0.5f }, Object::cube);

	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
				
		//Establecer el color de fondo
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//establecer el shader
		
		GLint objectColorLoc, lightColorLoc, lightPosLoc, viewPosLoc;
		GLint lightDirPos;
		mat4 proj;		mat4 view;		mat4 model;
		GLint modelLoc, viewLoc, projectionLoc;
//////////////////////////////////////////////////LIGHTS-DIRECCIONAL///////////////////////////////////////////////////
		if (LightOption ==1){
			DireccionShader.USE();
			objectColorLoc = glGetUniformLocation(DireccionShader.Program, "objectColor");
			lightColorLoc = glGetUniformLocation(DireccionShader.Program, "lightColor");
			lightPosLoc = glGetUniformLocation(DireccionShader.Program, "lightPos");
			viewPosLoc = glGetUniformLocation(DireccionShader.Program, "viewPos");
				
			glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.0f);
			glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);

			//direccional
			lightDirPos = glGetUniformLocation(DireccionShader.Program, "Ldirection");
			glUniform3f(lightDirPos, -0.2f, -1.0f, -0.3f);
			glUniform3f(lightPosLoc, cubB.GetPosition().x, cubB.GetPosition().y, cubB.GetPosition().z);
			glUniform3f(viewPosLoc, myCamera.cameraPos.x, myCamera.cameraPos.y, myCamera.cameraPos.z);
		
			proj = perspective(radians(myCamera.GetFOV()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);
			myCamera.DoMovement(window);
			view = myCamera.LookAt();

			// Set lights properties
			glUniform3f(glGetUniformLocation(DireccionShader.Program, "Lambient"), 0.2f, 0.2f, 0.2f);
			glUniform3f(glGetUniformLocation(DireccionShader.Program, "Ldiffuse"), 0.5f, 0.5f, 0.5f);
			glUniform3f(glGetUniformLocation(DireccionShader.Program, "Lspecular"), 1.0f, 1.0f, 1.0f);

			cubA.Rotate(radiansX, radiansY);
			cubA.Move(movement);
			model = glm::translate(model, cubA.GetPosition());
			model = cubA.GetModelMatrix();
		
			modelLoc = glGetUniformLocation(DireccionShader.Program,"model");	
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
			viewLoc = glGetUniformLocation(DireccionShader.Program, "view");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
			projectionLoc = glGetUniformLocation(DireccionShader.Program, "projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(proj));

			//pintar el VAO
			cubA.Draw();
		}
//////////////////////////////////////////////////LIGHTS-PUNTUAL///////////////////////////////////////////////////
		if (LightOption == 2) {
			PointShader.USE();
			objectColorLoc = glGetUniformLocation(PointShader.Program, "objectColor");
			lightColorLoc = glGetUniformLocation(PointShader.Program, "lightColor");
			lightPosLoc = glGetUniformLocation(PointShader.Program, "lightPos");
			viewPosLoc = glGetUniformLocation(PointShader.Program, "viewPos");

			glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.0f);
			glUniform3f(lightColorLoc, 1.0f, 1.0f, 4.0f);

			//Puntual
			glUniform3f(lightPosLoc, cubB.GetPosition().x, cubB.GetPosition().y, cubB.GetPosition().z);
			glUniform3f(viewPosLoc, myCamera.cameraPos.x, myCamera.cameraPos.y, myCamera.cameraPos.z);

			proj = perspective(radians(myCamera.GetFOV()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);
			myCamera.DoMovement(window);
			view = myCamera.LookAt();

			// Set lights properties
			glUniform3f(glGetUniformLocation(PointShader.Program, "Lambient"), 0.2f, 0.2f, 0.2f);
			glUniform3f(glGetUniformLocation(PointShader.Program, "Ldiffuse"), 0.5f, 0.5f, 0.5f);
			glUniform3f(glGetUniformLocation(PointShader.Program, "Lspecular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(PointShader.Program, "Lconstant"), 1.0f);
			glUniform1f(glGetUniformLocation(PointShader.Program, "Llinear"), 0.09);
			glUniform1f(glGetUniformLocation(PointShader.Program, "Lquadratic"), 0.032);

			cubA.Rotate(radiansX, radiansY);
			cubA.Move(movement);
			model = glm::translate(model, cubA.GetPosition());
			model = cubA.GetModelMatrix();

			modelLoc = glGetUniformLocation(PointShader.Program, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
			viewLoc = glGetUniformLocation(PointShader.Program, "view");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
			projectionLoc = glGetUniformLocation(PointShader.Program, "projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(proj));

			//pintar el VAO
			cubA.Draw();
		}
//////////////////////////////////////////////////LIGHTS-FOCAL///////////////////////////////////////////////////
		if (LightOption == 3) {
			FocalShader.USE();
			objectColorLoc = glGetUniformLocation(FocalShader.Program, "objectColor");
			lightColorLoc = glGetUniformLocation(FocalShader.Program, "lightColor");
			lightPosLoc = glGetUniformLocation(FocalShader.Program, "lightPos");
			viewPosLoc = glGetUniformLocation(FocalShader.Program, "viewPos");
			lightDirPos = glGetUniformLocation(FocalShader.Program, "Ldirection");
			GLint lightSpotCutOffLoc = glGetUniformLocation(FocalShader.Program, "LcutOff");
			GLint lightSpotOuterCutOffLoc = glGetUniformLocation(FocalShader.Program, "LouterCutOff");
			
			glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.0f);
			glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
			glUniform1f(lightSpotCutOffLoc, glm::cos(glm::radians(45.5f)));
			glUniform1f(lightSpotOuterCutOffLoc, glm::cos(glm::radians(60.5f)));
			//glUniform3f(lightDirPos, myCamera.cameraFront.x, myCamera.cameraFront.y, myCamera.cameraFront.z);
			glUniform3f(lightDirPos, 0, 0, -1);

			//Puntual
			glUniform3f(lightPosLoc, cubB.GetPosition().x, cubB.GetPosition().y, cubB.GetPosition().z);
			glUniform3f(viewPosLoc, myCamera.cameraPos.x, myCamera.cameraPos.y, myCamera.cameraPos.z);

			proj = perspective(radians(myCamera.GetFOV()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);
			myCamera.DoMovement(window);
			view = myCamera.LookAt();

			// Set lights properties
			glUniform3f(glGetUniformLocation(FocalShader.Program, "Lambient"), 0.1f, 0.1f, 0.1f);
			glUniform3f(glGetUniformLocation(FocalShader.Program, "Ldiffuse"), 0.8f, 0.8f, 0.8f);
			glUniform3f(glGetUniformLocation(FocalShader.Program, "Lspecular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(FocalShader.Program, "Lconstant"), 1.0f);
			glUniform1f(glGetUniformLocation(FocalShader.Program, "Llinear"), 0.09);
			glUniform1f(glGetUniformLocation(FocalShader.Program, "Lquadratic"), 0.032);

			cubA.Rotate(radiansX, radiansY);
			cubA.Move(movement);
			model = glm::translate(model, cubA.GetPosition());
			model = cubA.GetModelMatrix();

			modelLoc = glGetUniformLocation(FocalShader.Program, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
			viewLoc = glGetUniformLocation(FocalShader.Program, "view");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
			projectionLoc = glGetUniformLocation(FocalShader.Program, "projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(proj));

			//pintar el VAO
			cubA.Draw();
		}

////////////////////////////////////////////////ReceiveCub///////////////////////////////////////////////////////
		ReceiveShader.USE();
		
		model = cubB.GetModelMatrix();

		modelLoc = glGetUniformLocation(ReceiveShader.Program, "model");

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
		viewLoc = glGetUniformLocation(ReceiveShader.Program, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
		projectionLoc = glGetUniformLocation(ReceiveShader.Program, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(proj));

		//pintar el VAO
		cubB.Draw();
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	// liberar la memoria de los VAO, EBO y VBO
	cubA.Delete();
	cubB.Delete();

	// Terminate GLFW, clearing any resources allocated by GLFW.
	exit(EXIT_SUCCESS);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	
	if (key == GLFW_KEY_KP_4)
		radiansX -= 0.5;
	if (key == GLFW_KEY_KP_6)
		radiansX += 0.5;
	if (key == GLFW_KEY_KP_8)
		radiansY -= 0.5;
	if (key == GLFW_KEY_KP_2)
		radiansY += 0.5;
	
	if (key == GLFW_KEY_LEFT )
		movement.x -= 0.05;
	if (key == GLFW_KEY_RIGHT)
		movement.x += 0.05;
	if (key == GLFW_KEY_UP )
		movement.y += 0.05;
	if (key == GLFW_KEY_DOWN )
		movement.y -= 0.05;

	if (key == GLFW_KEY_1)
		LightOption = 1;
	if (key == GLFW_KEY_2)
		LightOption = 2;
	if (key == GLFW_KEY_3)
		LightOption = 3;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	myCamera.MouseMove(window, xpos, ypos);
};
