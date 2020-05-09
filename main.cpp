#include "glad/glad.h"  //Include order can matter here

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#else
#include <SDL.h>
#include <SDL_opengl.h>
#endif
#include <cstdio>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "camera.h"

// Library to load obj files
// https://github.com/syoyo/tinyobjloader
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "path.h"

#include <iostream>




using namespace std;

int screen_width = 800;
int screen_height = 600;

bool SHOW_PATH = false;
bool APP_START = false;
int AGENT_NUMBER = 0;
int OBSTACLE_NUMBER = 6;
int PICKED = 0;
GLuint vao[5];
GLuint vbo[5];

// Shader sources
const GLchar* vertexSource =
"#version 150 core\n"
"in vec3 position;"
"in vec3 inNormal;"
"in vec2 inTexcoord;"
"const vec3 inLightDir = normalize(vec3(0,-1,1));"
"out vec3 normal;"
"out vec3 lightDir;"
"out vec2 texcoord;"
"out vec3 Color;"
"uniform mat4 model;"
"uniform mat4 view;"
"uniform mat4 proj;"
"uniform vec3 inColor;"
"void main() {"
"   Color = inColor;"
"   gl_PointSize = 10.0;"
"   gl_Position = proj * view * model * vec4(position,1.0);"
"   vec4 norm4 = transpose(inverse(view * model)) * vec4(inNormal,0.0);"
"   normal = normalize(norm4.xyz);"
"   lightDir = (view * vec4(inLightDir,0)).xyz;"
"	texcoord = inTexcoord;"
"}";

const GLchar* fragmentSource =
"#version 150 core\n"
"in vec3 normal;"
"in vec3 lightDir;"
"in vec2 texcoord;"
"in vec3 Color;"
"out vec4 outColor;"
"uniform sampler2D tex0;"
"uniform int type;"
"const float ambient = .6;"
"void main() {"
"	vec3 color;"
"	if (type == 1)"
"		color = Color;"
"	else if (type == 2)"
"		color = texture(tex0, texcoord).rgb;"
"   else if (type == 3){"
"       outColor = vec4(Color,1.0);"
"       return;"
"   }"
"   vec3 diffuseC = color*max(dot(-lightDir,normal),0);"
"   vec3 ambC = color*ambient;"
"	vec3 combined = diffuseC + ambC;"
"   outColor = vec4(combined, 1.0);"
"}";

bool fullscreen = false;
void draw(float dt);

//Index of where to model, view, and projection matricies are stored on the GPU
GLint uniModel, uniView, uniProj, uniColor, uniType;

float aspect; //aspect ratio (needs to be updated if the window is resized)
Camera* camera;
SPath* path;
int num_verts_total;
int num_verts_player;
int num_verts_obstacle;
int num_verts_floor;
int num_verts_P90;

std::vector<tinyobj::real_t> loadModel(const char* filename) {
	std::string inputfile = filename;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	bool ret =
		tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str());
	if (!err.empty()) {  // `err` may contain warning message.
		std::cerr << err << std::endl;
	}

	if (!ret) {
		exit(1);
	}

	std::vector<tinyobj::real_t> model;

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			unsigned fv = shapes[s].mesh.num_face_vertices[f];
			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				tinyobj::real_t vx, vy, vz, nx, ny, nz, tx, ty;
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				vx = attrib.vertices[3 * idx.vertex_index + 0];
				vy = attrib.vertices[3 * idx.vertex_index + 1];
				vz = attrib.vertices[3 * idx.vertex_index + 2];
				if (attrib.normals.size()) {
					nx = attrib.normals[3 * idx.normal_index + 0];
					ny = attrib.normals[3 * idx.normal_index + 1];
					nz = attrib.normals[3 * idx.normal_index + 2];
				}
				else {
					nx = 0.;
					ny = 0.;
					nz = 0.;
				}
				if (attrib.texcoords.size()) {
					tx = attrib.texcoords[2 * idx.texcoord_index + 0];
					ty = attrib.texcoords[2 * idx.texcoord_index + 1];
				}
				else {
					tx = 0.;
					ty = 0.;
				}
				tinyobj::real_t data[8] = { vx, vy, vz, nx, ny, nz, tx, ty };
				model.insert(model.end(), data, data + 8);
			}
			index_offset += fv;
		}
	}
	return model;
}

