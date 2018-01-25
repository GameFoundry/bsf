//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Renderer/BsParamBlocks.h"
#include "RenderAPI/BsGpuParam.h"

namespace bs { namespace ct
{
	ParamBlock::~ParamBlock()
	{
		ParamBlockManager::unregisterBlock(this);
	}

	Vector<ParamBlock*> ParamBlockManager::sToInitialize;

	ParamBlockManager::ParamBlockManager()
	{
		for (auto& entry : sToInitialize)
			entry->initialize();

		sToInitialize.clear();
	}

	void ParamBlockManager::registerBlock(ParamBlock* paramBlock)
	{
		if (isStarted())
			paramBlock->initialize();
		else
			sToInitialize.push_back(paramBlock);
	}

	void ParamBlockManager::unregisterBlock(ParamBlock* paramBlock)
	{
		auto iterFind = std::find(sToInitialize.begin(), sToInitialize.end(), paramBlock);
		if (iterFind != sToInitialize.end())
			sToInitialize.erase(iterFind);
	}
}}