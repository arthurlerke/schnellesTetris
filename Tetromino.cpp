#include "Tetromino.h"

	Tetromino::Tetromino(std::vector < std::vector<GameSector> >* gf, std::vector <Block> blocks) {
		gamefield = gf;
		width = gamefield->size();
		height = (*gamefield)[0].size();

		createRandomBlock(blocks);
		setupVariables();
	}

	Tetromino::Tetromino(std::vector < std::vector<GameSector> >* gf, std::vector<GameSector> blocksToMoveDown) {
		gamefield = gf;
		width = gamefield->size();
		height = (*gamefield)[0].size();

		createFigurFromVector(blocksToMoveDown);
		setupVariables();
	}

	bool Tetromino::isMoveable(int xDirection, int yDirection) {
		for (int i = 0; i < currentIndex.size(); i++) {
			int x = currentIndex[i].x + xDirection;
			int y = currentIndex[i].y + yDirection;
			if ((*gamefield)[x][y].blocked) {
				return false;
			}
		}
		return true;
	}

	void Tetromino::move(glm::mat4* model, float* m_direction, int index) {
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

	void Tetromino::moveDown(glm::mat4* model, int index, bool* shouldMoveDown, float* m_direction) {
		bool moveable = isMoveable(0, -1.0f);
		if (moveable && *shouldMoveDown && movedY[index] < speed) {
			movedY[index]++;
			*model = glm::translate(*model, glm::vec3(0.0, 1.0, 0.0) * (1.0f / speed) * -1.0f);
		}
		else if (movedY[index] == speed) {
			currentIndex[index].y--;
			movedY[index] = 0;
		}
		else if (!moveable) {
			*shouldMoveDown = false;
			*m_direction = 0.0;
			reachedBottom = true;
			addNewTetrominoToGamefield();
			checkForFullRow();
		}
	}

	void Tetromino::rotate(glm::mat4 *transform, float *r_direction, int index) {
		if (*r_direction != 0.0f && rotated[index] < maxAngle) {
			rotated[index] += r_speed;
			glm::vec3 pivotTemp = pivot - elements[index].WorldPosition;
			*transform = glm::translate(*transform, pivotTemp);
			*transform = glm::rotate(*transform, glm::radians(*r_direction * r_speed), glm::vec3(0.0f, 0.0f, 1.0f));
			*transform = glm::translate(*transform, -pivotTemp);
		}
		else {
			rotated[index] = 0.0f;
			rotateIndex(-1 * *r_direction);
			*r_direction = 0.0f;
		}
	}

	void Tetromino::rotateIndex(float r_direction) {
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

	void Tetromino::addToIndex(int n, bool moveDown) {
		if (n == 0) return;
		for (int i = 0; i < currentIndex.size(); i++) {
			currentIndex[i].x += n * !moveDown;
		}
	}
	void Tetromino::addNewTetrominoToGamefield() {
		for (int i = 0; i < currentIndex.size(); i++) {
			int x = currentIndex[i].x;
			int y = currentIndex[i].y;
			(*gamefield)[x][y].block = elements[i].block;
			(*gamefield)[x][y].blocked = true;
		}
	}

	void Tetromino::checkForFullRow() {
		int count = 0;
		std::vector <int> numbersY;
		for (int i = 0; i < currentIndex.size(); i++) {
			for (int j = 1; j < width - 1; j++) {
				if ((*gamefield)[j][currentIndex[i].y].blocked) {
					count++;
				}
			}

			if (count == width - 2) {
				numbersY.push_back(currentIndex[i].y);
				for (int j = 1; j < width - 1; j++) {
					GameSector gs((*gamefield)[j][currentIndex[i].y].WorldPosition);
					gs.blocked = false;
					(*gamefield)[j][currentIndex[i].y] = gs;
				}
			}
			count = 0;
		}
		int max = getMaxNumber(numbersY);
		if (max != -1) {
			moveBlocksAboveDown(max);
		}

	}

	int Tetromino::getMaxNumber(std::vector<int> numbers) {
		if (numbers.size() == 0)
			return -1;
		int max = numbers[0];
		for (int i = 1; i < numbers.size(); i++) {
			max = std::max(max, numbers[i]);
		}
		return max;
	}

	void Tetromino::draw(float* m_direction, float* r_direction, bool * isMovingDown) {
		for (int i = 0; i < models.size(); i++) {
			glm::mat4 model = models[i];
			glm::mat4 transform = transforms[i];
			pivot = elements[pivotIndex].WorldPosition;
			shouldMoveDown = isMovingDown;
			moveDirection = m_direction;
			if (*shouldMoveDown) {
				moveDown(&model, i, shouldMoveDown, m_direction);
			}
			else {
				move(&model, m_direction, i);
				rotate(&transform, r_direction, i);
			}
			if (models.size() > i) {
				models[i] = model;
				transforms[i] = transform;
				elements[i].block.draw(model, transform);
			//pivot = elements[pivotIndex].WorldPosition;
			}
		}
	}
	void Tetromino::moveBlocksAboveDown(int max) {
		//create vector of blocks
		//than create tetromino from these blocks
		std::vector<GameSector> blocks;

		//Minus 1 because the most right block is the wall
		for (int i = 1; i < (*gamefield).size()-1; i++) {
			for (int j = max + 1; j < (*gamefield)[0].size()-1; j++) {
				if ((*gamefield)[i][j].blocked) {
					blocks.push_back((*gamefield)[i][j]);
					GameSector gs((*gamefield)[i][j].WorldPosition);
					gs.blocked = false;
					(*gamefield)[i][j] = gs;
				}
			}
		}
		createFigurFromVector(blocks);
		setupVariables();
		reachedBottom = false;
		*shouldMoveDown = true;
		*moveDirection = -1.0f;
	}

	bool Tetromino::checkCollision(float direction) {
		return false;
	}
	float Tetromino::getAngle(bool rotate) {
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

	void Tetromino::createRandomBlock(std::vector <Block> blocks) {
		std::random_device rd;
		std::mt19937 generator(rd());
		std::uniform_int_distribution<> colorRange(0, blocks.size() - 1);
		std::uniform_int_distribution<> formRange(0, 3);
		Block blockColor = blocks[colorRange(generator)];
		int randomForm = formRange(generator);

		int x = gamefield->size() / 2;
		int y = (*gamefield)[0].size() - 2;

		randomForm = 3;
		//L
		switch (randomForm) {
		case 0:
			createLBlock(x, y, blockColor);
			break;
		case 1:
			createSquareBlock(x, y, blockColor);
			break;
		case 2:
			createTBlock(x, y, blockColor);
			break;
		case 3:
			createLongBlock(x, y, blockColor);
			break;
		default:
			break;
		}
	}
	void Tetromino::createLBlock(int x, int y, Block blockColor) {
		std::vector<glm::vec2> positions = { {x,y}, {x,y - 1}, {x, y - 2}, {x + 1,y - 2} };
		pivotIndex = 1;
		createBlock(blockColor, positions);
	}
	void Tetromino::createSquareBlock(int x, int y, Block blockColor) {

		std::vector<glm::vec2> positions = { {x,y}, {x + 1,y}, {x, y - 1}, {x + 1,y - 1} };
		pivotIndex = 0;
		createBlock(blockColor, positions);
	}
	void Tetromino::createTBlock(int x, int y, Block blockColor) {

		std::vector<glm::vec2> positions = { {x,y}, {x - 1,y - 1}, {x, y - 1}, {x + 1,y - 1} };
		pivotIndex = 2;
		createBlock(blockColor, positions);
	}
	void Tetromino::createLongBlock(int x, int y, Block blockColor) {

		std::vector<glm::vec2> positions = { {x,y}, {x,y - 1}, {x, y - 2}, {x,y - 3} };
		pivotIndex = 1;
		createBlock(blockColor, positions);
	}

	void Tetromino::createBlock(Block blockColor, std::vector<glm::vec2> positions) {
		elements.push_back(GameSector((*gamefield)[positions[0].x][positions[0].y].WorldPosition, blockColor));
		elements.push_back(GameSector((*gamefield)[positions[1].x][positions[1].y].WorldPosition, blockColor));
		elements.push_back(GameSector((*gamefield)[positions[2].x][positions[2].y].WorldPosition, blockColor));
		elements.push_back(GameSector((*gamefield)[positions[3].x][positions[3].y].WorldPosition, blockColor));
		pivot = elements[pivotIndex].WorldPosition;
	}

	void Tetromino::setupVariables() {
		models = std::vector<glm::mat4>();
		transforms = std::vector<glm::mat4>();
		currentIndex = std::vector<glm::vec2>();
		moved = std::vector<int>();
		movedY = std::vector<int>();
		rotated = std::vector<int>();
		for (int i = 0; i < elements.size(); i++) {
			models.push_back(glm::translate(glm::mat4(1.0f), elements[i].WorldPosition));
			transforms.push_back(glm::mat4(1.0f));
			float xPosition = elements[i].WorldPosition.x + (float)width / 2.0f;
			float yPosition = elements[i].WorldPosition.y + (float)height / 2.0f;
			currentIndex.push_back(glm::vec2(xPosition, yPosition));
			moved.push_back(0);
			movedY.push_back(0);
			rotated.push_back(0);
		}
	}

	void Tetromino::createFigurFromVector(std::vector<GameSector> blocksToMoveDown) {
		elements = blocksToMoveDown;
	}