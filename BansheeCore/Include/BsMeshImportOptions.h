#pragma once

#include "BsCorePrerequisites.h"
#include "BsImportOptions.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains import options you may use to control how is a mesh imported
	 *			from some external format into engine format.
	 */
	class BS_CORE_EXPORT MeshImportOptions : public ImportOptions
	{
	public:
		MeshImportOptions();

		/**
		 * @brief	Sets whether the texture data is also stored in CPU memory.
		 */
		void setCPUReadable(bool readable) { mCPUReadable = readable; }
		
		/**
		 * @brief	Retrieves whether the texture data is also stored in CPU memory.
		 */
		bool getCPUReadable() const { return mCPUReadable; }

		/**
		 * @brief	Sets a value that controls should mesh normals be imported
		 *			if available.
		 */
		void setImportNormals(bool import) { mImportNormals = import; }

		/**
		 * @brief	Retrieves a value that controls should mesh normals be imported
		 *			if available.
		 */
		bool getImportNormals() const { return mImportNormals; }

		/**
		 * @brief	Sets a value that controls should mesh tangent/bitangent be imported
		 *			if available.
		 */
		void setImportTangents(bool import) { mImportTangents = import; }

		/**
		 * @brief	Retrieves a value that controls should mesh tangent/bitangent be imported
		 *			if available.
		 */
		bool getImportTangents() const { return mImportTangents; }

		/**
		 * @brief	Sets a value that controls should mesh blend shapes be imported
		 *			if available.
		 */
		void setImportBlendShapes(bool import) { mImportBlendShapes = import; }

		/**
		 * @brief	Retrieves a value that controls should mesh blend shapes be imported
		 *			if available.
		 */
		bool getImportBlendShapes() const { return mImportBlendShapes; }

		/**
		 * @brief	Sets a value that controls should mesh skin data like bone weights, 
		 *			indices and bind poses be imported if available.
		 */
		void setImportSkin(bool import) { mImportSkin = import; }

		/**
		 * @brief	Retrieves a value that controls should mesh skin data like bone weights, 
		 *			indices and bind poses be imported if available.
		 */
		bool getImportSkin() const { return mImportSkin; }

		/**
		 * @brief	Sets a value that controls should animation clips be imported if available.
		 */
		void setImportAnimation(bool import) { mImportAnimation = import; }

		/**
		 * @brief	Retrieves a value that controls should animation clips be imported if available.
		 */
		bool getImportAnimation() const { return mImportAnimation; }

		/**
		 * @brief	Sets a value that will uniformly scale the imported mesh by the specified value.
		 */
		void setImportScale(float import) { mImportScale = import; }

		/**
		 * @brief	Retrieves a value that will uniformly scale the imported mesh by the specified value.
		 */
		float getImportScale() const { return mImportScale; }

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class MeshImportOptionsRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;

	private:
		bool mCPUReadable;
		bool mImportNormals;
		bool mImportTangents;
		bool mImportBlendShapes;
		bool mImportSkin;
		bool mImportAnimation;
		float mImportScale;
	};
}