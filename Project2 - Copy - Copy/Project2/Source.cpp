/*
* Program name: Assignment 1 (Real Time Rendering)
* Written by: Ben Thompson
* Date:12/10/2016
* 

SINCE GLFW DOESN'T SUPPORT WRITING TEXT TO SCREEN EXTRA LIBRARIES WERE REQUIRED
WHICH AT THIS POINT ARE NOT FULLY IMPLEMENTED... THE USER INPUT CONTROLS ARE AS 
FOLLOWS

CONTROLS:

USING NUMBER TO SWITCH BETWEEN SHADERS
1: VERTEX MANIPULATION WITH AMBIENT,DIFFUSE AND SPECULAR LIGHTING
2: TOON SHADER
3: BLINN-PHONG

ROTATING OBJECT:
P: (DO SUPERMAN THING) SPEEDS UP RIGHT ON REALEASE
O: (DO SUPERMAN THING) SPEEDS UP LEFT ON REALEASE

FIRST PERSON CAMERA MOVEMENT FROM HTTP://LEARNOPENGL.COM (SEE CAMERA INTERNAL HEADER FILE) 
W A D S AND MOUSE MOVEMENT

shader class: HTTP://LEARNOPENGL.COM 

*/

#include <vector>
#include <iostream>
#include <cmath>
#include <string>

#include <algorithm>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libraries
//Texture Library 
#include <SOIL.h>

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



/*
* To implement Text printing (half implemented)
* */
// FreeType Headers
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

// for text 
#include <GL/freeglut.h>

//ftgl
#include <FTGL/ftgl.h>

// Internal header files
#include "Shader.h"
#include "Camera.h"
#include "Teapot.h"



// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();
void set_rotation();
void choose_lighting();
void drawtext(const char *text, int length, int x, int y);//(has bug)
void RenderQuad();
GLuint loadTexture(GLchar* path);
GLuint loadCubemap(std::vector<const GLchar*> faces);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Camera
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool    keys[1024];

// Light attributes
glm::vec3 lightPos(1.0f, 1.0f, 2.0f);

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

//rotation variable
GLfloat rotateSpeed = 45.0f;

//chosing light
GLint lightingChoice=2;

/*
*Shader Options
*/


//Specular Ambient Diffuse options(lighting1)
GLint specValue = 256;
GLfloat ambientStrength = 0.5f;
GLfloat specularStrength = 0.5f;
//Toon Shader (lighting2)


//Blinn Shader options (lighting3)
GLboolean blinn = true;


//string for displaying Shader name
std::string outputText;

