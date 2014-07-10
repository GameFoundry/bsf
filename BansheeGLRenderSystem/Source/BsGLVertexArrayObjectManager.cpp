//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGLVertexArrayObjectManager.h"
#include "BsGLVertexBuffer.h"
#include "BsVertexDeclaration.h"
#include "BsGLSLGpuProgram.h"
#include "BsGLHardwareBufferManager.h"
#include "BsUtil.h"
#include "BsRenderStats.h"

#define VBO_BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace BansheeEngine
{
	GLVertexArrayObject::GLVertexArrayObject()
		:mHandle(0), mVertProgId(0), mAttachedBuffers(nullptr), mNumBuffers(0)
	{ }

	GLVertexArrayObject::GLVertexArrayObject(GLuint handle, UINT64 vertexProgramId, 
		GLVertexBuffer** attachedBuffers, UINT32 numBuffers)
		:mHandle(handle), mVertProgId(vertexProgramId), mAttachedBuffers(attachedBuffers), mNumBuffers(numBuffers)
	{ }

	::std::size_t GLVertexArrayObject::Hash::operator()(const GLVertexArrayObject &vao) const
	{
		std::size_t seed = 0;
		hash_combine(seed, vao.mVertProgId);

		for (UINT32 i = 0; i < vao.mNumBuffers; i++)
			hash_combine(seed, vao.mAttachedBuffers[i]->getInternalID());

		return seed;
	}

	bool GLVertexArrayObject::Equal::operator()(const GLVertexArrayObject &a, const GLVertexArrayObject &b) const
	{
		if (a.mVertProgId != b.mVertProgId)
			return false;

		if (a.mNumBuffers != b.mNumBuffers)
			return false;

		for (UINT32 i = 0; i < a.mNumBuffers; i++)
		{
			if (a.mAttachedBuffers[i]->getInternalID() != b.mAttachedBuffers[i]->getInternalID())
				return false;
		}

		return true;
	}

	bool GLVertexArrayObject::operator== (const GLVertexArrayObject& obj)
	{
		if (mVertProgId != obj.mVertProgId)
			return false;

		if (mNumBuffers != obj.mNumBuffers)
			return false;

		for (UINT32 i = 0; i < mNumBuffers; i++)
		{
			if (mAttachedBuffers[i]->getInternalID() != obj.mAttachedBuffers[i]->getInternalID())
				return false;
		}

		return true;
	}

	bool GLVertexArrayObject::operator!= (const GLVertexArrayObject& obj)
	{
		return !operator==(obj);
	}

	GLVertexArrayObjectManager::~GLVertexArrayObjectManager()
	{
		assert(mVAObjects.size() == 0 && "VertexArrayObjectManager getting shut down but not all VA objects were released.");
	}

	const GLVertexArrayObject& GLVertexArrayObjectManager::getVAO(const GLSLGpuProgramPtr& vertexProgram,
		const VertexDeclarationPtr& vertexDecl, const Vector<VertexBufferPtr>& boundBuffers)
	{
		UINT32 numUsedBuffers = 0;
		INT32* streamToSeqIdx = stackAllocN<INT32>((UINT32)boundBuffers.size());
		GLVertexBuffer** usedBuffers = stackAllocN<GLVertexBuffer*>((UINT32)boundBuffers.size());
		
		const VertexDeclaration::VertexElementList& decl = vertexDecl->getElements();
		for (auto& elem : decl)
		{
			UINT16 streamIdx = elem.getStreamIdx();
			if (streamIdx >= (UINT32)boundBuffers.size())
			{
				streamToSeqIdx[streamIdx] = -1;
				continue;
			}

			VertexBufferPtr vertexBuffer = boundBuffers[streamIdx];

			if (vertexBuffer == nullptr)
				continue; // Skip unbound elements

			streamToSeqIdx[streamIdx] = (INT32)numUsedBuffers;
			usedBuffers[numUsedBuffers++] = static_cast<GLVertexBuffer*>(vertexBuffer.get());
		}
		
		GLVertexArrayObject wantedVAO(0, vertexProgram->getInternalID(), usedBuffers, numUsedBuffers);

		auto findIter = mVAObjects.find(wantedVAO);
		if (findIter != mVAObjects.end())
		{
			stackDeallocLast(usedBuffers);
			stackDeallocLast(streamToSeqIdx);

			return *findIter; // Found existing, return that
		}

		// Need to create new VAO
		const VertexDeclaration::VertexElementList& inputAttributes = vertexProgram->getInputAttributes().getElements();

		glGenVertexArrays(1, &wantedVAO.mHandle);
		glBindVertexArray(wantedVAO.mHandle);

		for (auto& elem : decl)
		{
			UINT16 streamIdx = elem.getStreamIdx();
			INT32 seqIdx = streamToSeqIdx[streamIdx];

			if (seqIdx == -1)
				continue;

			bool foundSemantic = false;
			GLint attribLocation = 0;
			for (auto iter = inputAttributes.begin(); iter != inputAttributes.end(); ++iter)
			{
				if (iter->getSemantic() == elem.getSemantic() && iter->getSemanticIdx() == elem.getSemanticIdx())
				{
					foundSemantic = true;
					attribLocation = iter->getOffset();
					break;
				}
			}

			if (!foundSemantic) // Shader needs to have a matching input attribute, otherwise we skip it
				continue;

			// TODO - We might also want to check the size of input and buffer, and make sure they match? Or does OpenGL handle that internally?

			GLVertexBuffer* vertexBuffer = usedBuffers[seqIdx];
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->getGLBufferId());
			void* bufferData = VBO_BUFFER_OFFSET(elem.getOffset());

			GLboolean normalized = GL_FALSE;
			switch (elem.getType())
			{
			case VET_COLOR:
			case VET_COLOR_ABGR:
			case VET_COLOR_ARGB:
				normalized = GL_TRUE;
				break;
			default:
				break;
			};

			UINT16 typeCount = VertexElement::getTypeCount(elem.getType());
			GLenum glType = GLHardwareBufferManager::getGLType(elem.getType());
			GLsizei vertexSize = static_cast<GLsizei>(vertexBuffer->getVertexSize());
			glVertexAttribPointer(attribLocation, typeCount, glType, normalized,
				vertexSize, bufferData);

			glEnableVertexAttribArray(attribLocation);
		}

		wantedVAO.mAttachedBuffers = (GLVertexBuffer**)bs_alloc<GLVertexBuffer*>(numUsedBuffers);
		for (UINT32 i = 0; i < numUsedBuffers; i++)
		{
			wantedVAO.mAttachedBuffers[i] = usedBuffers[i];
			wantedVAO.mAttachedBuffers[i]->registerVAO(wantedVAO);
		}

		stackDeallocLast(usedBuffers);
		stackDeallocLast(streamToSeqIdx);

		auto iter = mVAObjects.insert(wantedVAO);

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_VertexArrayObject);
		return *iter.first;
	}

	void GLVertexArrayObjectManager::notifyBufferDestroyed(const GLVertexArrayObject& vao)
	{
		mVAObjects.erase(vao);

		for (UINT32 i = 0; i < vao.mNumBuffers; i++)
		{
			vao.mAttachedBuffers[i]->unregisterVAO(vao);
		}

		glDeleteVertexArrays(1, &vao.mHandle);
		bs_free(vao.mAttachedBuffers);

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_VertexArrayObject);
	}
}