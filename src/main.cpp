#define TINYOBJLOADER_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS

#include "precompiled.h"
#include "functions.h"
#include "draw.h"
#include "dll_cable.h"

int framebufferWidth, framebufferHeight;

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

	framebufferWidth = width;
	framebufferHeight = height;
}

void errorCallback(int errorCode, const char* errorDescription)
{
	std::cerr << "Error: " << errorDescription << std::endl;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main()
{
	glfwSetErrorCallback(errorCallback);
	if (!glfwInit()) 
	{
		std::cerr << "Error: GLFW initialization failed" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "OpenGL", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glewExperimental = GL_TRUE;
	GLenum errorCode = glewInit();
	if (GLEW_OK != errorCode)
	{
		std::cerr << "Error: GLEW initialization failed - " << glewGetErrorString(errorCode) << std::endl;
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}
	if (!GLEW_VERSION_3_3)
	{
		std::cerr << "Error: OpenGL 3.3 API is not available." << std::endl;
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

	// Frame rate
	glfwSwapInterval(0);
	double lastTime = glfwGetTime();
	int numOfFrames = 0;
	int count = 0;

	// Initial interface
	glm::vec3 position = glm::vec3(0, -0.4, -1.44); 
	float horizontalAngle = PI;
	float verticalAngle = PI;
	float initialFoV = 1.00f;
	float speed = 0.005f;
	float FoV = initialFoV;
	glfwSetCursorPos(window, 1280 / 2, 720 / 2);

	// Draw class
	Draw drawground;
	drawground.draw_init();
	drawground.initShaderProgram();
	const char* ground_objdir_vec;
	const char* ground_objrootdir_vec;
	ground_objdir_vec = "..\\obj\\tile\\tile.obj";
	ground_objrootdir_vec = "..\\obj\\tile\\";
	drawground.read_obj(ground_objdir_vec, ground_objrootdir_vec);

	Draw drawcylinder;
	drawcylinder.draw_init();
	drawcylinder.initShaderProgram();
	const char* cyl_objdir_vec;
	const char* cyl_objrootdir_vec;
	cyl_objdir_vec = "..\\obj\\cylinder\\cylinder.obj";
	cyl_objrootdir_vec = "..\\obj\\cylinder\\";
	drawcylinder.read_obj(cyl_objdir_vec, cyl_objrootdir_vec);

	Draw drawlink;
	drawlink.draw_init();
	drawlink.initShaderProgram();
	const char* link_objdir_vec;
	const char* link_objrootdir_vec;
	link_objdir_vec = "..\\obj\\link\\link.obj";
	link_objrootdir_vec = "..\\obj\\link\\";
	drawlink.read_obj(link_objdir_vec, link_objrootdir_vec);

	Draw drawFranka[8];
	for (int i = 0;i < 8;i++)
	{
		drawFranka[i].draw_init();
		drawFranka[i].initShaderProgram();
		char Franka_objdir_vec[100];
		char Franka_objrootdir_vec[100];
		char *t1 = "..\\obj\\franka\\Franka_Link";
		char t2[1];
		sprintf(t2, "%d", i);
		char *t3 = "_assy OBJ\\Franka_Link";
		char *t4 = "_assy.obj";
		sprintf(Franka_objdir_vec, "%s%s%s%s%s", t1, t2, t3, t2, t4);
		char *t5 = "_assy OBJ\\";
		sprintf(Franka_objrootdir_vec, "%s%s%s", t1, t2, t5);
		drawFranka[i].read_obj(Franka_objdir_vec, Franka_objrootdir_vec);
	}
	
	Draw drawFT;
	drawFT.draw_init();
	drawFT.initShaderProgram();
	const char* FT_objdir_vec;
	const char* FT_objrootdir_vec;
	FT_objdir_vec = "..\\obj\\FT\\FT.obj";
	FT_objrootdir_vec = "..\\obj\\FT\\";
	drawFT.read_obj(FT_objdir_vec, FT_objrootdir_vec);

	// Default coordinate transform
	glm::mat4 Default_Rot(0.0);
	Default_Rot[0][0] = 1.0;
	Default_Rot[1][2] = -1.0;
	Default_Rot[2][1] = 1.0;
	Default_Rot[3][3] = 1.0;
	glm::mat4 Default_Rot_T(0.0);
	Default_Rot_T[0][0] = 1.0;
	Default_Rot_T[2][1] = -1.0;
	Default_Rot_T[1][2] = 1.0;
	Default_Rot_T[3][3] = 1.0;

	glm::mat4 Default_Rot2(0.0);
	Default_Rot2[2][0] = -1.0;
	Default_Rot2[0][1] = 1.0;
	Default_Rot2[1][2] = -1.0;
	Default_Rot2[3][3] = 1.0;
	glm::mat4 Default_Rot_T2(0.0);
	Default_Rot_T2[0][2] = -1.0;
	Default_Rot_T2[1][0] = 1.0;
	Default_Rot_T2[2][1] = -1.0;
	Default_Rot_T2[3][3] = 1.0;

	// Set cable environment
	int N_link = 160;
	double r = 0.002;
	double L = 0.4 / N_link;
	double rho = 1259;
	double E = 2.954*1e6;
	double nu = 0.49;
	double mu = 1.3;
	double g = 9.81;
	double cyl_radius = 0.02;
	double cyl_center_x = 0.4920;
	double cyl_center_y = -0.2025;
	DLL_CABLE::set(N_link, r, L, rho, E, nu, mu, g, cyl_radius, cyl_center_x, cyl_center_y);
	double p_des[3];
	double R_des[3][3];
	double time;
	double *data = new double[12 * N_link + 12 * 8];
	double *FT = new double[6];

	// Load trajectory
	std::string dir_p = "..\\data\\p_des.txt";
	std::string dir_R = "..\\data\\R_des.txt";
	std::vector<double> traj_p;
	set_trajectory(dir_p.c_str(), traj_p);
	std::vector<double> traj_R;
	set_trajectory(dir_R.c_str(), traj_R);

	// Rendering loop start
	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;
	glm::mat4 ModelMatrix;
	glm::mat4 MVP;
	int simulation_step = 0;
	int max_simulation_step = 5500;
	while (!glfwWindowShouldClose(window))
	{
		// Get data
		DLL_CABLE::get_info(time, data, FT);

		// Clear
		glClearColor(0.9, 0.9, 0.9, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Interface
		double currentTime = glfwGetTime();
		numOfFrames++;
		if (currentTime - lastTime >= 1.0) 
		{
			printf("%f ms/frame  %d fps \n", 1000.0 / double(numOfFrames), numOfFrames);
			numOfFrames = 0;
			lastTime = currentTime;
		}
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		glm::vec3 direction(cos(verticalAngle)*sin(horizontalAngle), sin(verticalAngle), cos(verticalAngle)*cos(horizontalAngle));
		glm::vec3 right = glm::vec3(sin(horizontalAngle - PI / 2), 0, cos(horizontalAngle - PI / 2));
		glm::vec3 up = glm::cross(right, direction);
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			position += direction*speed;
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			position -= direction*speed;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			position += right*speed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			position -= right*speed;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			position += up*speed;
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			position -= up*speed;
		}
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
			horizontalAngle += 0.01;
		}
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
			horizontalAngle -= 0.01;
		}
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
			verticalAngle += 0.01;
		}
		if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
			verticalAngle -= 0.01;
		}
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
			FoV += 0.01;
		}
		//std::cout << "Camera position : " << position[0] << " " << position[1] << " " << position[2] << std::endl;
		//std::cout << "Horizontal : " << horizontalAngle << std::endl;
		//std::cout << "Vertical : " << verticalAngle << std::endl;
		//std::cout << "FoV : " << FoV << std::endl;
		ProjectionMatrix = glm::perspective(FoV, 1280.0f / 720.0f, 0.1f, 100.0f);
		ViewMatrix = glm::lookAt(position, position + direction, up);

		// Draw ground
		for (int n = 0; n < 25;n++)
		{
			glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3((float)(1.0), (float)(1.0), (float)(1.0)));
			int n1 = n / 5;
			int n2 = n % 5;
			glm::vec4 Translation = Default_Rot_T2*glm::vec4(-1.6+0.8*n1, -1.6+0.8*n2, -0.01, 1); 
			glm::mat4 TransMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(Translation[0], Translation[1], Translation[2]));
			glm::mat4 RotationMatrix(0.0);
			float rot_angle = 0*PI/2; 
			RotationMatrix[0][0] = 1.0;
			RotationMatrix[1][1] = cos(rot_angle);
			RotationMatrix[1][2] = -sin(rot_angle);
			RotationMatrix[2][1] = sin(rot_angle);
			RotationMatrix[2][2] = cos(rot_angle);
			RotationMatrix[3][3] = 1.0;
			ModelMatrix = TransMatrix*Default_Rot_T2*RotationMatrix*Default_Rot*ScaleMatrix;
			MVP = ProjectionMatrix*ViewMatrix*ModelMatrix;
			drawground.draw_obj(MVP, ModelMatrix, ViewMatrix, ProjectionMatrix);
		}
		
		// Draw cylinder
		for (int n = 0; n < 1;n++)
		{	
			glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3((float)(cyl_radius/0.02), (float)(1.0), (float)(cyl_radius/0.02)));
			glm::vec4 Translation = Default_Rot_T2*glm::vec4(cyl_center_x, cyl_center_y, 0.35, 1);
			glm::mat4 TransMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(Translation[0], Translation[1], Translation[2]));
			ModelMatrix = TransMatrix*ScaleMatrix;
			MVP = ProjectionMatrix*ViewMatrix*ModelMatrix;
			drawcylinder.draw_obj(MVP, ModelMatrix, ViewMatrix, ProjectionMatrix);
		}

		// Draw Link
		for (int n = 0; n < N_link;n++)
		{
			glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3((float)(r/0.002), (float)(L/0.005), (float)(r/0.002)));
			glm::vec4 Translation = Default_Rot_T2*glm::vec4(data[12*n], data[12*n + 1], data[12*n + 2], 1);
			glm::mat4 TransMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(Translation[0], Translation[1], Translation[2]));
			glm::mat4 RotationMatrix(1.0f);
			RotationMatrix[0][0] = data[12 * n + 3];
			RotationMatrix[1][0] = data[12 * n + 4];
			RotationMatrix[2][0] = data[12 * n + 5];
			RotationMatrix[0][1] = data[12 * n + 6];
			RotationMatrix[1][1] = data[12 * n + 7];
			RotationMatrix[2][1] = data[12 * n + 8];
			RotationMatrix[0][2] = data[12 * n + 9];
			RotationMatrix[1][2] = data[12 * n + 10];
			RotationMatrix[2][2] = data[12 * n + 11];
			ModelMatrix = TransMatrix*Default_Rot_T2*RotationMatrix*Default_Rot2*ScaleMatrix;
			MVP = ProjectionMatrix*ViewMatrix*ModelMatrix;
			drawlink.draw_obj(MVP, ModelMatrix, ViewMatrix, ProjectionMatrix);
		}

		// Draw franka 
		Matrix3d Rot_franka[8];
		Vector3d Trans_franka[8];
		for (int n = 0; n < 8;n++)
		{
			if (n == 0)
			{
				Rot_franka[n] = Matrix3d::Identity();
				Trans_franka[n] << 0, 0, 0;
			}
			else
			{
				Trans_franka[n](0) = data[12 * N_link + 12 * (n - 1) + 0];
				Trans_franka[n](1) = data[12 * N_link + 12 * (n - 1) + 1];
				Trans_franka[n](2) = data[12 * N_link + 12 * (n - 1) + 2];
				Rot_franka[n](0, 0) = data[12 * N_link + 12 * (n - 1) + 3];
				Rot_franka[n](0, 1) = data[12 * N_link + 12 * (n - 1) + 4];
				Rot_franka[n](0, 2) = data[12 * N_link + 12 * (n - 1) + 5];
				Rot_franka[n](1, 0) = data[12 * N_link + 12 * (n - 1) + 6];
				Rot_franka[n](1, 1) = data[12 * N_link + 12 * (n - 1) + 7];
				Rot_franka[n](1, 2) = data[12 * N_link + 12 * (n - 1) + 8];
				Rot_franka[n](2, 0) = data[12 * N_link + 12 * (n - 1) + 9];
				Rot_franka[n](2, 1) = data[12 * N_link + 12 * (n - 1) + 10];
				Rot_franka[n](2, 2) = data[12 * N_link + 12 * (n - 1) + 11];
			}
		}
		for (int n = 0; n < 8;n++)
		{
			glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3((float)(1.0), (float)(1.0), (float)(1.0)));
			glm::vec4 Translation = Default_Rot_T2*glm::vec4(Trans_franka[n](0), Trans_franka[n](1), Trans_franka[n](2), 1);
			glm::mat4 TransMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(Translation[0], Translation[1], Translation[2]));
			glm::mat4 RotationMatrix(1.0f);
			RotationMatrix[0][0] = Rot_franka[n](0, 0);
			RotationMatrix[0][1] = Rot_franka[n](1, 0);
			RotationMatrix[0][2] = Rot_franka[n](2, 0);
			RotationMatrix[1][0] = Rot_franka[n](0, 1);
			RotationMatrix[1][1] = Rot_franka[n](1, 1);
			RotationMatrix[1][2] = Rot_franka[n](2, 1);
			RotationMatrix[2][0] = Rot_franka[n](0, 2);
			RotationMatrix[2][1] = Rot_franka[n](1, 2);
			RotationMatrix[2][2] = Rot_franka[n](2, 2);
			ModelMatrix = TransMatrix*Default_Rot_T2*RotationMatrix*ScaleMatrix;
			MVP = ProjectionMatrix*ViewMatrix*ModelMatrix;
			drawFranka[n].draw_obj(MVP, ModelMatrix, ViewMatrix, ProjectionMatrix);
		}

		// Draw FT
		Matrix3d Rot_FT = Rot_franka[7]*rot(PI, 'x')*rot(PI, 'y')*rot(3*PI / 4, 'z');
		for (int n = 0; n < 1;n++)
		{
			glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3((float)(1.0), (float)(1.0), (float)(1.0)));
			glm::mat4 RotationMatrix(1.0f);
			RotationMatrix[0][0] = Rot_FT(0, 0);
			RotationMatrix[0][1] = Rot_FT(1, 0);
			RotationMatrix[0][2] = Rot_FT(2, 0);
			RotationMatrix[1][0] = Rot_FT(0, 1);
			RotationMatrix[1][1] = Rot_FT(1, 1);
			RotationMatrix[1][2] = Rot_FT(2, 1);
			RotationMatrix[2][0] = Rot_FT(0, 2);
			RotationMatrix[2][1] = Rot_FT(1, 2);
			RotationMatrix[2][2] = Rot_FT(2, 2);
			Vector3d tmp = Rot_franka[7] * Vector3d(0,0,0.1527);
			glm::vec4 Translation = Default_Rot_T2 * (glm::vec4(Trans_franka[7](0) + tmp(0), Trans_franka[7](1) + tmp(1), Trans_franka[7](2) + tmp(2), 1));
			glm::mat4 TransMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(Translation[0], Translation[1], Translation[2]));
			ModelMatrix = TransMatrix*Default_Rot_T2*RotationMatrix*ScaleMatrix;
			MVP = ProjectionMatrix*ViewMatrix*ModelMatrix;
			drawFT.draw_obj(MVP, ModelMatrix, ViewMatrix, ProjectionMatrix);
		}
		
		// Forward simulation
		p_des[0] = traj_p[3 * simulation_step];
		p_des[1] = traj_p[3 * simulation_step + 1];
		p_des[2] = traj_p[3 * simulation_step + 2];
		R_des[0][0] = traj_R[9 * simulation_step];
		R_des[0][1] = traj_R[9 * simulation_step + 1];
		R_des[0][2] = traj_R[9 * simulation_step + 2];
		R_des[1][0] = traj_R[9 * simulation_step + 3];
		R_des[1][1] = traj_R[9 * simulation_step + 4];
		R_des[1][2] = traj_R[9 * simulation_step + 5];
		R_des[2][0] = traj_R[9 * simulation_step + 6];
		R_des[2][1] = traj_R[9 * simulation_step + 7];
		R_des[2][2] = traj_R[9 * simulation_step + 8];
		DLL_CABLE::forward(p_des, R_des);

		// Step setting
		if (simulation_step < max_simulation_step)
		{
			simulation_step = simulation_step + 1;
		}
		else
		{
			simulation_step = 0;
			DLL_CABLE::set(N_link, r, L, rho, E, nu, mu, g, cyl_radius, cyl_center_x, cyl_center_y);
			Sleep(1000);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clear
	glfwTerminate();
	std::exit(EXIT_SUCCESS);

	return 0;
}
