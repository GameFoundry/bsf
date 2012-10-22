#pragma once

#include "CmPrerequisites.h"

namespace CamelotEngine
{
	class Material
	{
		// Required for initialization
		void setShader(ShaderPtr shader);

		void setTexture(const String& name, TexturePtr value);
		void setFloat(const String& name, float value);
		void setColor(const String& name, const Color& value);
		void setVec(const String& name, const Vector2& value);
		void setVec(const String& name, const Vector3& value);
		void setVec(const String& name, const Vector4& value);
		void setMat(const String& name, const Matrix3& value);
		void setMat(const String& name, const Matrix4& value);

		/**
		 * @brief	Use the provided pass for rendering of all following objects.
		 */
		void applyPass(UINT32 passIdx);

		UINT32 getNumPasses() const;

	private:
		ShaderPtr mShader;

		void throwIfNotInitialized() const;
	};
}