#pragma once

#include "BsCorePrerequisites.h"
#include "BsImportOptions.h"

namespace BansheeEngine
{
	/** @addtogroup Importer
	 *  @{
	 */

	/**
	 * Contains import options you may use to control how is a mesh imported from some external format into engine format.
	 */
	class BS_CORE_EXPORT MeshImportOptions : public ImportOptions
	{
	public:
		MeshImportOptions();

		/**	Sets whether the texture data is also stored in CPU memory. */
		void setCPUReadable(bool readable) { mCPUReadable = readable; }
		
		/**	Retrieves whether the texture data is also stored in CPU memory. */
		bool getCPUReadable() const { return mCPUReadable; }

		/**	Sets a value that controls should mesh normals be imported if available. */
		void setImportNormals(bool import) { mImportNormals = import; }

		/**	Retrieves a value that controls should mesh normals be imported if available. */
		bool getImportNormals() const { return mImportNormals; }

		/**	Sets a value that controls should mesh tangents/bitangents be imported if available. */
		void setImportTangents(bool import) { mImportTangents = import; }

		/**	Retrieves a value that controls should mesh tangent/bitangent be imported if available. */
		bool getImportTangents() const { return mImportTangents; }

		/**	Sets a value that controls should mesh blend shapes be imported	if available. */
		void setImportBlendShapes(bool import) { mImportBlendShapes = import; }

		/**	Retrieves a value that controls should mesh blend shapes be imported if available. */
		bool getImportBlendShapes() const { return mImportBlendShapes; }

		/**	
		 * Sets a value that controls should mesh skin data like bone weights, indices and bind poses be imported if 
		 * available.
		 */
		void setImportSkin(bool import) { mImportSkin = import; }

		/**
		 * Retrieves a value that controls should mesh skin data like bone weights, indices and bind poses be imported if 
		 * available.
		 */
		bool getImportSkin() const { return mImportSkin; }

		/** Sets a value that controls should animation clips be imported if available. */
		void setImportAnimation(bool import) { mImportAnimation = import; }

		/**	Retrieves a value that controls should animation clips be imported if available. */
		bool getImportAnimation() const { return mImportAnimation; }

		/**	Sets a value that will uniformly scale the imported mesh by the specified value. */
		void setImportScale(float import) { mImportScale = import; }

		/**	Retrieves a value that will uniformly scale the imported mesh by the specified value. */
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

	/** @} */
}