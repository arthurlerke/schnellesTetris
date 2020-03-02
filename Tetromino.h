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
	std::vector <glm::mat4> translation;
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
			currentIndex.push_back(glm::vec2(elements[i].WorldPosition.x + 5.5, elements[i].WorldPosition.y + 11));
			moved.push_back(0);
		}

		for (int i = 0; i < rotationMatrix.size(); i++) {
			for (int j = 0; j < rotationMatrix[i].size(); j++) {
				rotationMatrix[i][j] = gf[i + 3][j + 17].WorldPosition;
			}
		}
	}

	float movements = 100.0f;
	std::vector<int> moved;
	void move(glm::mat4 *model, float *direction, int index) {
		if (*direction != 0.0f && moved[index] < movements) {
			moved[index]++;
			*model = glm::translate(*model, glm::vec3(1.0, 0.0, 0.0) * 1.0f / movements * *direction);
		}
		else {
			moved[index] = 0;
			addToIndex((int)*direction);
			*direction = 0.0;
			if (index == 0) {
				std::cout << "\n";
			}
			std::cout << "Pos:";
			std::cout << currentIndex[index].x;
		}
	}
	void addToIndex(int n) {
		if (n == 0) return;
		for (int i = 0; i < currentIndex.size(); i++) {
			currentIndex[i] += n;
		}
	}
	void draw(float *direction, float r_speed) {
		std::vector <float> vec;

		//rotation
		angle = angle + r_speed;

		bool collsion = false;

		for (int i = 0; i < models.size(); i++) {
			glm::mat4 model = models[i];
			glm::mat4 transform = glm::mat4(1.0f);
			pivot = elements[pivotIndex].WorldPosition;
			glm::vec3 pivotTemp = pivot - elements[i].WorldPosition;
			transform = glm::translate(transform, pivotTemp);
			transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
			transform = glm::translate(transform, -pivotTemp);

			move(&model, direction, i);

			models[i] = model;
			elements[i].block.draw(model, transform);
		}
	}

	bool checkCollision(float direction) {
		return false;
	}
	float angleRot = 0;
	float getAngle(bool rotate) {
		if (rotate && angleRot <= 90) {
			return angleRot++;
		}
		else if (angleRot >= 90) {

			angleRot = 0;
		}
		else {
			return angleRot;
		}
	}
	std::vector < std::vector<glm::vec3> > rotateMatrix(std::vector < std::vector<glm::vec3> > matrix) {
		std::vector<std::vector<glm::vec3> >rm(size, std::vector<glm::vec3>(size));
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				rm[i][j] = matrix[(size - j) - 1.0][i];
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
