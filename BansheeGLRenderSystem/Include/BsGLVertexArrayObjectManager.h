#pragma once

#include "BsGLPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Vertex array object that contains vertex buffer object bindings and vertex
	 *			attribute pointers for a specific set of vertex buffers and a vertex declaration.
	 */
	class BS_RSGL_EXPORT GLVertexArrayObject
	{
	private:
		/**
		 * @brief	Generates hash value for the VAO object.
		 */
		class Hash
		{
		public:
			::std::size_t operator()(const GLVertexArrayObject& vao) const;
		};

		/**
		 * @brief	Checks if two VAO objects are equal.
		 */
		class Equal
		{
		public:
			bool operator()(const GLVertexArrayObject &a, const GLVertexArrayObject &b) const;
		};

	public:
		bool operator== (const GLVertexArrayObject& obj);
		bool operator!= (const GLVertexArrayObject& obj);

		/**
		 * @brief	Returns internal OpenGL VBO handle.
		 */
		GLuint getGLHandle() const { return mHandle;  }

	private:
		friend class GLVertexArrayObjectManager;

		GLVertexArrayObject();
		GLVertexArrayObject(GLuint handle, UINT64 vertexProgramId, GLVertexBufferCore** attachedBuffers, UINT32 numBuffers);

		GLuint mHandle;
		UINT64 mVertProgId;
		GLVertexBufferCore** mAttachedBuffers;
		UINT32 mNumBuffers;
	};

	/**
	 * @brief	Manager that handles creation and destruction of vertex array objects.
	 */
	class BS_RSGL_EXPORT GLVertexArrayObjectManager : public Module<GLVertexArrayObjectManager>
	{
	public:
		~GLVertexArrayObjectManager();

		/**
		 * @brief	Attempts to find an existing vertex array object matching the provided set of vertex buffers,
		 *			vertex declaration, and vertex shader input parameters. If one cannot be found new one is created
		 *			and returned.
		 *
		 *			Lifetime of returned VAO is managed by the vertex buffers that it binds.
		 */
		const GLVertexArrayObject& getVAO(const GLSLGpuProgramPtr& vertexProgram, 
			const VertexDeclarationPtr& vertexDecl, const Vector<SPtr<VertexBufferCore>>& boundBuffers);

		/**
		 * @brief	Called when a vertex buffer containing the provided VAO is destroyed.
		 */
		void notifyBufferDestroyed(GLVertexArrayObject vao);
	private:
		typedef UnorderedSet<GLVertexArrayObject, GLVertexArrayObject::Hash, GLVertexArrayObject::Equal> VAOMap;

		VAOMap mVAObjects;
	};
}