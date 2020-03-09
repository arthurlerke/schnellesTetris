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
	int width;

	int size = 4;

	std::vector <glm::mat4> models;
	std::vector <glm::mat4> transforms;
	std::vector <GameSector> elements;
	std::vector <glm::vec3> currentPositions;
	std::vector <glm::vec2> currentIndex;

	std::vector<std::vector<GameSector>> *gamefield;

	glm::vec3 pivot;
	int pivotIndex;

	float rotation = 0.0f;
	float distance = 0.0f;

	bool reachedBottom = false;

	Tetromino(std::vector < std::vector<GameSector> > *gf, std::vector <Block> blocks, int c) {
		gamefield = gf;
		width = gamefield->size();
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
			elements.push_back(GameSector((*gamefield)[x][y - 1.0].WorldPosition, b));
			elements.push_back(GameSector((*gamefield)[x][y - 2.0].WorldPosition, b));
			elements.push_back(GameSector((*gamefield)[x + 1.0][y - 2.0].WorldPosition, b));
			pivotIndex = 1;
			pivot = elements[pivotIndex].WorldPosition;
		}

		//ander Figuren


		for (int i = 0; i < elements.size(); i++) {
			models.push_back(glm::translate(glm::mat4(1.0f), elements[i].WorldPosition));
			transforms.push_back(glm::mat4(1.0f));
			currentPositions.push_back(elements[i].WorldPosition);
			float xPosition = elements[i].WorldPosition.x + (float)width / 2.0f;
			float yPosition = elements[i].WorldPosition.y + (float)height / 2.0f;
			currentIndex.push_back(glm::vec2(xPosition, yPosition));
			moved.push_back(0);
			movedY.push_back(0);
			rotated.push_back(0);
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
	void move(glm::mat4 *model, float *m_direction, int index) {
		if (!isMoveable(*m_direction, 0)) {
			*m_direction = 0;
			return;
		}
		if (*m_direction != 0.0f && moved[index] < speed) {
			moved[index]++;
			*model = glm::translate(*model, glm::vec3(1.0, 0.0, 0.0) * (1.0f / speed) * *m_direction);
		}
		else {
			moved[index] = 0;
			addToIndex((int)*m_direction, false);
			*m_direction = 0.0;
		}
	}

	std::vector<int> movedY;
	void moveDown(glm::mat4* model, int index, bool *shouldMoveDown,float *m_direction) {
		bool moveable = isMoveable(0, -1.0f);
		if (moveable && *shouldMoveDown && movedY[index] < speed) {
			movedY[index]++;
			*model = glm::translate(*model, glm::vec3(0.0, 1.0, 0.0) * (1.0f / speed) * -1.0f);
		}
		else if(movedY[index]==speed) {
			currentIndex[index].y--;
			movedY[index] = 0;
		}
		else if(!moveable){
			*shouldMoveDown = false;
			*m_direction = 0.0;
			reachedBottom = true;
			addNewTetrominoToGamefield();
			checkForFullRow();
		}
	}

	float maxAngle = 90.0f;
	float r_speed = 1;
	std::vector<int> rotated;
	void rotate(glm::mat4 *transform, float *r_direction, int index) {
		if (*r_direction != 0.0f && rotated[index] < maxAngle) {
			rotated[index] += r_speed;
			glm::vec3 pivotTemp = pivot - elements[index].WorldPosition;
			*transform = glm::translate(*transform, pivotTemp);
			*transform = glm::rotate(*transform, glm::radians(*r_direction*r_speed), glm::vec3(0.0f, 0.0f, 1.0f));
			*transform = glm::translate(*transform, -pivotTemp);
		}
		else {
			rotated[index] = 0.0f;
			rotateIndex(-1 * *r_direction);
			*r_direction = 0.0f;
		}
	}

	void rotateIndex(float r_direction) {
		if (r_direction == 0.0) return;
		int px = currentIndex[pivotIndex].x;
		int py = currentIndex[pivotIndex].y;
		for (int i = 0; i < currentIndex.size(); i++) {
			int x = currentIndex[i].x;
			int y = currentIndex[i].y;
			int translatedX = x - px;
			int translatedY = y - py;
			int newX = translatedY;
			int newY = -translatedX;
			newX = (newX * r_direction) + px;
			newY = (newY * r_direction) + py;
			currentIndex[i].x = newX;
			currentIndex[i].y = newY;
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

	void checkForFullRow() {
		int count = 0;
		for (int i = 0; i < currentIndex.size(); i++) {
			for (int j = 1; j < width - 1; j++) {
				if ((*gamefield)[j][currentIndex[i].y].blocked == true) {
					count++;
				}
			}

			if (count == width - 2) {
				for (int j = 1; j < width - 1; j++) {
					std::cout << j;
					GameSector gs((*gamefield)[j][currentIndex[i].y].WorldPosition);
					gs.blocked = false;
					(*gamefield)[j][currentIndex[i].y] = gs;
				}
			}
			count = 0;
		}
	}

	void draw(float *m_direction, float *r_direction, bool *shouldMoveDown) {
		for (int i = 0; i < models.size(); i++) {
			glm::mat4 model = models[i];
			glm::mat4 transform = transforms[i];
			pivot = elements[pivotIndex].WorldPosition;

			if (*shouldMoveDown) {
				moveDown(&model, i, shouldMoveDown, m_direction);
			}
			else {
				move(&model, m_direction, i);
				rotate(&transform,r_direction,i);
			}

			models[i] = model;
			transforms[i] = transform;
			elements[i].block.draw(model, transform);
			//pivot = elements[pivotIndex].WorldPosition;
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

};

#endif
