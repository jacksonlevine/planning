#pragma once
#include "Model.hpp"



	class Tree {
	public:
		static Model getTreeModel(float x, float y, float z);
		static Model getPineTreeModel(float x, float y, float z);
	};

	class Rock {
	public:
		static Model getRockModel(float x, float y, float z);
	};

	class Plant {
	public:
		static Model getPricklyPearCactusModel(float x, float y, float z);
		static Model getSaguaroCactusModel(float x, float y, float z);
	};