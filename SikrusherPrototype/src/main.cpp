#include <glad.h>
#include <glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

unsigned initVertexShader();
unsigned initFragmentShader(const char* fragmentShader);

// Generic functions that show general flow
void drawObjectVBO(unsigned VBO, unsigned shaderProgram, float* vertices, int verticesLength);
void drawObjVAO(unsigned VAO, unsigned VBO, unsigned shaderProgram, float* vertices, int verticesLength);

const char* vertexShaderSource = 
								"#version 460 core\n"
								"layout (location = 0) in vec3 aPos;\n"
								"void main()\n"
								"{\n"
								"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
								"}\0";

const char* fragmentShaderSourceOrnage = 
										"#version 460 core\n"
									    "out vec4 FragColor;\n"
								        "void main()\n"
								        "{\n"
								        "	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
								        "}\0";

const char* fragmentShaderSourcePurple = 
										"#version 460 core\n"
										"out vec4 FragColor;\n"
										"void main()\n"
										"{\n"
										"	FragColor = vec4(0.5f, 0.0f, 0.5f, 1.0f);\n"
										"}\0";

struct vec3
{
	float x, y, z;
};

class Rectangle
{
public:
	Rectangle(vec3 upRight, vec3 bottomRight, vec3 bottomLeft, vec3 upLeft)
	{
		float vertices[12] =
		{
			upRight.x, upRight.y, upRight.z,
			bottomRight.x, bottomRight.y, bottomRight.z,
			bottomLeft.x, bottomLeft.y, bottomLeft.z,
			upLeft.x, upLeft.y, upLeft.z
		};

		unsigned indices[6] =
		{
			0, 1, 3,
			1, 2, 3
		};

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glGenBuffers(1, &EBO);
		 
		// Put vertices on the gpu
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// Put the indices on the gpu
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// Inform openGL on how to read the vetex's
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);
		glEnableVertexAttribArray(0);
	}

	~Rectangle()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}
	void draw(unsigned shaderProgram)
	{
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}


private:
	unsigned VAO, VBO, EBO;
};


int main(int argc, char** argv)
{
	//--------------------------------------------------------------------
	// Init
	
	// Start GLFW and tell it what version of openGL to use
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a window and set it as teh contex
	GLFWwindow* window = glfwCreateWindow(800, 800, "Sikrusher", NULL, NULL);
	if (window == nullptr)
	{
		std::cout << "Window failed to create" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Init GLAD - GLAD handles openGL function pointer retival 
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to init GLAD" << std::endl;
	}

	// Tell openGL size of the window
	glViewport(0, 0, 800, 800);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//--------------------------------------------------------------------
	// Create shaders and upload them to the GPU

	// Compile shaders
	unsigned vertexShader = initVertexShader();
	unsigned fragmentShaderOrange = initFragmentShader(fragmentShaderSourceOrnage);
	unsigned fragmentShaderPurple = initFragmentShader(fragmentShaderSourcePurple);

	// Link the shaders
	unsigned shaderProgramOrnage;
	shaderProgramOrnage = glCreateProgram();

	glAttachShader(shaderProgramOrnage, vertexShader);
	glAttachShader(shaderProgramOrnage, fragmentShaderOrange);
	glLinkProgram(shaderProgramOrnage);

	int success;
	glGetProgramiv(shaderProgramOrnage, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(shaderProgramOrnage, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::LINKING_FAILED\n" << infoLog << std::endl;
	}

	unsigned shaderProgramPurple;
	shaderProgramPurple = glCreateProgram();

	glAttachShader(shaderProgramPurple, vertexShader);
	glAttachShader(shaderProgramPurple, fragmentShaderPurple);
	glLinkProgram(shaderProgramPurple);

	glGetProgramiv(shaderProgramPurple, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(shaderProgramPurple, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// No longer need the shaders in local memory
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShaderOrange);
	glDeleteShader(fragmentShaderPurple);

	//--------------------------------------------------------------------
	// Load vertex to the gpu

	Rectangle tangle(
		vec3{ 0.5f,  0.5f, 0.0f }, 
		vec3{ 0.5f, -0.5f, 0.0f }, 
		vec3{ -0.5f, -0.5f, 0.0f }, 
		vec3{ -0.5f,  0.5f, 0.0f });

	Rectangle tangle2(
		vec3{ 1.0f,  1.0f, 0.0f },
		vec3{ 1.0f, 0.5f, 0.0f },
		vec3{ 0.5f, 0.5f, 0.0f },
		vec3{ 0.5f,  1.0f, 0.0f });

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw a triagnle
		tangle.draw(shaderProgramOrnage);
		tangle2.draw(shaderProgramPurple);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Delete buffers
	glDeleteProgram(shaderProgramOrnage);
	glDeleteProgram(shaderProgramPurple);

	glfwTerminate();

	return 0;
}

// Call back for when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

unsigned initVertexShader()
{
	// Create a vertex shader
	unsigned vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	// Verify that the shader compiled
	int success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	return vertexShader;
}

unsigned initFragmentShader(const char* fragmentShaderSoruce)
{
	unsigned fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSoruce, nullptr);
	glCompileShader(fragmentShader);

	int success;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	return fragmentShader;
}

// Generic fucntion to draw single vertice object
void drawObjectVBO(unsigned VBO, unsigned shaderProgram, float* vertices, int verticesLength)
{
	// 0. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesLength, vertices, GL_STATIC_DRAW);
	// 1. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 2. use our shader program when we want to render an object
	glUseProgram(shaderProgram);
	// 3. now draw the object 
	//someOpenGLFunctionThatDrawsOurTriangle();
}

void drawObjVAO(unsigned VAO, unsigned VBO, unsigned shaderProgram, float* vertices, int verticesLength)
{
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 3. then set our vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// ..:: Drawing code (in render loop) :: ..
	
	// 4. draw the object
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
//	someOpenGLFunctionThatDrawsOurTriangle();
}