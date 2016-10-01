#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "math.h"

#define METER_TO_PIXEL(a) a*10
#define PIXEL_TO_METER(a) 10*a

#define METER_TO_PIXELES(a) a/10

// TODO 1: Include Box 2 header and library

#include "Box2D\Box2D\Box2D.h"

#ifdef _DEBUG

#pragma comment (lib, "Box2D/libx86/Debug/Box2D.lib")

#endif // !_DEBUG

#ifndef _DEBUG

#pragma comment (lib, "Box2D/libx86/Release/Box2D.lib")

#endif // !_DEBUG




ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	// TODO 2: Create a private variable for the world
	// - You need to send it a default gravity
	// - You need init the world in the constructor
	// - Remember to destroy the world after using it
	b2Vec2 gravity(0.0f, 10.0f);
	world = new b2World(gravity);


	// TODO 4: Create a a big static circle as "ground"

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(PIXEL_TO_METER(5.0f), PIXEL_TO_METER(3.9f));

	b2Body* groundball = world->CreateBody(&bodyDef);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METER(3);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	
	groundball->CreateFixture(&fixture);



	//Box - Terrain
	b2BodyDef groundDef;
	groundDef.type = b2_staticBody;
	groundDef.position.Set(PIXEL_TO_METER(5.0f), PIXEL_TO_METER(5.0f));

	b2Body* ground = world->CreateBody(&groundDef);

	b2PolygonShape groundshape;
	groundshape.SetAsBox(0.1f, 0.1f);

	b2FixtureDef groundfixture;
	groundfixture.shape = &groundshape;

	ground->CreateFixture(&groundfixture);

	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	// TODO 3: Update the simulation ("step" the world)
	world->Step(1.0f / 60.0f, 8, 3);

	return UPDATE_CONTINUE;
}

// 
update_status ModulePhysics::PostUpdate()
{
	// TODO 5: On space bar press, create a circle on mouse position
	// - You need to transform the position / radius

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {

		int x = App->input->GetMouseX();
		int y = App->input->GetMouseY();

	
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(METER_TO_PIXELES(x), METER_TO_PIXELES(y));

		b2Body* ball = world->CreateBody(&bodyDef);

		b2CircleShape shape;
		shape.m_radius = PIXEL_TO_METER(0.25);

		b2FixtureDef fixture;
		fixture.shape = &shape;

		ball->CreateFixture(&fixture);
		
	}

	



	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	
	for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch(f->GetType())
			{
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();
					App->renderer->DrawCircle(METER_TO_PIXEL(pos.x), METER_TO_PIXEL(pos.y), METER_TO_PIXEL(shape->m_radius), 255, 255, 255);
				}
				break;

				// You will have to add more cases to draw boxes, edges, and polygons ...
			}
		}
	}

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}
