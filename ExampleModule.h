#pragma once
#include <string>

#include "module.h"
#include "structures.h"

class ExampleModule : public module
{
public:
	ExampleModule(string name, int priority = 0);
	void onNotify(message* m);
};
