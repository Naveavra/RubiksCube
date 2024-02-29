#include "game.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

glm::vec3 transPosition;
float angle = 0.0f;
const int MAX_FRAMES = 1800;
int framesForCubeRotation = MAX_FRAMES;
int framesForWallRotation = MAX_FRAMES;
int cubesToRotate[] = { -2, -2, -2 };

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

	float distance = 1;
	int index = 0;

	for (float i = -distance; i <= distance; i += 1)
	{
		for (float j = -distance; j <= distance; j += 1)
		{
			for (float k = -distance; k <= distance; k += 1)
			{
				AddShape(Scene::Cube, -1, Scene::TRIANGLES);
				SetShapeTex(index, 0);
				shapes[index]->MyTranslate(glm::vec3(i, j, k), 0);
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
	if (framesForCubeRotation < MAX_FRAMES) {
		rotateCube();
	}
	if (framesForWallRotation < MAX_FRAMES)
	{
		rotateWall();
	}
	s->Unbind();
}


/// <summary>
///  For each cube, it calculates the rotation axis. This is done by multiplying transPosition with the current rotation matrix of the cube (shapes[idx]->getRot()).
///  This calculation determines the axis around which the cube will rotate.
/// </summary>
void Game::rotateCube()
{
	for (int idx = 0; idx < 27; idx++) {
		glm::vec3 rotationAxis = transPosition * glm::mat3(shapes[idx]->getRot());
		shapes[idx]->MyRotate(angle, rotationAxis, 0);
	}
	framesForCubeRotation++;
}

void Game::rotateWall()
{
	for (int idx = 0; idx < 27; idx++) {
		glm::mat4 transformationMatrix = shapes[idx]->MakeTrans(); //gets the tranformation matrix of each cube
		RoundMatrixElements(transformationMatrix); //ensure that small floating-point inaccuracies don't affect the logic determining whether a cube should rotate

		glm::vec3 shapePosition = glm::vec3(transformationMatrix[3][0], transformationMatrix[3][1], transformationMatrix[3][2]); //It is extracting the translation component of the transformation matrix.
		if (ShouldRotateShape(shapePosition, cubesToRotate)) {
			ApplyRotationToShape(shapes[idx], transPosition, angle);
		}
	}
	framesForWallRotation++;

}

glm::mat4 Game::RoundMatrixElements(const glm::mat4& matrix)
{
	glm::mat4 roundedMatrix;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			roundedMatrix[i][j] = round(matrix[i][j]);
		}
	}
	return roundedMatrix;
}

bool Game::ShouldRotateShape(const glm::vec3& position, const int cubesToRotate[3]) {
	bool checkX = cubesToRotate[0] > -2 ? position.x == cubesToRotate[0] : true;
	bool checkY = cubesToRotate[1] > -2 ? position.y == cubesToRotate[1] : true;
	bool checkZ = cubesToRotate[2] > -2 ? position.z == cubesToRotate[2] : true;

	return checkX && checkY && checkZ;
}

void Game::ApplyRotationToShape(Shape* shape, const glm::vec3& transPosition, float angleCur) {
	glm::vec3 rotationAxis = transPosition * glm::mat3(shape->getRot());
	shape->MyRotate(angleCur, rotationAxis, 0);
}

void Game::keyPressedEventHandler(float ang, glm::vec3 transPos, char key)
{
	if (framesForWallRotation == MAX_FRAMES && framesForCubeRotation == MAX_FRAMES) // ensuring sync between rotation 2000 is the frames number we chose
	{
		transPosition = transPos;
		angle = ang / MAX_FRAMES;
		framesForWallRotation = 0;
		switch (key)
		{
		case 'r':
			cubesToRotate[0] = 1;
			cubesToRotate[1] = -2;
			cubesToRotate[2] = -2;
			break;
		case 'l':
			cubesToRotate[0] = -1;
			cubesToRotate[1] = -2;
			cubesToRotate[2] = -2;
			break;
		case 'u':
			cubesToRotate[1] = 1;
			cubesToRotate[0] = -2;
			cubesToRotate[2] = -2;
			break;
		case 'd':
			cubesToRotate[1] = -1;
			cubesToRotate[0] = -2;
			cubesToRotate[2] = -2;
			break;
		case 'b':
			cubesToRotate[2] = -1;
			cubesToRotate[0] = -2;
			cubesToRotate[1] = -2;
			break;
		case 'f':
			cubesToRotate[2] = 1;
			cubesToRotate[0] = -2;
			cubesToRotate[1] = -2;
			break;
		default:
			break;
		}
	}
}

void Game::arrowPressedEventHandler(float ang, glm::vec3 transPos)
{
	if (framesForWallRotation == MAX_FRAMES && framesForCubeRotation == MAX_FRAMES) // ensuring sync between rotation 2000 is the frames number we chose
	{
		transPosition = transPos;
		angle = ang / MAX_FRAMES;
		framesForCubeRotation = 0;
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