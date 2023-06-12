#pragma once
#include "Model.hpp"
#include "BlockTypes.hpp"


	class Tree {
	public:
		static Model getTreeModel(float x, float y, float z);
		static Model getPineTreeModel(float x, float y, float z);
	};

	class Rock {
	public:
		static Model getRockModel(float x, float y, float z);
		static Model getSmallRockModel(float x, float y, float z);
	};

	class Plant {
	public:
		static Model getPricklyPearCactusModel(float x, float y, float z);
		static Model getSaguaroCactusModel(float x, float y, float z);
		static Model getCatTailsModel(float x, float y, float z);
		static Model getTallGrassModel(float x, float y, float z);
		static Model getShortGrassModel(float x, float y, float z);
		static Model getDeadTallGrassModel(float x, float y, float z);
		static Model getDeadShortGrassModel(float x, float y, float z);
		static Model getBlackberryBushModel(float x, float y, float z);
	};

	class General {
	public:
		static Model getCrossModel(float x, float y, float z, TextureFace face, int yOffset, bool flat);
		static Model getTallCrossModel(float x, float y, float z, TextureFace bottom, TextureFace top, int yOffset, bool flat);
	};