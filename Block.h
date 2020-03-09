#ifndef BLOCK_H
#define BLOCK_H


#include "Shader.h"
#include "Cube.h"


class Block {
public:

	Shader shader;
	Cube blockObject;

	Block(){}

	Block(Shader sh, Cube bo) {
		shader = sh;
		blockObject = bo;
	}

	void draw(glm::mat4 model, glm::mat4 transform) {
		shader.use();
		shader.setMat4("model", model);
		shader.setMat4("transform", transform);
		//glBindVertexArray(blockObject.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	





};
#endif