// The MAIN function, from here we start the application and run the game loop
int main()
{
	
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "TeapotOpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);

	
	// Build and compile our shader programs
	Shader lighting1("shaders/1.vs", "shaders/1.frag");
	Shader lighting2("shaders/rr.vs", "shaders/rr.frag");
	Shader lighting3("shaders/lighting3.vs", "shaders/lighting3.frag");

	Shader skyboxShader("shaders/skybox.vs", "shaders/skybox.frag");

	//learnopengl.com shader
	Shader lampShader("shaders/lamp.vs", "shaders/lamp.frag");

	//normal map
	Shader normalMapShader("shaders/normal_mapping.vs", "shaders/normal_mapping.frag");



	/*
	* Defining object (teapot)
	*/
	GLuint teapotVAO,teapot_vbo, normals_teapot_vbo;
	glGenBuffers(1, &teapot_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, teapot_vbo);


	// header file vertex points into vbo 
	glBufferData(GL_ARRAY_BUFFER, sizeof(teapot_vertex_points),
		teapot_vertex_points, GL_STATIC_DRAW);

	//Getting the normals
	glGenBuffers(1, &normals_teapot_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, normals_teapot_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(teapot_normals),
		teapot_normals, GL_STATIC_DRAW);
	
	//texture 
	GLuint texcoords_vbo;
	glGenBuffers(1, &texcoords_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, texcoords_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(teapot_tex_coords),teapot_tex_coords, GL_STATIC_DRAW);

	//vertex array VAO
	glGenVertexArrays(1, &teapotVAO);
	glBindVertexArray(teapotVAO);
	glEnableVertexAttribArray(0);
	
	//vbo array structure
	glBindBuffer(GL_ARRAY_BUFFER, teapot_vbo);
	glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 0,NULL);
	glEnableVertexAttribArray(1);
	
	//normals array structure
	glBindBuffer(GL_ARRAY_BUFFER, normals_teapot_vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	//textures array structure
	glEnableVertexAttribArray(texcoords_vbo);
	glVertexAttribPointer(texcoords_vbo, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(3);
	glBindVertexArray(0);


	///
	/// Vertices 
	///

	//skybox
	GLfloat skyboxVertices[] = {
		// Positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	//Cube for lamp
	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};


	///
	/// Lamps VAO and VBO
	///
	GLuint VBO , lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(lightVAO);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);



	///
	/// Skybox VAO and VBO
	///
	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);



	///
	/// Cubemap (Skybox)
	///

	std::vector<const GLchar*> faces;
	/*
	faces.push_back("skybox/borg_rt.jpg");
	faces.push_back("skybox/borg_lf.jpg");
	faces.push_back("skybox/borg_up.jpg");
	faces.push_back("skybox/borg_dn.jpg");
	faces.push_back("skybox/borg_bk.jpg");
	faces.push_back("skybox/borg_ft.jpg");
	*/

	faces.push_back("skybox/lake1_rt.jpg");
	faces.push_back("skybox/lake1_lf.jpg");
	faces.push_back("skybox/lake1_up.jpg");
	faces.push_back("skybox/lake1_dn.jpg");
	faces.push_back("skybox/lake1_bk.jpg");
	faces.push_back("skybox/lake1_ft.jpg");
	GLuint cubemapTexture = loadCubemap(faces);

	///
	/// Load textures
	///
	GLuint diffuseMap = loadTexture("textures/brickwall.jpg");
	GLuint normalMap = loadTexture("textures/brickwall_normal.jpg");

	///
	/// Set texture units for normal map
	///
	normalMapShader.Use();
	glUniform1i(glGetUniformLocation(normalMapShader.Program, "diffuseMap"), 0);
	glUniform1i(glGetUniformLocation(normalMapShader.Program, "normalMap"), 1);


	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		do_movement();
		set_rotation();
		choose_lighting();

		// Clear the colorbuffer
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		// Configure view/projection matrices
		normalMapShader.Use();
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(normalMapShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(normalMapShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		// Render normal-mapped quad
		glm::mat4 model2;
	//	model2 = glm::rotate(model2, (GLfloat)glfwGetTime() * -10, glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // Rotates the quad to show normal mapping works in all directions
		
		glUniformMatrix4fv(glGetUniformLocation(normalMapShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model2));
		glUniform3fv(glGetUniformLocation(normalMapShader.Program, "lightPos"), 1, &lightPos[0]);
		glUniform3fv(glGetUniformLocation(normalMapShader.Program, "viewPos"), 1, &camera.Position[0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
		RenderQuad();


		// Draw skybox first
		glDepthMask(GL_FALSE);// Remember to turn depth writing off
		skyboxShader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		projection = glm::perspective(camera.Zoom, (float)WIDTH / (float)HEIGHT , 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(skyboxShader.Program, "skybox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);



		//attempt at using FTGL library (bug)
		//FTGLPixmapFont font("Users\BenTh\Desktop\Fonts\arial.ttf");
		//font.Render("Hello World");

		// Use cooresponding shader when setting uniforms/drawing objects
		GLint objectColorLoc;
		GLint lightColorLoc;
		GLint lightPosLoc;
		GLint viewPosLoc;

//		glm::mat4 view;
//		glm::mat4 projection;

		GLint modelLoc;
		GLint viewLoc;
		GLint projLoc;



		
		switch (lightingChoice)
		{
			case 1:
			{
				lighting1.Use();
//				glActiveTexture(GL_TEXTURE0);
//				glBindTexture(GL_TEXTURE_2D, texture1);
//				glUniform1i(glGetUniformLocation(lighting1.Program, "ourTexture1"), 0);


			

				//local Object colour
				objectColorLoc = glGetUniformLocation(lighting1.Program, "objectColor");
				//local Light Color 
				lightColorLoc = glGetUniformLocation(lighting1.Program, "lightColor");
				//local light Position
				lightPosLoc = glGetUniformLocation(lighting1.Program, "lightPos");
				//local view Position
				viewPosLoc = glGetUniformLocation(lighting1.Program, "viewPos");
				

				//setting light Position
				glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
				//setting object colour
				glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
				//setting light colour
				glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
				//setting view(cammera) position 
				glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

				// Create camera transformations
				view = camera.GetViewMatrix();
				projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
				// Get the uniform matrixes
				modelLoc = glGetUniformLocation(lighting1.Program, "model");
				viewLoc = glGetUniformLocation(lighting1.Program, "view");
				projLoc = glGetUniformLocation(lighting1.Program, "projection");
				
				// Pass the matrices for view and projection
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

				//setting option varibales for lighting 1
				glUniform1i(glGetUniformLocation(lighting1.Program, "SpecularValue"), specValue);
				glUniform1f(glGetUniformLocation(lighting1.Program, "ambientStrength"), ambientStrength);
				glUniform1f(glGetUniformLocation(lighting1.Program, "specularStrength"), specularStrength);

				break;
			}
			case 2:
			{
				lighting2.Use();
				

				//local Object colour
				objectColorLoc = glGetUniformLocation(lighting2.Program, "objectColor");
				//local Light Color 
				lightColorLoc = glGetUniformLocation(lighting2.Program, "lightColor");
				//local light Position
				lightPosLoc = glGetUniformLocation(lighting2.Program, "lightPos");
				//local view Position
				viewPosLoc = glGetUniformLocation(lighting2.Program, "cameraPos");

				//setting light Position
				glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
				//setting object colour
				glUniform3f(objectColorLoc, .7f, .7f, .7f);
				//setting light colour
				glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
				//setting view(cammera) position 
				glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

				// Create camera transformations
				view = camera.GetViewMatrix();
				projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);



				// Get the uniform locations
				modelLoc = glGetUniformLocation(lighting2.Program, "model");
				viewLoc = glGetUniformLocation(lighting2.Program, "view");
				projLoc = glGetUniformLocation(lighting2.Program, "projection");
				// Pass the matrices to the shader
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
				break;
			}
			case 3:
			{

				// Draw objects
				lighting3.Use();


				//local Object colour
				objectColorLoc = glGetUniformLocation(lighting3.Program, "objectColor");
				//local Light Color 
				lightColorLoc = glGetUniformLocation(lighting3.Program, "lightColor");
				//local light Position
				lightPosLoc = glGetUniformLocation(lighting3.Program, "lightPos");
				//local view Position
				viewPosLoc = glGetUniformLocation(lighting3.Program, "viewPos");

			
				//setting light Position
				glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
				//setting object colour
				glUniform3f(objectColorLoc, .8f, .8f, .8f);
				//setting light colour
				glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
				//setting view(cammera) position 
				glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);



				// Create camera transformations
				view = camera.GetViewMatrix();
				projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
				

				// Get the uniform locations
				modelLoc = glGetUniformLocation(lighting3.Program, "model");
				viewLoc = glGetUniformLocation(lighting3.Program, "view");
				projLoc = glGetUniformLocation(lighting3.Program, "projection");
				// Pass the matrices to the shader
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
				
			
				//if blinn true or false
				glUniform1i(glGetUniformLocation(lighting3.Program, "blinn"), blinn);
			

				break;
			}
		}

		
		//draw teapot
	
		
		glBindVertexArray(teapotVAO);
		glm::mat4 model;
		model = glm::rotate(model, (GLfloat)glfwGetTime() * glm::radians(rotateSpeed), glm::vec3(0.0, 1.0, 0.0));
		model = glm::scale(model, glm::vec3(0.1f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, teapot_vertex_count);
		glBindVertexArray(0);
		
		//Draw the lamp object, again binding the appropriate shader
/*		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");
		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		
		// Draw the light object (using light's vertex attributes)
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		*/


		//draw text (bug)
		/* 
		glColor3f(.1f, .1f, .1f);
	    drawtext(outputText.data(), outputText.size(), 0, 0);
		*/

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}



// RenderQuad() Renders a 1x1 quad in NDC
GLuint quadVAO = 0;
GLuint quadVBO;
void RenderQuad()
{
	if (quadVAO == 0)
	{
		// positions
		glm::vec3 pos1(-1.0, 1.0, 0.0);
		glm::vec3 pos2(-1.0, -1.0, 0.0);
		glm::vec3 pos3(1.0, -1.0, 0.0);
		glm::vec3 pos4(1.0, 1.0, 0.0);
		// texture coordinates
		glm::vec2 uv1(0.0, 1.0);
		glm::vec2 uv2(0.0, 0.0);
		glm::vec2 uv3(1.0, 0.0);
		glm::vec2 uv4(1.0, 1.0);
		// normal vector
		glm::vec3 nm(0.0, 0.0, 1.0);

		// calculate tangent/bitangent vectors of both triangles
		glm::vec3 tangent1, bitangent1;
		glm::vec3 tangent2, bitangent2;
		// - triangle 1
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent1 = glm::normalize(tangent1);

		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent1 = glm::normalize(bitangent1);

		// - triangle 2
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent2 = glm::normalize(tangent2);


		bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent2 = glm::normalize(bitangent2);


		GLfloat quadVertices[] = {
			// Positions            // normal         // TexCoords  // Tangent                          // Bitangent
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(11 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void do_movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}


void set_rotation() {
	if (keys[GLFW_KEY_P])
		rotateSpeed += 0.1f;
	if (keys[GLFW_KEY_O])
		rotateSpeed -= 0.1f;
}

void choose_lighting() {

	if (keys[GLFW_KEY_1])
		lightingChoice = 1;
	if (keys[GLFW_KEY_2])
		lightingChoice = 2;
	if (keys[GLFW_KEY_3])
		lightingChoice = 3;
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void drawtext(const char *text, int length, int x, int y)
{
	glMatrixMode(GL_PROJECTION);
	double *matrix = new double[16];
	glGetDoublev(GL_PROJECTION_MATRIX, matrix);
	glLoadIdentity();
	glOrtho(0, HEIGHT, 0, WIDTH, -5, 5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(x,y);
	for (int i = 0; i < length; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)text[i]);

	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matrix);
	glMatrixMode(GL_MODELVIEW);

}

GLuint loadCubemap(std::vector<const GLchar*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}


// This function loads a texture from file. Note: texture loading functions like these are usually 
// managed by a 'Resource Manager' that manages all resources (like textures, models, audio). 
// For learning purposes we'll just define it as a utility function.
GLuint loadTexture(GLchar* path)
{
	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}
