//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsCamera.h"

namespace BansheeEngine
{
	class BS_EXPORT CameraRTTI : public RTTIType <Camera, IReflectable, CameraRTTI>
	{
	private:
		ViewportPtr getViewport(Camera* obj) { return obj->mViewport; }
		void setViewport(Camera* obj, ViewportPtr val) { obj->mViewport = val; }

		UINT64& getLayers(Camera* obj) { return obj->mLayers; }
		void setLayers(Camera* obj, UINT64& val) { obj->mLayers = val; }

		UINT32& getFlags(Camera* obj) { return obj->mCameraFlags; }
		void setFlags(Camera* obj, UINT32& val) { obj->mCameraFlags = val; }

		Vector3& getPosition(Camera* obj) { return obj->mPosition; }
		void setPosition(Camera* obj, Vector3& val) { obj->mPosition = val; }

		Quaternion& getRotation(Camera* obj) { return obj->mRotation; }
		void setRotation(Camera* obj, Quaternion& val) { obj->mRotation = val; }

		ProjectionType& getProjType(Camera* obj) { return obj->mProjType; }
		void setProjType(Camera* obj, ProjectionType& val) { obj->mProjType = val; }

		Radian& getHorzFOV(Camera* obj) { return obj->mHorzFOV; }
		void setHorzFOV(Camera* obj, Radian& val) { obj->mHorzFOV = val; }

		float& getFarDist(Camera* obj) { return obj->mFarDist; }
		void setFarDist(Camera* obj, float& val) { obj->mFarDist = val; }

		float& getNearDist(Camera* obj) { return obj->mNearDist; }
		void setNearDist(Camera* obj, float& val) { obj->mNearDist = val; }

		float& getAspect(Camera* obj) { return obj->mAspect; }
		void setAspect(Camera* obj, float& val) { obj->mAspect = val; }

		float& getOrthoHeight(Camera* obj) { return obj->mOrthoHeight; }
		void setOrthoHeight(Camera* obj, float& val) { obj->mOrthoHeight = val; }

		INT32& getPriority(Camera* obj) { return obj->mPriority; }
		void setPriority(Camera* obj, INT32& val) { obj->mPriority = val; }

		bool& getCustomViewMatrix(Camera* obj) { return obj->mCustomViewMatrix; }
		void setCustomViewMatrix(Camera* obj, bool& val) { obj->mCustomViewMatrix = val; }

		bool& getCustomProjMatrix(Camera* obj) { return obj->mCustomProjMatrix; }
		void setCustomProjMatrix(Camera* obj, bool& val) { obj->mCustomProjMatrix = val; }

		bool& getFrustumManual(Camera* obj) { return obj->mFrustumExtentsManuallySet; }
		void setFrustumManual(Camera* obj, bool& val) { obj->mFrustumExtentsManuallySet = val; }

		Matrix4& getProjMatrix(Camera* obj) { return obj->mProjMatrix; }
		void setProjMatrix(Camera* obj, Matrix4& val) { obj->mProjMatrix = val; }

		Matrix4& getProjMatrixRS(Camera* obj) { return obj->mProjMatrixRS; }
		void setProjMatrixRS(Camera* obj, Matrix4& val) { obj->mProjMatrixRS = val; }

		Matrix4& getViewMatrix(Camera* obj) { return obj->mViewMatrix; }
		void setViewMatrix(Camera* obj, Matrix4& val) { obj->mViewMatrix = val; }

		float& getLeft(Camera* obj) { return obj->mLeft; }
		void setLeft(Camera* obj, float& val) { obj->mLeft = val; }

		float& getRight(Camera* obj) { return obj->mRight; }
		void setRight(Camera* obj, float& val) { obj->mRight = val; }

		float& getTop(Camera* obj) { return obj->mTop; }
		void setTop(Camera* obj, float& val) { obj->mTop = val; }

		float& getBottom(Camera* obj) { return obj->mBottom; }
		void setBottom(Camera* obj, float& val) { obj->mBottom = val; }

	public:
		CameraRTTI()
		{
			addReflectablePtrField("mViewport", 0, &CameraRTTI::getViewport, &CameraRTTI::setViewport);
			addPlainField("mLayers", 1, &CameraRTTI::getLayers, &CameraRTTI::setLayers);
			addPlainField("mPosition", 2, &CameraRTTI::getPosition, &CameraRTTI::setPosition);
			addPlainField("mRotation", 3, &CameraRTTI::getRotation, &CameraRTTI::setRotation);
			addPlainField("mProjType", 4, &CameraRTTI::getProjType, &CameraRTTI::setProjType);
			addPlainField("mHorzFOV", 5, &CameraRTTI::getHorzFOV, &CameraRTTI::setHorzFOV);
			addPlainField("mFarDist", 6, &CameraRTTI::getFarDist, &CameraRTTI::setFarDist);
			addPlainField("mNearDist", 7, &CameraRTTI::getNearDist, &CameraRTTI::setNearDist);
			addPlainField("mAspect", 8, &CameraRTTI::getAspect, &CameraRTTI::setAspect);
			addPlainField("mOrthoHeight", 9, &CameraRTTI::getOrthoHeight, &CameraRTTI::setOrthoHeight);
			addPlainField("mPriority", 10, &CameraRTTI::getPriority, &CameraRTTI::setPriority);
			addPlainField("mCustomViewMatrix", 11, &CameraRTTI::getCustomViewMatrix, &CameraRTTI::setCustomViewMatrix);
			addPlainField("mCustomProjMatrix", 12, &CameraRTTI::getCustomProjMatrix, &CameraRTTI::setCustomProjMatrix);
			addPlainField("mFrustumExtentsManuallySet", 13, &CameraRTTI::getFrustumManual, &CameraRTTI::setFrustumManual);
			addPlainField("mProjMatrixRS", 15, &CameraRTTI::getProjMatrixRS, &CameraRTTI::setProjMatrixRS);
			addPlainField("mProjMatrix", 16, &CameraRTTI::getProjMatrix, &CameraRTTI::setProjMatrix);
			addPlainField("mViewMatrix", 17, &CameraRTTI::getViewMatrix, &CameraRTTI::setViewMatrix);
			addPlainField("mLeft", 18, &CameraRTTI::getLeft, &CameraRTTI::setLeft);
			addPlainField("mRight", 19, &CameraRTTI::getRight, &CameraRTTI::setRight);
			addPlainField("mTop", 20, &CameraRTTI::getTop, &CameraRTTI::setTop);
			addPlainField("mBottom", 21, &CameraRTTI::getBottom, &CameraRTTI::setBottom);
			addPlainField("mFlags", 22, &CameraRTTI::getFlags, &CameraRTTI::setFlags);
		}

		virtual void onDeserializationEnded(IReflectable* obj) override
		{
			// Note: Since this is a CoreObject I should call initialize() right after deserialization,
			// but since this specific type is used in Components we delay initialization until Component
			// itself does it. Keep this is mind in case this ever needs to be deserialized for non-Component 
			// purposes (you'll need to call initialize manually).
		}

		virtual const String& getRTTIName() override
		{
			static String name = "Camera";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_Camera;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return Camera::createEmpty();
		}
	};
}