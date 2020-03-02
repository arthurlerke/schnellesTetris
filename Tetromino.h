#ifndef TETROMINO_H
#define TETROMINO_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/string_cast.hpp"

#include "Gamesector.h"
#include <random>
#include <iostream>


class Tetromino {
public:

	int height;
	int widht;

	int size = 4;

	std::vector <glm::mat4> models;
	std::vector <GameSector> elements;
	std::vector <glm::vec3> currentPositions;
	std::vector <glm::vec2> currentIndex;

	std::vector<std::vector<GameSector> > gamefield;

	std::vector<glm::vec2> index;
	std::vector<std::vector<glm::vec3> > rotationMatrix;

	glm::vec3 pivot;
	int pivotIndex;

	float angle = 0.0f;
	float rotation = 0.0f;
	float distance = 0.0f;


	Tetromino(std::vector < std::vector<GameSector> > gf, std::vector <Block> blocks, int c) {
		rotationMatrix = std::vector<std::vector<glm::vec3> >(size, std::vector<glm::vec3>(size));
		widht = gf.size();
		height = gf[0].size();
		gamefield = gf;
		int x = gf.size() / 2;
		int y = gf[0].size() - 2;
		std::random_device rd;
		std::mt19937 generator(rd());
		std::uniform_int_distribution<> range(0, blocks.size() - 1);
		Block b = blocks[range(generator)];

		//L
		if (c == 0) {
			elements.push_back(GameSector(gf[x][y].WorldPosition, b));
			index.push_back(glm::vec2(x, y));
			elements.push_back(GameSector(gf[x][y - 1.0].WorldPosition, b));
			index.push_back(glm::vec2(x, y - 1));
			elements.push_back(GameSector(gf[x][y - 2.0].WorldPosition, b));
			index.push_back(glm::vec2(x, y - 2));
			elements.push_back(GameSector(gf[x + 1.0][y - 2.0].WorldPosition, b));
			index.push_back(glm::vec2(x + 1, y - 2));
			pivotIndex = 1;
			pivot = elements[pivotIndex].WorldPosition;
		}

		//ander Figuren


		for (int i = 0; i < elements.size(); i++) {
			models.push_back(glm::translate(glm::mat4(1.0f), elements[i].WorldPosition));
			currentPositions.push_back(elements[i].WorldPosition);
			currentIndex.push_back(glm::vec2(elements[i].WorldPosition.x+5.5, elements[i].WorldPosition.y+11));
		
		}

		for (int i = 0; i < rotationMatrix.size(); i++) {
			for (int j = 0; j < rotationMatrix[i].size(); j++) {
				rotationMatrix[i][j] = gf[i + 3][j + 17].WorldPosition;
			}
		}
	}

std::vector<float> draw(float m_speed, float r_speed) {
	std::vector <float> vec;

	//rotation
	angle = angle + r_speed;
	if (angle == 181)
		angle = -179;
	if (angle == -181)
		angle = 179;

	//horizontal movement
	distance = distance + m_speed;
	if (distance == 1.25)
		distance = 0.25;
	if (distance == -1.25)
		distance = -0.25;

	bool collsion = false;

	if (m_speed != 0.0)
		collsion = checkCollision(m_speed);

	for (int i = 0; i < models.size(); i++) {
		glm::mat4 model = glm::translate(models[i], glm::vec3(0.0, 0.0, 0.0));
		glm::mat4 transform = glm::mat4(1.0f);
		pivot = elements[pivotIndex].WorldPosition;
		glm::vec3 pivotTemp = pivot - elements[i].WorldPosition;
		transform = glm::translate(transform, pivotTemp);
		transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
		transform = glm::translate(transform, -pivotTemp);

		if (collsion == false) {
			model = glm::translate(model, glm::vec3(1.0, 0.0, 0.0) * m_speed);		
		}

		models[i] = model;
		elements[i].block.draw(model, transform);
	}
	if (((distance == 1.0f || distance == -1.0f) && m_speed != 0.0f)) {
		for (int i = 0; i < elements.size(); i++) {
			currentIndex[i].x = currentIndex[i].x + distance;
			distance = 0.0f;
		}
		distance = 0.0f;
		vec.push_back(0.0f);
	}
	else
		vec.push_back(m_speed);

	if ((angle == 0.0f || angle == 90.0f || angle == 180.0f || angle == -90.0f || angle == -180.0f) && r_speed != 0.0) {
		rotatePoitions(angle);
		vec.push_back(0.0f);
	}
	else
		vec.push_back(r_speed);

	return vec;
}

bool checkCollision(float direction) {
	return false;
}

std::vector < std::vector<glm::vec3> > rotateMatrix(std::vector < std::vector<glm::vec3> > matrix) {
	std::vector<std::vector<glm::vec3> >rm(size, std::vector<glm::vec3>(size));
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			rm[i][j] = matrix[(size-j)-1.0][i];
		}
	}

	return rm;
}

void rotatePoitions(float a) {
	std::vector<std::vector<glm::vec3> >rm = rotationMatrix;

	if (a == 0.0f) {
		rm = rotationMatrix;
	}
	if (a == 90.0f) {
		rm = rotateMatrix(rm);
	}
	if (a == 180.0f) {
		rm = rotateMatrix(rm);
		rm = rotateMatrix(rm);
	}
	if (a == -90.0f) {
		rm = rotateMatrix(rm);
		rm = rotateMatrix(rm);
	}
	if (a == -180.0f) {
		rm = rotateMatrix(rm);
		rm = rotateMatrix(rm);
		rm = rotateMatrix(rm);
	}

}

};
 
#endif
