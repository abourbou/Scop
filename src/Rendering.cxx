
#include "Rendering.hpp"

GLFWwindow*	init_openGL()
{
	GLFWwindow*	window;

	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(1);
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	if (!(window = glfwCreateWindow(1024, 768, "Scop", NULL, NULL)))
	{
		std::cerr << "Failed to open GLFW window" << std::endl;
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		glfwTerminate();
		exit(1);
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	return window;
}

void	loopDraw(GLFWwindow* window, const Obj &obj)
{
	// Create VOA for vertex
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("shaders/TiltVertexShader.vertexshader", "shaders/TiltFragmentShader.fragmentshader");

	// Bind our list of triangle
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj.vecTriangle.size(), obj.vecTriangle.data(), GL_STATIC_DRAW);

	// Create VOA for normal
	GLuint NormalArrayID;
	glGenVertexArrays(1, &NormalArrayID);
	glBindVertexArray(NormalArrayID);

	// Bind list of normal
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj.vecNormal.size(), obj.vecNormal.data(), GL_STATIC_DRAW);

	// Create View and Projection matrices
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	auto Projection = PerspectiveProj(45.f, 4.f/3.f, 0.1f, 100.f);

	auto viewDist = (obj.maxDistCenter + obj.meanDistCenter) / 2.f;
	auto View = ViewMatrix(Vector3<GLfloat>{{viewDist * 4.f,0,0}},
						   obj.centerPoint,
						   Vector3<GLfloat>{{0,1,0}});

	std::chrono::milliseconds start_time = std::chrono::duration_cast<std::chrono::milliseconds>(
						std::chrono::system_clock::now().time_since_epoch());

	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Use our shader
		glUseProgram(programID);

		// Make the object rotate around himself
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
							std::chrono::system_clock::now().time_since_epoch());
		GLfloat rotPerMs = 0.036;
		GLfloat angle = -10. + ((ms.count() - start_time.count()) % 10000) * rotPerMs;

		auto Model = Translation(obj.centerPoint[0], obj.centerPoint[1], obj.centerPoint[2])
				     * Ry(angle)
				     * Translation(-obj.centerPoint[0], -obj.centerPoint[1], -obj.centerPoint[2]);

		auto MVP = Projection * View * Model;

		// openGL use column major matrix so we need to transpose final matrix
		auto t_MVP = transpose(MVP);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &t_MVP(0,0));

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(VertexArrayID - 1);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			VertexArrayID - 1,  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : normal
		glEnableVertexAttribArray(NormalArrayID - 1);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			NormalArrayID - 1,  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glDrawArrays(GL_TRIANGLES, 0, obj.vecTriangle.size() / 3);
		glDisableVertexAttribArray(VertexArrayID - 1);
		glDisableVertexAttribArray(NormalArrayID - 1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

void	rendering(Obj& obj)
{
	GLFWwindow*	window = init_openGL();
	loopDraw(window, obj);
}