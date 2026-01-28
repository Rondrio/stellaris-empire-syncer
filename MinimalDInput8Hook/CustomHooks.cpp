#include "stdafx.h"
#include "CustomHooks.h"
#include <stdio.h>
#include <iostream>
#include <libcurl>
void SetupHooks()
{
	// Create a console for Debug output
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

	std::cout << "Hello world";



	// Setup hooks here, see examples below
}

