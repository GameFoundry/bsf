#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsCameraHandler.h"

namespace BansheeEngine
{
	class BS_EXPORT CameraHandlerRTTI : public RTTIType <CameraHandler, IReflectable, CameraHandlerRTTI>
	{
	private:
		ViewportPtr getViewport(CameraHandler* obj) { return obj->mViewport; }
		void setViewport(CameraHandler* obj, ViewportPtr val) { obj->mViewport = val; }

		UINT64& getLayers(CameraHandler* obj) { return obj->mLayers; }
		void setLayers(CameraHandler* obj, UINT64& val) { obj->mLayers = val; }

		Vector3& getPosition(CameraHandler* obj) { return obj->mPosition; }
		void setPosition(CameraHandler* obj, Vector3& val) { obj->mPosition = val; }

		Quaternion& getRotation(CameraHandler* obj) { return obj->mRotation; }
		void setRotation(CameraHandler* obj, Quaternion& val) { obj->mRotation = val; }

		ProjectionType& getProjType(CameraHandler* obj) { return obj->mProjType; }
		void setProjType(CameraHandler* obj, ProjectionType& val) { obj->mProjType = val; }

		Radian& getHorzFOV(CameraHandler* obj) { return obj->mHorzFOV; }
		void setHorzFOV(CameraHandler* obj, Radian& val) { obj->mHorzFOV = val; }

		float& getFarDist(CameraHandler* obj) { return obj->mFarDist; }
		void setFarDist(CameraHandler* obj, float& val) { obj->mFarDist = val; }

		float& getNearDist(CameraHandler* obj) { return obj->mNearDist; }
		void setNearDist(CameraHandler* obj, float& val) { obj->mNearDist = val; }

		float& getAspect(CameraHandler* obj) { return obj->mAspect; }
		void setAspect(CameraHandler* obj, float& val) { obj->mAspect = val; }

		float& getOrthoHeight(CameraHandler* obj) { return obj->mOrthoHeight; }
		void setOrthoHeight(CameraHandler* obj, float& val) { obj->mOrthoHeight = val; }

		INT32& getPriority(CameraHandler* obj) { return obj->mPriority; }
		void setPriority(CameraHandler* obj, INT32& val) { obj->mPriority = val; }

		bool& getCustomViewMatrix(CameraHandler* obj) { return obj->mCustomViewMatrix; }
		void setCustomViewMatrix(CameraHandler* obj, bool& val) { obj->mCustomViewMatrix = val; }

		bool& getCustomProjMatrix(CameraHandler* obj) { return obj->mCustomProjMatrix; }
		void setCustomProjMatrix(CameraHandler* obj, bool& val) { obj->mCustomProjMatrix = val; }

		bool& getFrustumManual(CameraHandler* obj) { return obj->mFrustumExtentsManuallySet; }
		void setFrustumManual(CameraHandler* obj, bool& val) { obj->mFrustumExtentsManuallySet = val; }

		Matrix4& getProjMatrix(CameraHandler* obj) { return obj->mProjMatrix; }
		void setProjMatrix(CameraHandler* obj, Matrix4& val) { obj->mProjMatrix = val; }

		Matrix4& getProjMatrixRS(CameraHandler* obj) { return obj->mProjMatrixRS; }
		void setProjMatrixRS(CameraHandler* obj, Matrix4& val) { obj->mProjMatrixRS = val; }

		Matrix4& getViewMatrix(CameraHandler* obj) { return obj->mViewMatrix; }
		void setViewMatrix(CameraHandler* obj, Matrix4& val) { obj->mViewMatrix = val; }

		float& getLeft(CameraHandler* obj) { return obj->mLeft; }
		void setLeft(CameraHandler* obj, float& val) { obj->mLeft = val; }

		float& getRight(CameraHandler* obj) { return obj->mRight; }
		void setRight(CameraHandler* obj, float& val) { obj->mRight = val; }

		float& getTop(CameraHandler* obj) { return obj->mTop; }
		void setTop(CameraHandler* obj, float& val) { obj->mTop = val; }

		float& getBottom(CameraHandler* obj) { return obj->mBottom; }
		void setBottom(CameraHandler* obj, float& val) { obj->mBottom = val; }

	public:
		CameraHandlerRTTI()
		{
			addReflectablePtrField("mViewport", 0, &CameraHandlerRTTI::getViewport, &CameraHandlerRTTI::setViewport);
			addPlainField("mLayers", 1, &CameraHandlerRTTI::getLayers, &CameraHandlerRTTI::setLayers);
			addPlainField("mPosition", 2, &CameraHandlerRTTI::getPosition, &CameraHandlerRTTI::setPosition);
			addPlainField("mRotation", 3, &CameraHandlerRTTI::getRotation, &CameraHandlerRTTI::setRotation);
			addPlainField("mProjType", 4, &CameraHandlerRTTI::getProjType, &CameraHandlerRTTI::setProjType);
			addPlainField("mHorzFOV", 5, &CameraHandlerRTTI::getHorzFOV, &CameraHandlerRTTI::setHorzFOV);
			addPlainField("mFarDist", 6, &CameraHandlerRTTI::getFarDist, &CameraHandlerRTTI::setFarDist);
			addPlainField("mNearDist", 7, &CameraHandlerRTTI::getNearDist, &CameraHandlerRTTI::setNearDist);
			addPlainField("mAspect", 8, &CameraHandlerRTTI::getAspect, &CameraHandlerRTTI::setAspect);
			addPlainField("mOrthoHeight", 9, &CameraHandlerRTTI::getOrthoHeight, &CameraHandlerRTTI::setOrthoHeight);
			addPlainField("mPriority", 10, &CameraHandlerRTTI::getPriority, &CameraHandlerRTTI::setPriority);
			addPlainField("mCustomViewMatrix", 11, &CameraHandlerRTTI::getCustomViewMatrix, &CameraHandlerRTTI::setCustomViewMatrix);
			addPlainField("mCustomProjMatrix", 12, &CameraHandlerRTTI::getCustomProjMatrix, &CameraHandlerRTTI::setCustomProjMatrix);
			addPlainField("mFrustumExtentsManuallySet", 13, &CameraHandlerRTTI::getFrustumManual, &CameraHandlerRTTI::setFrustumManual);
			addPlainField("mProjMatrixRS", 15, &CameraHandlerRTTI::getProjMatrixRS, &CameraHandlerRTTI::setProjMatrixRS);
			addPlainField("mProjMatrix", 16, &CameraHandlerRTTI::getProjMatrix, &CameraHandlerRTTI::setProjMatrix);
			addPlainField("mViewMatrix", 17, &CameraHandlerRTTI::getViewMatrix, &CameraHandlerRTTI::setViewMatrix);
			addPlainField("mLeft", 18, &CameraHandlerRTTI::getLeft, &CameraHandlerRTTI::setLeft);
			addPlainField("mRight", 19, &CameraHandlerRTTI::getRight, &CameraHandlerRTTI::setRight);
			addPlainField("mTop", 20, &CameraHandlerRTTI::getTop, &CameraHandlerRTTI::setTop);
			addPlainField("mBottom", 21, &CameraHandlerRTTI::getBottom, &CameraHandlerRTTI::setBottom);
		}

		virtual void onDeserializationEnded(IReflectable* obj) override
		{
			CameraHandler* cameraHandler = static_cast<CameraHandler*>(obj);
			cameraHandler->initialize();
		}

		virtual const String& getRTTIName() override
		{
			static String name = "CameraHandler";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_CameraHandler;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return CameraHandler::createEmpty();
		}
	};
}