#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "math.h"

#include "Box2D/Box2D/Box2D.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));

	// big static circle as "ground" in the middle of the screen
	int x = SCREEN_WIDTH / 2;
	int y = SCREEN_HEIGHT / 1.5f;
	int diameter = SCREEN_WIDTH / 2;

	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(diameter) * 0.5f;

	b2FixtureDef fixture;
	fixture.shape = &shape;
	b->CreateFixture(&fixture);

	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	return UPDATE_CONTINUE;
}

// 
update_status ModulePhysics::PostUpdate()
{
	// On space bar press, create a circle on mouse position
	

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
				// Draw circles ------------------------------------------------
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();
					App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
				}
				break;

				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->GetVertexCount();
					b2Vec2 prev, v;

					for(int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->GetVertex(i));
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

						prev = v;
					}

					v = b->GetWorldPoint(polygonShape->GetVertex(0));
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
				}
				break;

				// Draw chains contour -------------------------------------------
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = (b2ChainShape*)f->GetShape();
					b2Vec2 prev, v;

					for(int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
				}
				break;

				// Draw a single segment(edge) ----------------------------------
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					b2Vec2 v1, v2;

					v1 = b->GetWorldPoint(shape->m_vertex0);
					v1 = b->GetWorldPoint(shape->m_vertex1);
					App->renderer->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
				}
				break;
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

BodyData* ModulePhysics::CreateCircle(int x, int  y)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	float radius = PIXEL_TO_METERS(25);
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = radius;
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1;

	b->CreateFixture(&fixture);

	BodyData ret(b);

	return &ret;
}

void ModulePhysics::CreateRectangle(int x, int y)
{
	// TODO 1: When pressing 2, create a box on the mouse position

	b2BodyDef body;
	body.type = b2_dynamicBody;
	float radius = PIXEL_TO_METERS(25);
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape shape;
	shape.SetAsBox(1.0f, 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);


	// TODO 2: To have the box behave normally, set fixture's density to 1.0f
}

void ModulePhysics::CreateChain(int x, int y)
{
	// TODO 3: Create a chain shape using those vertices
	// remember to convert them from pixels to meters!
	//H from points
	int points[24] = {
		-38, 80,
		-44, -54,
		-16, -60,
		-16, -17,
		19, -19,
		19, -79,
		61, -77,
		57, 73,
		17, 78,
		20, 16,
		-25, 13,
		-9, 72
	};


	// Pivot 0, 0
	int rick_head[78] = {
		99, 133,
		89, 146,
		72, 150,
		55, 145,
		46, 134,
		34, 138,
		40, 126,
		22, 126,
		31, 115,
		9, 104,
		28, 91,
		0, 77,
		29, 63,
		13, 36,
		42, 39,
		43, 25,
		41, 0,
		64, 18,
		75, 31,
		82, 19,
		83, 14,
		88, 4,
		93, 15,
		95, 27,
		95, 40,
		114, 34,
		105, 59,
		107, 63,
		119, 67,
		110, 75,
		112, 79,
		112, 86,
		107, 94,
		112, 99,
		104, 107,
		103, 115,
		107, 121,
		107, 126,
		100, 128
	};

	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2Vec2 chainPoints[39];

	for (int i = 0, j = 0; i < 39; i++, j += 2)
		chainPoints[i].Set(PIXEL_TO_METERS(rick_head[j]), PIXEL_TO_METERS(rick_head[j + 1]));


	b2ChainShape chainShape;
	chainShape.CreateLoop(chainPoints, 39);

	b2FixtureDef fixture;
	fixture.shape = &chainShape;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

}

BodyData::BodyData(b2Body* body) : body(body)
{
}

BodyData::~BodyData()
{
}

void BodyData::GetPosition(int& x, int& y)
{
	b2Vec2 ret;
	
	ret = body->GetPosition();
	
	x = METERS_TO_PIXELS(ret.x);
	y = METERS_TO_PIXELS(ret.y);

}
