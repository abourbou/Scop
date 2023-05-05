
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

// TODO buttons/mouse for rotations ?
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

std::chrono::milliseconds	GetTimeMs()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
}

Matrix<float, 4, 4>	GetModel(const Vector3f& centerObj)
{
	static const auto	startTime = GetTimeMs();
	auto				currentTime = GetTimeMs();

	// Make the object rotate around himself
	const GLfloat rotPerMs = 0.036;
	GLfloat angle = -10. + ((currentTime.count() - startTime.count()) % 10000) * rotPerMs;

	auto Model = Translation(centerObj[0], centerObj[1], centerObj[2])
					* Ry(angle)
					* Translation(-centerObj[0], -centerObj[1], -centerObj[2]);

	return Model;
}

Matrix<float,4,4> HandleView(GLFWwindow* window, Vector3f& camPos, Vector3f& centerView, float magnitude)
{
	static const auto	startCamPos = camPos;
	static const auto	startCenterView = centerView;

	static const auto	startTime = GetTimeMs();
	auto				currentTime = GetTimeMs();
	static auto			lastTime = currentTime;

	// Translation
	GLfloat translSpeed = 0.005 * magnitude;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		translSpeed *= 2;
	auto deltaTime = currentTime - lastTime;

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
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		camPos = startCamPos;
		centerView = startCenterView;
	}

	auto View = ViewMatrix(camPos,
						   centerView,
						   Vector3<GLfloat>{{0,1,0}});

	lastTime = currentTime;
	return View;
}

float	HandleChangeColor(GLFWwindow* window)
{
	(void)window;
	static float direction = 1.f;
	static auto startTime = GetTimeMs();
	static float degree = 0;
	const float ratio = 0.5f;

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		if (degree == 0 || degree == 1)
			startTime = GetTimeMs();
		if (degree == 0)
			degree = 0.000001;
		else if (degree == 1)
			degree = 0.999999;
	}

	if (degree > 0. && degree < 1.)
	{
		auto currentTime = GetTimeMs();
        auto deltaTime = currentTime.count() - startTime.count();
		degree += deltaTime / 1000.0f * ratio * direction;
		if (degree > 1.)
		{
			direction *= -1.;
			degree = 1.;
		}
		else if (degree < 0.)
		{
			direction *= -1.;
			degree = 0.;
		}
	}

	return degree;
}

void	loopDraw(GLFWwindow* window, const Obj &obj, std::string textureFile)
{
	// Create VOA for vertex
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("shaders/VertexShader.vertexshader", "shaders/FragmentShader.fragmentshader");

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

	// Preparation for MVP matrix
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	const auto Projection = PerspectiveProj(45.f, 4.f/3.f, 0.1f, 100.f);
	const auto magnitude = (obj.maxDistCenter + obj.meanDistCenter) / 2.f;
	Vector3f camPos = {{magnitude * 4.0f,0,0}};
	Vector3f centerView = {obj.centerPoint};

	// Create VOA for UV
	GLuint UVarrayID;
	glGenVertexArrays(1, &UVarrayID);
	glBindVertexArray(UVarrayID);

	// Bind list of UV
	GLuint	UVbuffer;
	glGenBuffers(1, &UVbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, UVbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj.vecUV.size(), obj.vecUV.data(), GL_STATIC_DRAW);

	// Create var for gradTexture
	GLuint gradientColorID = glGetUniformLocation(programID, "gradTexture");

	// Load texture
	GLuint TextureNb = LoadTexture(textureFile, "./wavefront_obj/fire_camp.bmp");
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Use our shaders
		glUseProgram(programID);

		// Bind our texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureNb);
		// Set our "myTextureSampler" sampler to texture
		glUniform1i(TextureID, TextureNb - 1);

		// Handle texture key
		//auto gradTexture = HandleChangeColor(window);
		// float gradTexture = 0.5;
		float gradTexture = HandleChangeColor(window);
		glUniform1f(gradientColorID, gradTexture);

		// Create MVP matrix
		auto Model = GetModel(obj.centerPoint);
		auto View = HandleView(window, camPos, centerView, magnitude);
		auto MVP = Projection * View * Model;

		// openGL use column major matrix so we need to transpose final matrix
		auto t_MVP = transpose(MVP);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &t_MVP(0,0));

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(VertexArrayID - 1);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(VertexArrayID - 1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		// 2nd attribute buffer : normal
		glEnableVertexAttribArray(NormalArrayID - 1);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(NormalArrayID - 1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		// 3rd attribute buffer : texture coordinates
		glEnableVertexAttribArray(UVarrayID - 1);
		glBindBuffer(GL_ARRAY_BUFFER, UVbuffer);
		glVertexAttribPointer(UVarrayID - 1, 2, GL_FLOAT, GL_FALSE, 0, 0);

        // Draw the object
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
	glDeleteTextures(1, &TextureNb);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

void	rendering(Obj& obj, std::string textureFile)
{
	GLFWwindow*	window = init_openGL();
	loopDraw(window, obj, textureFile);
}