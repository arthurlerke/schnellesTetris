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

	std::vector<std::vector<GameSector>> *gamefield;

	std::vector<glm::vec2> index;
	std::vector<std::vector<glm::vec3> > rotationMatrix;

	glm::vec3 pivot;
	int pivotIndex;

	float angle = 0.0f;
	float rotation = 0.0f;
	float distance = 0.0f;

	bool reachedBottom = false;


	Tetromino(std::vector < std::vector<GameSector> > *gf, std::vector <Block> blocks, int c) {
		rotationMatrix = std::vector<std::vector<glm::vec3> >(size, std::vector<glm::vec3>(size));
		gamefield = gf;
		widht = gamefield->size();
		height = (*gamefield)[0].size();
		int x = gamefield->size() / 2;
		int y = (*gamefield)[0].size() - 2;
		std::random_device rd;
		std::mt19937 generator(rd());
		std::uniform_int_distribution<> range(0, blocks.size() - 1);
		Block b = blocks[range(generator)];

		//L
		if (c == 0) {
			elements.push_back(GameSector((*gamefield)[x][y].WorldPosition, b));
			index.push_back(glm::vec2(x, y));
			elements.push_back(GameSector((*gamefield)[x][y - 1.0].WorldPosition, b));
			index.push_back(glm::vec2(x, y - 1));
			elements.push_back(GameSector((*gamefield)[x][y - 2.0].WorldPosition, b));
			index.push_back(glm::vec2(x, y - 2));
			elements.push_back(GameSector((*gamefield)[x + 1.0][y - 2.0].WorldPosition, b));
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
			movedY.push_back(0);
		}

		for (int i = 0; i < rotationMatrix.size(); i++) {
			for (int j = 0; j < rotationMatrix[i].size(); j++) {
				rotationMatrix[i][j] = (*gamefield)[i + 3][j + 17].WorldPosition;
			}
		}
	}

	bool isMoveable(int xDirection, int yDirection) {
		for (int i = 0; i < currentIndex.size(); i++) {
			int x = currentIndex[i].x + xDirection;
			int y = currentIndex[i].y + yDirection;
			if ((*gamefield)[x][y].blocked) {
				return false;
			}
		}
		return true;
	}
	float speed = 20.0f;
	std::vector<int> moved;
	void move(glm::mat4 *model, float *direction, int index) {
		if (!isMoveable(*direction, 0)) {
			*direction = 0;
			return;
		}
		if (*direction != 0.0f && moved[index] < speed) {
			moved[index]++;
			*model = glm::translate(*model, glm::vec3(1.0, 0.0, 0.0) * 1.0f / speed * *direction);
		}
		else {
			moved[index] = 0;
			addToIndex((int)*direction, false);
			*direction = 0.0;
		}
	}
	std::vector<int> movedY;
	void moveDown(glm::mat4* model, int index, bool *shouldMoveDown,float *direction) {
		bool moveable = isMoveable(0, -1.0f);
		if (moveable && *shouldMoveDown && movedY[index] < speed) {
			movedY[index]++;
			*model = glm::translate(*model, glm::vec3(0.0, 1.0, 0.0) * 1.0f / speed * -1.0f);
		}
		else if(movedY[index]==speed) {
			currentIndex[index].y--;
			movedY[index] = 0;
		}
		else if(!moveable){
			*shouldMoveDown = false;
			*direction = 0.0;
			reachedBottom = true;
			addNewTetrominoToGamefield();
		}
	}
	void addToIndex(int n, bool moveDown) {
		if (n == 0) return;
		for (int i = 0; i < currentIndex.size(); i++) {
			currentIndex[i].x += n * !moveDown;
		}
	}
	void addNewTetrominoToGamefield() {
		for (int i = 0; i < currentIndex.size(); i++) {
			int x = currentIndex[i].x;
			int y = currentIndex[i].y;
			(*gamefield)[x][y].block = elements[i].block;
			(*gamefield)[x][y].blocked = true;
		}
	}
	void draw(float *direction, float r_speed, bool *shouldMoveDown) {
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
			if (*shouldMoveDown) {
				moveDown(&model, i, shouldMoveDown, direction);
			}
			else {
				move(&model, direction, i);
			}

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
