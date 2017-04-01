#pragma once

#include "../Source/System/IElixirGame.h"

using namespace Elixir;

class PulseGame : public IElixirGame
{
public:
	PulseGame();

	//void StartScene();

	void InitTestScene();
	void UpdateTestScene(float dt);

	virtual void Init() override;
	virtual void Update(float dt) override;


};