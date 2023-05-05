
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

// TODO buttons for movements (and rotations ?)
// TODO bind key to texture mapping
// TODO smooth transitions between colors
// TODO better way to apply texture
// TODO only draw lines for the object



GLint	LoadTexture(std::string textureFile, std::string defaultTexture)
{
	GLuint Texture = 0;

	if (!textureFile.empty())
	{
		if (textureFile.size() < 5)
		{ PRINT_ERROR("Invalid texture, must be a .bmp or .dds extension"); }
		else
		{
			std::string extension = textureFile.substr(textureFile.size() - 4);
			if (extension == ".bmp")
				Texture = LoadBMP(textureFile.c_str());
			else if (extension == ".dds")
				Texture = LoadDDS(textureFile.c_str());
			else
			{ PRINT_ERROR("Invalid texture, must be a.bmp or.dds extension"); }
		}
	}
	if (Texture == 0)
	{
		PRINT_INFO("Use default texture");
		Texture = LoadBMP(defaultTexture.c_str());
	}
	return Texture;
}

void	loopDraw(GLFWwindow* window, const Obj &obj, std::string textureFile)
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

	Vector3f camPos = {{viewDist * 4.0f,0,0}};
	Vector3f centerView = {obj.centerPoint};

	auto View = ViewMatrix(camPos,
						   centerView,
						   Vector3<GLfloat>{{0,1,0}});

	// Create VOA for UV
	GLuint UVarrayID;
	glGenVertexArrays(1, &UVarrayID);
	glBindVertexArray(UVarrayID);

	// Bind list of UV
	GLuint	UVbuffer;
	glGenBuffers(1, &UVbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, UVbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj.vecUV.size(), obj.vecUV.data(), GL_STATIC_DRAW);

	// Load texture
	GLuint Texture = LoadTexture(textureFile, "./wavefront_obj/fire_camp.bmp");
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

	// Start timer main loop
	std::chrono::milliseconds start_time = std::chrono::duration_cast<std::chrono::milliseconds>(
						std::chrono::system_clock::now().time_since_epoch());

	std::chrono::milliseconds last_time = start_time;

	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Use our shader
		glUseProgram(programID);

		// Bind our texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to texture
		glUniform1i(TextureID, 0);

		// Movements
		std::chrono::milliseconds current_time = std::chrono::duration_cast<std::chrono::milliseconds>(
							std::chrono::system_clock::now().time_since_epoch());
		// Make the object rotate around himself
		GLfloat rotPerMs = 0.036;
		GLfloat angle = -10. + ((current_time.count() - start_time.count()) % 10000) * rotPerMs;

		auto Model = Translation(obj.centerPoint[0], obj.centerPoint[1], obj.centerPoint[2])
					 * Ry(angle)
					 * Translation(-obj.centerPoint[0], -obj.centerPoint[1], -obj.centerPoint[2]);

		// Translation
		GLfloat translSpeed = 0.005 * viewDist;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			translSpeed *= 2;
		auto deltaTime = current_time - last_time;

		static Vector3f headUp = {{0., 1., 0.}};
		auto direction = (centerView - camPos).normalize();
		auto right = direction.crossProduct(headUp);
		auto up = right.crossProduct(direction);

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS ||
			glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			camPos		+= translSpeed * up * (float)deltaTime.count();
			centerView	+= translSpeed * up * (float)deltaTime.count();
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS ||
			glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			camPos		-= translSpeed * up * (float)deltaTime.count();
			centerView	-= translSpeed * up * (float)deltaTime.count();
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS ||
			glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			camPos		-= translSpeed * right * (float)deltaTime.count();
			centerView	-= translSpeed * right * (float)deltaTime.count();
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS ||
			glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			camPos		+= translSpeed * right * (float)deltaTime.count();
			centerView	+= translSpeed * right * (float)deltaTime.count();
		}

		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			camPos		-= translSpeed * direction * (float)deltaTime.count();
			centerView	-= translSpeed * direction * (float)deltaTime.count();
		}

		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			camPos		+= translSpeed * direction * (float)deltaTime.count();
			centerView	+= translSpeed * direction * (float)deltaTime.count();
		}

		// auto delta_sec = (current_time.count() - start_time.count()) / 1000;
		// static auto count = delta_sec;
		// if (delta_sec > count)
		// {
		// 	std::cout << "camPos : " << transpose(camPos) << std::endl;
		// 	std::cout << "centerView : " << transpose(centerView) << std::endl;
		// 	std::cout << "center object : " << transpose(obj.centerPoint) << std::endl;
		// 	++count;
		// }
		View = ViewMatrix(camPos,
						  centerView,
						  Vector3<GLfloat>{{0,1,0}});


		auto MVP = Projection * View * Model;

		// openGL use column major matrix so we need to transpose final matrix
		auto t_MVP = transpose(MVP);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &t_MVP(0,0));

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(VertexArrayID - 1);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			VertexArrayID - 1,  // attribute 0.
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
			NormalArrayID - 1,  // attribute 1
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 3rd attribute buffer : texture coordinates
		glEnableVertexAttribArray(UVarrayID - 1);
		glBindBuffer(GL_ARRAY_BUFFER, UVbuffer);
		glVertexAttribPointer(
			UVarrayID - 1,		// attribute 2
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,
			0
		);

		glDrawArrays(GL_TRIANGLES, 0, obj.vecTriangle.size() / 3);
		glDisableVertexAttribArray(VertexArrayID - 1);
		glDisableVertexAttribArray(NormalArrayID - 1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		// Reset time
		last_time = current_time;

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteTextures(1, &Texture);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

void	rendering(Obj& obj, std::string textureFile)
{
	GLFWwindow*	window = init_openGL();
	loopDraw(window, obj, textureFile);
}