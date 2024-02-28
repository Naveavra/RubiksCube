#pragma once
#include "scene.h"

class Game : public Scene
{
public:
	
	Game();
	Game(float angle,float relationWH,float near, float far);
	void Init();
	void Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx);
	void ControlPointUpdate();
	void WhenRotate();
	void WhenTranslate();
	void Motion();
	void arrowPressedEventHandler(float angle, glm::vec3 transPos);
	void scrollEventHandler(float xoffsset, float yoffset);
	void WallRotate();
	void RowRotate();
	glm::mat4 RoundMatrixElements(const glm::mat4& matrix);
	bool ShouldRotateShape(const glm::vec3& position, const int tmpPlaces[3]);
	void ApplyRotationToShape(Shape* shape, const glm::vec3& transPosition, float angle);
	void keyPressedEventHandler(float angle, glm::vec3 transPos, char key);
	~Game(void);
};

