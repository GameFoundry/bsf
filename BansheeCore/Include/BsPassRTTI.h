//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsPass.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT PassRTTI : public RTTIType<Pass, IReflectable, PassRTTI>
	{
	private:
		HBlendState& getBlendState(Pass* obj) { return obj->mBlendState; }
		void setBlendState(Pass* obj, HBlendState& val) { obj->mBlendState = val; } 

		HRasterizerState& getRasterizerState(Pass* obj) { return obj->mRasterizerState; }
		void setRasterizerState(Pass* obj, HRasterizerState& val) { obj->mRasterizerState = val; } 

		HDepthStencilState& getDepthStencilState(Pass* obj) { return obj->mDepthStencilState; }
		void setDepthStencilState(Pass* obj, HDepthStencilState& val) { obj->mDepthStencilState = val; } 

		HGpuProgram& getVertexProgram(Pass* obj) { return obj->mVertexProgram; }
		void setVertexProgram(Pass* obj, HGpuProgram& val) { obj->mVertexProgram = val; } 

		HGpuProgram& getFragmentProgram(Pass* obj) { return obj->mFragmentProgram; }
		void setFragmentProgram(Pass* obj, HGpuProgram& val) { obj->mFragmentProgram = val; } 

		HGpuProgram& getGeometryProgram(Pass* obj) { return obj->mGeometryProgram; }
		void setGeometryProgram(Pass* obj, HGpuProgram& val) { obj->mGeometryProgram = val; } 

		HGpuProgram& getHullProgram(Pass* obj) { return obj->mHullProgram; }
		void setHullProgram(Pass* obj, HGpuProgram& val) { obj->mHullProgram = val; } 

		HGpuProgram& getDomainProgram(Pass* obj) { return obj->mDomainProgram; }
		void setDomainProgram(Pass* obj, HGpuProgram& val) { obj->mDomainProgram = val; } 

		HGpuProgram& getComputeProgram(Pass* obj) { return obj->mComputeProgram; }
		void setComputeProgram(Pass* obj, HGpuProgram& val) { obj->mComputeProgram = val; } 
	public:
		PassRTTI()
		{
			addReflectableField("mBlendState", 0, &PassRTTI::getBlendState, &PassRTTI::setBlendState);
			addReflectableField("mRasterizerState", 1, &PassRTTI::getRasterizerState, &PassRTTI::setRasterizerState);
			addReflectableField("mDepthStencilState", 2, &PassRTTI::getDepthStencilState, &PassRTTI::setDepthStencilState);

			addReflectableField("mVertexProgram", 3, &PassRTTI::getVertexProgram, &PassRTTI::setVertexProgram);
			addReflectableField("mFragmentProgram", 4, &PassRTTI::getFragmentProgram, &PassRTTI::setFragmentProgram);
			addReflectableField("mGeometryProgram", 5, &PassRTTI::getGeometryProgram, &PassRTTI::setGeometryProgram);
			addReflectableField("mHullProgram", 6, &PassRTTI::getHullProgram, &PassRTTI::setHullProgram);
			addReflectableField("mDomainProgram", 7, &PassRTTI::getDomainProgram, &PassRTTI::setDomainProgram);
			addReflectableField("mComputeProgram", 8, &PassRTTI::getComputeProgram, &PassRTTI::setComputeProgram);
		}

		virtual const String& getRTTIName()
		{
			static String name = "Pass";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_Pass;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return bs_shared_ptr<Pass, PoolAlloc>();
		}
	};
}