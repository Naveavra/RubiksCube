#include "game.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

glm::vec3 transPosition;
float currentAngle = 0.0f;
const int MAX_FRAMES = 2000;
int framesForWallRotation = MAX_FRAMES;
int framesForRowRotation = MAX_FRAMES;
int tmpPlaces[] = {-2, -2, -2};

static void printMat(const glm::mat4 mat)
{
	std::cout<<" matrix:"<<std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout<< mat[j][i]<<" ";
		std::cout<<std::endl;
	}
}

Game::Game() : Scene()
{
}

Game::Game(float angle ,float relationWH, float near1, float far1) : Scene(angle,relationWH,near1,far1)
{ 	
}

void Game::Init()
{		

	AddShader("../res/shaders/pickingShader");	
	AddShader("../res/shaders/basicShader");
	
	AddTexture("../res/textures/plane.png", false);

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
	MoveCamera(0,zTranslate,10);
	pickedShape = -1;
	
	//ReadPixel(); //uncomment when you are reading from the z-buffer
}

void Game::Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx)
{
	Shader *s = shaders[shaderIndx];
	int r = ((pickedShape+1) & 0x000000FF) >>  0;
	int g = ((pickedShape+1) & 0x0000FF00) >>  8;
	int b = ((pickedShape+1) & 0x00FF0000) >> 16;
	s->Bind();
	s->SetUniformMat4f("MVP", MVP);
	s->SetUniformMat4f("Normal",Model);
	s->SetUniform4f("lightDirection", 0.0f , 0.0f, -1.0f, 0.0f);
	if(shaderIndx == 0)
		s->SetUniform4f("lightColor",r/255.0f, g/255.0f, b/255.0f,1.0f);
	else 
		s->SetUniform4f("lightColor",0.7f,0.8f,0.1f,1.0f);
	if (framesForWallRotation < MAX_FRAMES) {
		WallRotate();
	}
	if (framesForRowRotation < MAX_FRAMES)
	{
		RowRotate();
	}
	s->Unbind();
}

void Game::WallRotate()
{
	//this loop is what given the cube the animation like rotations
	for (int idx = 0; idx < 27; idx++) {
		glm::vec3 tmpPosVec = transPosition * glm::mat3(shapes[idx]->getRot());
		shapes[idx]->MyRotate(currentAngle, tmpPosVec, 0);
	}
	framesForWallRotation++;
}

void Game::RowRotate()
{
	for (int idx = 0; idx < 27; idx++) {
		glm::mat4 transformationMatrix = shapes[idx]->MakeTrans();
		RoundMatrixElements(transformationMatrix);

		glm::vec3 shapePosition = glm::vec3(transformationMatrix[3][0], transformationMatrix[3][1], transformationMatrix[3][2]);
		if (ShouldRotateShape(shapePosition, tmpPlaces)) {
			ApplyRotationToShape(shapes[idx], transPosition, currentAngle);
		}
	}
	framesForRowRotation++;

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

bool Game::ShouldRotateShape(const glm::vec3& position, const int tmpPlaces[3]) {
	for (int i = 0; i < 3; i++) {
		if (tmpPlaces[i] > -2 && position[i] == tmpPlaces[i]) {
			return true;
		}
	}
	return false;
}

void Game::ApplyRotationToShape(Shape* shape, const glm::vec3& transPosition, float angle) {
	glm::vec3 rotationAxis = transPosition * glm::mat3(shape->getRot());
	shape->MyRotate(angle, rotationAxis, 0);
}

void Game::keyPressedEventHandler(float angle, glm::vec3 transPos, char key)
{
	if (framesForWallRotation == MAX_FRAMES && framesForRowRotation == MAX_FRAMES) // ensuring sync between rotation 2000 is the frames number we chose
	{
		transPosition = transPos;
		currentAngle = angle / MAX_FRAMES;
		framesForRowRotation = 0;
		switch (key)
		{
			case 'r':
				tmpPlaces[0] = 1;
				tmpPlaces[1] = -2;
				tmpPlaces[2] = -2;
				break;
			case 'l':
				tmpPlaces[0] = -1;
				tmpPlaces[1] = -2;
				tmpPlaces[2] = -2;
				break;
			case 'u':
				tmpPlaces[1] = 1;
				tmpPlaces[0] = -2;
				tmpPlaces[2] = -2;
				break;
			case 'd':
				tmpPlaces[1] = -1;
				tmpPlaces[0] = -2;
				tmpPlaces[2] = -2;
				break;
			case 'b':
				tmpPlaces[2] = -1;
				tmpPlaces[0] = -2;
				tmpPlaces[1] = -2;
				break;
			case 'f':
				tmpPlaces[2] = 1;
				tmpPlaces[0] = -2;
				tmpPlaces[1] = -2;
				break;
			default:
				break;
		}
	}
}

void Game::arrowPressedEventHandler(float angle, glm::vec3 transPos)
{
	if (framesForWallRotation == MAX_FRAMES && framesForRowRotation == MAX_FRAMES) // ensuring sync between rotation 2000 is the frames number we chose
	{
		transPosition = transPos;
		currentAngle = angle / MAX_FRAMES;
		framesForWallRotation = 0;
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
	if(isActive)
	{
	}
}

Game::~Game(void)
{
}
