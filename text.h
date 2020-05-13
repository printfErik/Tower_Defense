#pragma once
#include "glad/glad.h"  //Include order can matter here

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#else
#include <SDL.h>
#include <SDL_opengl.h>
#endif
#include <cstdio>
#include "ft2build.h"
#include FT_FREETYPE_H

#include "glm/glm.hpp"

#include <iostream>
#include <string>
#include <unordered_map>

class Character {
public:
	GLuint TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	GLuint Advance;

	Character() {};
	Character(GLuint id, glm::ivec2 size, glm::ivec2 bearing, GLuint advance)
		: TextureID(id), Size(size), Bearing(bearing), Advance(advance) {};
};

class Font
{
public:
	void init(const std::string& font_file, size_t font_size);
	void updateCharacters(GLchar c, Character character) { _characters[c] = character; };
	Character queryCharacter(GLchar c) { return _characters[c]; };
	GLuint vao() { return _vao; }
	GLuint vbo() { return _vbo; }

	static Font* getInstance();
	static void renderText(GLuint shader, const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
private:
	static Font* _font;
	std::unordered_map<GLchar, Character> _characters;
	GLuint _vao;
	GLuint _vbo;

	Font() {};
};

