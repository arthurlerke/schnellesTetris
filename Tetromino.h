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
		Tetromino(std::vector < std::vector<GameSector> >* gf, std::vector <Block> blocks);
		Tetromino(std::vector < std::vector<GameSector> >* gf, std::vector<GameSector> blocksToMoveDown);
		void draw(float* m_direction, float* r_direction, bool* shouldMoveDown);
		bool reachedBottom = false;

	private:
		bool *shouldMoveDown;
		float* moveDirection;
		int height;
		int width;
		int size = 4;
		std::vector <glm::mat4> models;
		std::vector <glm::mat4> transforms;
		std::vector <GameSector> elements;
		std::vector <glm::vec3> currentPositions;
		std::vector <glm::vec2> currentIndex;
		std::vector<std::vector<GameSector>>* gamefield;
		glm::vec3 pivot;
		int pivotIndex;
		float rotation = 0.0f;
		float distance = 0.0f;
		float maxAngle = 90.0f;
		float r_speed = 1;
		std::vector<int> rotated;
		float speed = 20.0f;
		std::vector<int> moved;
		std::vector<int> movedY;
		float angleRot = 0;

		bool checkCollision(float direction);
		float getAngle(bool rotate);
		bool isMoveable(int xDirection, int yDirection);
		void move(glm::mat4* model, float* m_direction, int index);
		void moveDown(glm::mat4* model, int index, bool* shouldMoveDown, float* m_direction);
		void rotate(glm::mat4* transform, float* r_direction, int index);
		void rotateIndex(float r_direction);
		void addToIndex(int n, bool moveDown);
		void addNewTetrominoToGamefield();
		void checkForFullRow();
		void createRandomBlock(std::vector <Block> blocks);
		void createLBlock(int x, int y, Block blockColor);
		void createSquareBlock(int x, int y, Block blockColor);
		void createTBlock(int x, int y, Block blockColor);
		void createLongBlock(int x, int y, Block blockColor);
		void createBlock(Block blockColor, std::vector<glm::vec2> positions);
		int getMaxNumber(std::vector<int> numbers);
		void moveBlocksAboveDown(int max);
		void setupVariables();
		void createFigurFromVector(std::vector<GameSector> blocksToMoveDown);
};

#endif