void draw(float dt) {
	camera->updateCamera(dt, 800.f, 600.f);
	if (APP_START) path->update(dt);
	

	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(camera->proj));
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera->view));

	glm::mat4 model = glm::mat4();

	if (SHOW_PATH)
	{
		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(3);
		glBindVertexArray(vao[1]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, path->n_edges_ * 6 * sizeof(float), path->vertices_, GL_STATIC_DRAW);
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
		glm::vec3 path_col = glm::vec3(1.0, 0.0, 1.0);
		glUniform3fv(uniColor, 1, glm::value_ptr(path_col));
		glUniform1i(uniType, 3);
		glDrawArrays(GL_LINES, 0, 2 * path->n_edges_);
		glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glDrawArrays(GL_POINTS, 0, 2 * path->n_edges_);
	}
	
	glBindVertexArray(vao[3]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	for (int i = 0; i < path->numEnemy; i++)
	{
		
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(path->enemys_[i]->ePos[0], 1.1, path->enemys_[i]->ePos[1]));
		model = glm::scale(model, glm::vec3(0.5, 1, 0.5));
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
		//glm::vec3 player_col = glm::vec3(1.0, 0.0, 0.0);
		glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(1.0, 0.0, 0.0)));
		glUniform1i(uniType, 1);
		glDrawArrays(GL_TRIANGLES, 0, num_verts_player);
	}
	
	glBindVertexArray(vao[2]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	for (int i = 0; i < path->o_pos.size(); i++)
	{
		
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(path->o_pos[i][0], 1.1, path->o_pos[i][1]));
		model = glm::scale(model, glm::vec3(path->o_radius[i], 1, path->o_radius[i]));
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
		if (i == PICKED) glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 0.0)));
		else glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 1.0)));
		glUniform1i(uniType, 1);
		glDrawArrays(GL_TRIANGLES, 0, num_verts_obstacle);
	}

	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	model = glm::mat4();
	model = glm::scale(model, 2.f * glm::vec3(1, 1, 1));
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniform1i(uniType, 2);
	glDrawArrays(GL_TRIANGLES, 0, num_verts_floor);


	glBindVertexArray(vao[4]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0,10,0));
	model = glm::scale(model, 2.f * glm::vec3(1, 1, 1));
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
	//glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
	glUniform1i(uniType, 2);
	glDrawArrays(GL_TRIANGLES, 0, num_verts_P90);


}


