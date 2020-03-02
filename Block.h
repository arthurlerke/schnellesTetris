#ifndef BLOCK_H
#define BLOCK_H


#include "Shader.h"
#include "BlockObject.h"


class Block {
public:

	Shader shader;
	BlockObject blockObject;

	Block(){}

	Block(Shader sh, BlockObject bo) {
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