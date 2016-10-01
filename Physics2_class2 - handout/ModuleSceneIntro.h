#pragma once
#include "Module.h"
#include "p2List.h"
#include "Globals.h"


class PhysBody;
class BodyData;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();

public:

	p2List<BodyData*> cList;

	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* rick;
};
