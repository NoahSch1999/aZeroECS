#pragma once
#include "../../src/aZeroECS.h"

struct ComponentOne
{
	int x;
};

struct ComponentTwo
{
	float x;
};

struct ComponentThree
{
	double x;
	ComponentThree() = default;
	ComponentThree(double InitData)
	{
		x = InitData;
	}
};

struct ComponentFour
{
	bool x;
};

// Typedef template specialization for ease-of-use
typedef aZero::ECS::ComponentManager<ComponentOne, ComponentTwo, ComponentThree, ComponentFour> ComponentManagerDecl;