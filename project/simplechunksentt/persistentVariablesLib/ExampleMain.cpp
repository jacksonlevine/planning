#include "pvarslib.hpp"
#include <stdlib.h>
#include <iostream>

int main()
{
	PVarsContext::tableName = std::string("jlvariables");
	setDbVariable("Hoopy", "Fazoole");
}