int main(int argc, char** argv) {


	SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)



	//Ask SDL to get a recent version of OpenGL (3.2 or greater)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	//Create a window (offsetx, offsety, width, height, flags)
	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 0, 0, screen_width, screen_height, SDL_WINDOW_OPENGL);
	aspect = screen_width / (float)screen_height; //aspect ratio (needs to be updated if the window is resized

	//The above window cannot be resized which makes some code slightly easier.
	//Below show how to make a full screen window or allow resizing
	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 0, 0, screen_width, screen_height, SDL_WINDOW_FULLSCREEN|SDL_WINDOW_OPENGL);
	SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 200, 200, screen_width, screen_height, SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,0,0,SDL_WINDOW_FULLSCREEN_DESKTOP|SDL_WINDOW_OPENGL); //Boarderless window "fake" full screen



	//Create a context to draw in
	SDL_GLContext context = SDL_GL_CreateContext(window);

	if (gladLoadGLLoader(SDL_GL_GetProcAddress)) {
		printf("\nOpenGL loaded\n");
		printf("Vendor:   %s\n", glGetString(GL_VENDOR));
		printf("Renderer: %s\n", glGetString(GL_RENDERER));
		printf("Version:  %s\n\n", glGetString(GL_VERSION));
	}
	else {
		printf("ERROR: Failed to initialize OpenGL context.\n");
		return -1;
	}

	camera = new Camera(glm::vec3(0.f, 35.f, 38.f), 800.f, 600.f);
	path = new SPath(OBSTACLE_NUMBER);
	//path->init();

	SDL_Surface* surface = SDL_LoadBMP("earth.bmp");

	GLuint tex0;
	glGenTextures(1, &tex0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);

	// What to do outside 0-1 range
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load the texture into memory
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA,
		GL_UNSIGNED_BYTE, surface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	SDL_FreeSurface(surface);
	

	std::vector<tinyobj::real_t> player_model = loadModel("objects/cylinder.obj");
	num_verts_player = player_model.size() / 8;
	int num_player = num_verts_player;
	std::vector<tinyobj::real_t> player_model_data;
	player_model_data.reserve(player_model.size());
	player_model_data.insert(player_model_data.end(), player_model.begin(), player_model.end());


	std::vector<tinyobj::real_t> obstacle_model = loadModel("objects/cylinder.obj");
	num_verts_obstacle = obstacle_model.size() / 8;
	int num_obstacle = num_verts_obstacle;
	std::vector<tinyobj::real_t> obstacle_model_data;
	obstacle_model_data.reserve(obstacle_model.size());
	obstacle_model_data.insert(obstacle_model_data.end(), obstacle_model.begin(), obstacle_model.end());

	std::vector<tinyobj::real_t> floor_model = loadModel("objects/floor.obj");
	num_verts_floor = floor_model.size() / 8;
	int num_floor = num_verts_floor;
	std::vector<tinyobj::real_t> floor_model_data;
	floor_model_data.reserve(floor_model.size());
	floor_model_data.insert(floor_model_data.end(), floor_model.begin(), floor_model.end());

	std::vector<tinyobj::real_t> P90_model = loadModel("objects/Neck_Mech_Walker_by_3DHaupt-(Wavefront OBJ).obj");
	num_verts_P90 = P90_model.size() / 8;
	int num_P90 = num_verts_P90;
	std::vector<tinyobj::real_t> P90_model_data;
	P90_model_data.reserve(P90_model.size());
	P90_model_data.insert(P90_model_data.end(), P90_model.begin(), P90_model.end());

	glGenVertexArrays(5, vao);
	glGenBuffers(5, vbo);

	//Load the vertex Shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	//Let's double check the shader compiled 
	GLint status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (!status) {
		char buffer[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"Compilation Error",
			"Failed to Compile: Check Consol Output.",
			NULL);
		printf("Vertex Shader Compile Failed. Info:\n\n%s\n", buffer);
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	//Double check the shader compiled 
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (!status) {
		char buffer[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"Compilation Error",
			"Failed to Compile: Check Consol Output.",
			NULL);
		printf("Fragment Shader Compile Failed. Info:\n\n%s\n", buffer);
	}

	//Join the vertex and fragment shaders together into one program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor"); // set output
	glLinkProgram(shaderProgram); //run the linker

	glUseProgram(shaderProgram); //Set the active shader (only one can be used at a time)

	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	GLint normAttrib = glGetAttribLocation(shaderProgram, "inNormal");
	GLint texAttrib = glGetAttribLocation(shaderProgram, "inTexcoord");

	
	glBindVertexArray(vao[3]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, num_player * 8 * sizeof(float), &player_model_data[0], GL_STATIC_DRAW);

	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glEnableVertexAttribArray(posAttrib);

	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(normAttrib);

	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(texAttrib);
	
	

	
	glBindVertexArray(vao[2]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, num_obstacle * 8 * sizeof(float), &obstacle_model_data[0], GL_STATIC_DRAW);

	//GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glEnableVertexAttribArray(posAttrib);

	//GLint normAttrib = glGetAttribLocation(shaderProgram, "inNormal");
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(normAttrib);

	//GLint texAttrib = glGetAttribLocation(shaderProgram, "inTexcoord");
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(texAttrib);
	
	

	//################# FLOOR ######################
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, num_floor * 8 * sizeof(float), &floor_model_data[0], GL_STATIC_DRAW);

	//GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glEnableVertexAttribArray(posAttrib);

	//GLint normAttrib = glGetAttribLocation(shaderProgram, "inNormal");
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(normAttrib);

	//GLint texAttrib = glGetAttribLocation(shaderProgram, "inTexcoord");
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(texAttrib);

	//################# P90 ######################
	glBindVertexArray(vao[4]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, num_P90 * 8 * sizeof(float), &P90_model_data[0], GL_STATIC_DRAW);

	//GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glEnableVertexAttribArray(posAttrib);

	//GLint normAttrib = glGetAttribLocation(shaderProgram, "inNormal");
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(normAttrib);

	//GLint texAttrib = glGetAttribLocation(shaderProgram, "inTexcoord");
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(texAttrib);


	glBindVertexArray(vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, path->n_edges_ * 6 * sizeof(float), path->vertices_, GL_STATIC_DRAW);

	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);


	//Where to model, view, and projection matricies are stored on the GPU
	uniModel = glGetUniformLocation(shaderProgram, "model");
	uniView = glGetUniformLocation(shaderProgram, "view");
	uniProj = glGetUniformLocation(shaderProgram, "proj");
	uniColor = glGetUniformLocation(shaderProgram, "inColor");
	uniType = glGetUniformLocation(shaderProgram, "type");

	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
	//Event Loop (Loop forever processing each event as fast as possible)
	SDL_Event windowEvent;


	bool quit = false;
	//float lastTime = SDL_GetTicks() / 1000.f;
	float dt = 0;
	int frame = 0;
	unsigned t0 = SDL_GetTicks();
	unsigned t1 = t0;

	while (!quit) {
		while (SDL_PollEvent(&windowEvent)) {
			if (windowEvent.type == SDL_QUIT) quit = true; //Exit event loop
		  //List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch many special keys
		  //Scancode referes to a keyboard position, keycode referes to the letter (e.g., EU keyboards)
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE)
				quit = true; ; //Exit event loop
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f) //If "f" is pressed
				fullscreen = !fullscreen;
			SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0); //Set to full screen 

			
			// w s a d q e control
			if (windowEvent.key.keysym.sym == SDLK_w && windowEvent.type == SDL_KEYDOWN) camera->positiveMovement.z = 1;
			else if (windowEvent.key.keysym.sym == SDLK_w && windowEvent.type == SDL_KEYUP) camera->positiveMovement.z = 0;

			if (windowEvent.key.keysym.sym == SDLK_s && windowEvent.type == SDL_KEYDOWN) camera->negativeMovement.z = -1;
			else if (windowEvent.key.keysym.sym == SDLK_s && windowEvent.type == SDL_KEYUP) camera->negativeMovement.z = 0;

			if (windowEvent.key.keysym.sym == SDLK_a && windowEvent.type == SDL_KEYDOWN) camera->positiveMovement.x = -1;
			else if (windowEvent.key.keysym.sym == SDLK_a && windowEvent.type == SDL_KEYUP) camera->positiveMovement.x = 0;

			if (windowEvent.key.keysym.sym == SDLK_d && windowEvent.type == SDL_KEYDOWN) camera->negativeMovement.x = 1;
			else if (windowEvent.key.keysym.sym == SDLK_d && windowEvent.type == SDL_KEYUP) camera->negativeMovement.x = 0;

			if (windowEvent.key.keysym.sym == SDLK_q && windowEvent.type == SDL_KEYDOWN) camera->positiveMovement.y = 1;
			else if (windowEvent.key.keysym.sym == SDLK_q && windowEvent.type == SDL_KEYUP) camera->positiveMovement.y = 0;

			if (windowEvent.key.keysym.sym == SDLK_e && windowEvent.type == SDL_KEYDOWN) camera->negativeMovement.y = -1;
			else if (windowEvent.key.keysym.sym == SDLK_e && windowEvent.type == SDL_KEYUP) camera->negativeMovement.y = 0;
			
			// left right up down control
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LEFT) camera->negativeTurn.x = 1;
			else if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_LEFT) camera->negativeTurn.x = 0;

			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_RIGHT) camera->positiveTurn.x = -1;
			else if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_RIGHT) camera->positiveTurn.x = 0;

			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_UP) camera->positiveTurn.y = -1;
			else if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_UP) camera->positiveTurn.y = 0;

			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_DOWN) camera->negativeTurn.y = 1;
			else if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_DOWN) camera->negativeTurn.y = 0;

			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_r) 
				SHOW_PATH = !SHOW_PATH;

			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_p)
			{
				path->DeleteAll();
				path->init();
				APP_START = true;
			}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_b)
			{
				path->March();
			}
			
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_SPACE)
			{
				OBSTACLE_NUMBER++;
				//path->num_obstacles ++;
				PICKED = OBSTACLE_NUMBER - 1;
				path->o_pos.push_back(glm::vec2(0.f, 0.f));
				path->o_radius.push_back(2.f);
			}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_i)
			{
				path->o_pos[PICKED].y -= 0.3;
			}

			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_j)
			{
				path->o_pos[PICKED].x -= 0.3;
			}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_l)
			{
				path->o_pos[PICKED].x += 0.3;
			}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_k)
			{
				path->o_pos[PICKED].y += 0.3;
			}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_u)
			{
				path->o_radius[PICKED] -= 0.1;
				if (path->o_radius[PICKED] < 0.11) path->o_radius[PICKED] = 0.11;
			}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_o)
			{
				path->o_radius[PICKED] += 0.1;
			}

			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_n)
			{			
				PICKED--;
				if (PICKED < 0) PICKED = 0;
			}
			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_m)
			{
				PICKED++;
				if (PICKED > OBSTACLE_NUMBER-1) PICKED = OBSTACLE_NUMBER - 1;
			}

			if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_BACKSPACE)
			{
				path->o_pos.erase(path->o_pos.begin() + PICKED);
				path->o_radius.erase(path->o_radius.begin() + PICKED);
				OBSTACLE_NUMBER--;
				//path->num_obstacles--;
				PICKED = OBSTACLE_NUMBER - 1;
				
			}
			/*
			if (windowEvent.type == SDL_MOUSEBUTTONUP)
			{
				add_new_obs = false;
			}
			*/
		}

		// Clear the screen to default color
		glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		dt = 0.001;
		
		draw(dt);

		//if (saveOutput) Win2PPM(screen_width, screen_height);


		SDL_GL_SwapWindow(window); //Double buffering


		frame++;
		t1 = SDL_GetTicks();
		if (t1 - t0 > 1000)
		{
			printf("Average Frames Per Second: %.4f\r", frame / ((t1 - t0) / 1000.f));
			fflush(stdout);
			t0 = t1;
			frame = 0;
		}
	}

	//Clean Up
	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	glDeleteBuffers(5, vbo);
	glDeleteVertexArrays(5, vao);

	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return 0;
}




