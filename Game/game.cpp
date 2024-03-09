#include "game.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

glm::vec3 axisToRotate;
float angle = 0.0f;
const int MAX_FRAMES = 2000;
int framesForCubeRotation = MAX_FRAMES;
int cubesToRotate[] = { 0, 0, 0 };
float size = 3;
int numOfCubs = size * size * size;
int rotateFlag = 0;

static void printMat(const glm::mat4 mat)
{
	std::cout << " matrix:" << std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout << mat[j][i] << " ";
		std::cout << std::endl;
	}
}

Game::Game() : Scene()
{
}

Game::Game(float angle, float relationWH, float near1, float far1) : Scene(angle, relationWH, near1, far1)
{
}

void Game::Init()
{

	AddShader("../res/shaders/pickingShader");
	AddShader("../res/shaders/basicShader");

	AddTexture("../res/textures/plane.png", false);

	/*int size;
	std::cout << "Enter a number: ";
	std::cin >> size;*/

	int index = 0;

	for (float i = 0; i < size; i += 1)
	{
		for (float j = 0; j < size; j += 1)
		{
			for (float k = 0; k < size; k += 1)
			{
				AddShape(Scene::Cube, -1, Scene::TRIANGLES);
				SetShapeTex(index, 0);
				shapes[index]->MyTranslate(glm::vec3(i - (size - 1) / 2, j - (size - 1) / 2, k - (size - 1) / 2), 0);
				shapes[index]->MyScale(glm::vec3(0.5, 0.5, 0.5));
				index++;
			}
		}
	}
	pickedShape = 0;
	MoveCamera(0, zTranslate, 10);
	pickedShape = -1;

	//ReadPixel(); //uncomment when you are reading from the z-buffer
}

void Game::Update(const glm::mat4& MVP, const glm::mat4& Model, const int  shaderIndx)
{
	Shader* s = shaders[shaderIndx];
	int r = ((pickedShape + 1) & 0x000000FF) >> 0;
	int g = ((pickedShape + 1) & 0x0000FF00) >> 8;
	int b = ((pickedShape + 1) & 0x00FF0000) >> 16;
	s->Bind();
	s->SetUniformMat4f("MVP", MVP);
	s->SetUniformMat4f("Normal", Model);
	s->SetUniform4f("lightDirection", 0.0f, 0.0f, -1.0f, 0.0f);
	if (shaderIndx == 0)
		s->SetUniform4f("lightColor", r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
	else
		s->SetUniform4f("lightColor", 0.7f, 0.8f, 0.1f, 1.0f);
	if (framesForCubeRotation < MAX_FRAMES && rotateFlag == 2) {
		rotateCube();
	}
	else if (framesForCubeRotation < MAX_FRAMES && rotateFlag == 1) {
		rotateWall();
	}
	else {
		rotateFlag = 0;
	}
	s->Unbind();
}


/// <summary>
///  For each cube, it calculates the rotation axis. This is done by multiplying axisToRotate with the current rotation matrix of the cube (shapes[idx]->getRot()).
///  This calculation determines the axis around which the cube will rotate.
/// </summary>
void Game::rotateCube()
{
	for (int idx = 0; idx < numOfCubs; idx++) {
		glm::vec3 rotationAxis = axisToRotate * glm::mat3(shapes[idx]->getRot());
		shapes[idx]->MyRotate(angle, rotationAxis, 0);
	}
	framesForCubeRotation++;
}

void Game::rotateWall()
{
	for (int idx = 0; idx < numOfCubs; idx++) {
		glm::mat4 transformationMatrix = shapes[idx]->MakeTrans(); //gets the tranformation matrix of each cube
		ronudMatValues(transformationMatrix);
		glm::vec3 translationVec = glm::vec3(transformationMatrix[3]); //It is extracting the translation component of the transformation matrix.
		if (ShouldRotateShape(translationVec)) {
			ApplyRotationToShape(shapes[idx], axisToRotate, angle);
		}
	}
	framesForCubeRotation++;

}

void Game::ronudMatValues(glm::mat4& mat)
{
	for (int i = 0; i < mat.length(); i++)
	{
		for (int j = 0; j < mat.length(); j++)
		{
			mat[i][j] = round(mat[i][j]);
		}
	}
}

bool Game::ShouldRotateShape(const glm::vec3& position) {
	bool checkX = cubesToRotate[0] != 0 ? position.x == cubesToRotate[0] : true;
	bool checkY = cubesToRotate[1] != 0 ? position.y == cubesToRotate[1] : true;
	bool checkZ = cubesToRotate[2] != 0 ? position.z == cubesToRotate[2] : true;

	return checkX && checkY && checkZ;
}

void Game::ApplyRotationToShape(Shape* shape, const glm::vec3& axisToRotate, float angleCur) {
	glm::vec3 rotationAxis = axisToRotate * glm::mat3(shape->getRot());
	shape->MyRotate(angleCur, rotationAxis, 0);
}

void Game::keyPressedEventHandler(float ang, glm::vec3 transPos, char key)
{
	if (framesForCubeRotation == MAX_FRAMES) // ensuring sync between rotation 2000 is the frames number we chose
	{
		axisToRotate = transPos;
		angle = ang / MAX_FRAMES;
		framesForCubeRotation = 0;
		if (key != 'a')
		{
			std::unordered_map<char, glm::ivec3> keyMap = {
			{'r', {1, 0, 0}},
			{'l', {-1, 0, 0}},
			{'u', {0, 1, 0}},
			{'d', {0, -1, 0}},
			{'b', {0, 0, -1}},
			{'f', {0, 0, 1}}
			};

			auto pressedKey = keyMap.find(key);
			if (pressedKey != keyMap.end()) {
				cubesToRotate[0] = pressedKey->second.x;
				cubesToRotate[1] = pressedKey->second.y;
				cubesToRotate[2] = pressedKey->second.z;
			}
			rotateFlag = 1;
			return;
		}
		rotateFlag = 2;
		
	}
}

void Game::WhenRotate()
{
}

void Game::WhenTranslate()
{
}

void Game::scrollEventHandler(float xoffsset, float yoffset)
{
	MoveCamera(0, zTranslate, yoffset * -1);
}

void Game::Motion()
{
	if (isActive)
	{
	}
}

Game::~Game(void)
{
}