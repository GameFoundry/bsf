//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Renderer/BsRendererUtility.h"
#include "RenderAPI/BsRenderAPI.h"
#include "Mesh/BsMesh.h"
#include "RenderAPI/BsVertexDataDesc.h"
#include "Material/BsMaterial.h"
#include "Material/BsGpuParamsSet.h"
#include "Material/BsPass.h"
#include "RenderAPI/BsGpuParams.h"
#include "Utility/BsShapeMeshes3D.h"
#include "Renderer/BsLight.h"
#include "Material/BsShader.h"
#include "Renderer/BsIBLUtility.h"
#include "Math/BsAABox.h"

namespace bs { namespace ct
{
	RendererUtility::RendererUtility()
	{
		{
			mFullscreenQuadVDesc = bs_shared_ptr_new<VertexDataDesc>();
			mFullscreenQuadVDesc->addVertElem(VET_FLOAT3, VES_POSITION);
			mFullscreenQuadVDesc->addVertElem(VET_FLOAT2, VES_TEXCOORD);

			INDEX_BUFFER_DESC ibDesc;
			ibDesc.indexType = IT_32BIT;
			ibDesc.numIndices = 6;
			ibDesc.usage = GBU_DYNAMIC;

			mFullScreenQuadIB = IndexBuffer::create(ibDesc);
			mFullscreenQuadVDecl = VertexDeclaration::create(mFullscreenQuadVDesc);

			VERTEX_BUFFER_DESC vbDesc;
			vbDesc.vertexSize = mFullscreenQuadVDecl->getProperties().getVertexSize(0);
			vbDesc.numVerts = 4 * NUM_QUAD_VB_SLOTS;
			vbDesc.usage = GBU_DYNAMIC;

			mFullScreenQuadVB = VertexBuffer::create(vbDesc);

			UINT32 indices[] { 0, 1, 2, 1, 3, 2 };
			mFullScreenQuadIB->writeData(0, sizeof(indices), indices, BWT_DISCARD);
		}

		{
			SPtr<VertexDataDesc> vertexDesc = bs_shared_ptr_new<VertexDataDesc>();
			vertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);

			UINT32 numVertices = 0;
			UINT32 numIndices = 0;

			ShapeMeshes3D::getNumElementsSphere(3, numVertices, numIndices);
			SPtr<MeshData> meshData = bs_shared_ptr_new<MeshData>(numVertices, numIndices, vertexDesc);

			UINT32* indexData = meshData->getIndices32();
			UINT8* positionData = meshData->getElementData(VES_POSITION);

			Sphere localSphere(Vector3::ZERO, 1.0f);
			ShapeMeshes3D::solidSphere(localSphere, positionData, nullptr, nullptr, 0,
				vertexDesc->getVertexStride(), indexData, 0, 3);

			mUnitSphereStencilMesh = Mesh::create(meshData);
		}

		{
			SPtr<VertexDataDesc> vertexDesc = bs_shared_ptr_new<VertexDataDesc>();
			vertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);

			UINT32 numVertices = 0;
			UINT32 numIndices = 0;

			ShapeMeshes3D::getNumElementsAABox(numVertices, numIndices);
			SPtr<MeshData> meshData = bs_shared_ptr_new<MeshData>(numVertices, numIndices, vertexDesc);

			UINT32* indexData = meshData->getIndices32();
			UINT8* positionData = meshData->getElementData(VES_POSITION);

			AABox localBox(-Vector3::ONE, Vector3::ONE);
			ShapeMeshes3D::solidAABox(localBox, positionData, nullptr, nullptr, 0,
				vertexDesc->getVertexStride(), indexData, 0);

			mUnitBoxStencilMesh = Mesh::create(meshData);
		}

		{
			UINT32 numSides = Light::LIGHT_CONE_NUM_SIDES;
			UINT32 numSlices = Light::LIGHT_CONE_NUM_SLICES;

			SPtr<VertexDataDesc> vertexDesc = bs_shared_ptr_new<VertexDataDesc>();
			vertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);

			UINT32 numVertices = numSides * numSlices * 2;
			UINT32 numIndices = ((numSides * 2) * (numSlices - 1) * 2) * 3;

			SPtr<MeshData> meshData = bs_shared_ptr_new<MeshData>(numVertices, numIndices, vertexDesc);

			UINT32* indexData = meshData->getIndices32();
			UINT8* positionData = meshData->getElementData(VES_POSITION);
			UINT32 stride = vertexDesc->getVertexStride();

			// Dummy vertex positions, actual ones generated in shader
			for (UINT32 i = 0; i < numVertices; i++)
			{
				memcpy(positionData, &Vector3::ZERO, sizeof(Vector3));
				positionData += stride;
			}

			// Cone indices
			UINT32 curIdx = 0;
			for (UINT32 sliceIdx = 0; sliceIdx < (numSlices - 1); sliceIdx++)
			{
				for (UINT32 sideIdx = 0; sideIdx < numSides; sideIdx++)
				{
					indexData[curIdx++] = sliceIdx * numSides + sideIdx;
					indexData[curIdx++] = sliceIdx * numSides + (sideIdx + 1) % numSides;
					indexData[curIdx++] = (sliceIdx + 1) * numSides + sideIdx;

					indexData[curIdx++] = sliceIdx * numSides + (sideIdx + 1) % numSides;
					indexData[curIdx++] = (sliceIdx + 1) * numSides + (sideIdx + 1) % numSides;
					indexData[curIdx++] = (sliceIdx + 1) * numSides + sideIdx;
				}
			}

			// Sphere cap indices
			UINT32 coneOffset = numSides * numSlices;
			for (UINT32 sliceIdx = 0; sliceIdx < (numSlices - 1); sliceIdx++)
			{
				for (UINT32 sideIdx = 0; sideIdx < numSides; sideIdx++)
				{
					indexData[curIdx++] = coneOffset + sliceIdx * numSides + sideIdx;
					indexData[curIdx++] = coneOffset + sliceIdx * numSides + (sideIdx + 1) % numSides;
					indexData[curIdx++] = coneOffset + (sliceIdx + 1) * numSides + sideIdx;

					indexData[curIdx++] = coneOffset + sliceIdx * numSides + (sideIdx + 1) % numSides;
					indexData[curIdx++] = coneOffset + (sliceIdx + 1) * numSides + (sideIdx + 1) % numSides;
					indexData[curIdx++] = coneOffset + (sliceIdx + 1) * numSides + sideIdx;
				}
			}

			mSpotLightStencilMesh = Mesh::create(meshData);
		}

		{
			SPtr<VertexDataDesc> vertexDesc = bs_shared_ptr_new<VertexDataDesc>();
			vertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);

			UINT32 numVertices = 0;
			UINT32 numIndices = 0;

			ShapeMeshes3D::getNumElementsAABox(numVertices, numIndices);
			SPtr<MeshData> meshData = bs_shared_ptr_new<MeshData>(numVertices, numIndices, vertexDesc);

			UINT32* indexData = meshData->getIndices32();
			UINT8* positionData = meshData->getElementData(VES_POSITION);

			AABox localBox(-Vector3::ONE * 1500.0f, Vector3::ONE * 1500.0f);
			ShapeMeshes3D::solidAABox(localBox, positionData, nullptr, nullptr, 0,
									   vertexDesc->getVertexStride(), indexData, 0);

			mSkyBoxMesh = Mesh::create(meshData);
		}
	}

	void RendererUtility::setPass(const SPtr<Material>& material, UINT32 passIdx, UINT32 techniqueIdx)
	{
		RenderAPI& rapi = RenderAPI::instance();

		SPtr<Pass> pass = material->getPass(passIdx, techniqueIdx);
		rapi.setGraphicsPipeline(pass->getGraphicsPipelineState());
		rapi.setStencilRef(pass->getStencilRefValue());
	}

	void RendererUtility::setComputePass(const SPtr<Material>& material, UINT32 passIdx)
	{
		RenderAPI& rapi = RenderAPI::instance();

		SPtr<Pass> pass = material->getPass(passIdx);
		rapi.setComputePipeline(pass->getComputePipelineState());
	}

	void RendererUtility::setPassParams(const SPtr<GpuParamsSet>& params, UINT32 passIdx)
	{
		SPtr<GpuParams> gpuParams = params->getGpuParams(passIdx);
		if (gpuParams == nullptr)
			return;

		RenderAPI& rapi = RenderAPI::instance();
		rapi.setGpuParams(gpuParams);
	}

	void RendererUtility::draw(const SPtr<MeshBase>& mesh, UINT32 numInstances)
	{
		draw(mesh, mesh->getProperties().getSubMesh(0), numInstances);
	}

	void RendererUtility::draw(const SPtr<MeshBase>& mesh, const SubMesh& subMesh, UINT32 numInstances)
	{
		RenderAPI& rapi = RenderAPI::instance();
		SPtr<VertexData> vertexData = mesh->getVertexData();

		rapi.setVertexDeclaration(mesh->getVertexData()->vertexDeclaration);

		auto& vertexBuffers = vertexData->getBuffers();
		if (vertexBuffers.size() > 0)
		{
			SPtr<VertexBuffer> buffers[BS_MAX_BOUND_VERTEX_BUFFERS];

			UINT32 endSlot = 0;
			UINT32 startSlot = BS_MAX_BOUND_VERTEX_BUFFERS;
			for (auto iter = vertexBuffers.begin(); iter != vertexBuffers.end(); ++iter)
			{
				if (iter->first >= BS_MAX_BOUND_VERTEX_BUFFERS)
					BS_EXCEPT(InvalidParametersException, "Buffer index out of range");

				startSlot = std::min(iter->first, startSlot);
				endSlot = std::max(iter->first, endSlot);
			}

			for (auto iter = vertexBuffers.begin(); iter != vertexBuffers.end(); ++iter)
			{
				buffers[iter->first - startSlot] = iter->second;
			}

			rapi.setVertexBuffers(startSlot, buffers, endSlot - startSlot + 1);
		}

		SPtr<IndexBuffer> indexBuffer = mesh->getIndexBuffer();
		rapi.setIndexBuffer(indexBuffer);

		rapi.setDrawOperation(subMesh.drawOp);

		UINT32 indexCount = subMesh.indexCount;
		rapi.drawIndexed(subMesh.indexOffset + mesh->getIndexOffset(), indexCount, mesh->getVertexOffset(),
			vertexData->vertexCount, numInstances);

		mesh->_notifyUsedOnGPU();
	}

	void RendererUtility::drawMorph(const SPtr<MeshBase>& mesh, const SubMesh& subMesh,
		const SPtr<VertexBuffer>& morphVertices, const SPtr<VertexDeclaration>& morphVertexDeclaration)
	{
		// Bind buffers and draw
		RenderAPI& rapi = RenderAPI::instance();

		SPtr<VertexData> vertexData = mesh->getVertexData();
		rapi.setVertexDeclaration(morphVertexDeclaration);

		auto& meshBuffers = vertexData->getBuffers();
		SPtr<VertexBuffer> allBuffers[BS_MAX_BOUND_VERTEX_BUFFERS];

		UINT32 endSlot = 0;
		UINT32 startSlot = BS_MAX_BOUND_VERTEX_BUFFERS;
		for (auto iter = meshBuffers.begin(); iter != meshBuffers.end(); ++iter)
		{
			if (iter->first >= BS_MAX_BOUND_VERTEX_BUFFERS)
				BS_EXCEPT(InvalidParametersException, "Buffer index out of range");

			startSlot = std::min(iter->first, startSlot);
			endSlot = std::max(iter->first, endSlot);
		}

		startSlot = std::min(1U, startSlot);
		endSlot = std::max(1U, endSlot);

		for (auto iter = meshBuffers.begin(); iter != meshBuffers.end(); ++iter)
			allBuffers[iter->first - startSlot] = iter->second;

		allBuffers[1] = morphVertices;
		rapi.setVertexBuffers(startSlot, allBuffers, endSlot - startSlot + 1);

		SPtr<IndexBuffer> indexBuffer = mesh->getIndexBuffer();
		rapi.setIndexBuffer(indexBuffer);

		rapi.setDrawOperation(subMesh.drawOp);

		UINT32 indexCount = subMesh.indexCount;
		rapi.drawIndexed(subMesh.indexOffset + mesh->getIndexOffset(), indexCount, mesh->getVertexOffset(),
			vertexData->vertexCount, 1);

		mesh->_notifyUsedOnGPU();
	}

	void RendererUtility::blit(const SPtr<Texture>& texture, const Rect2I& area, bool flipUV, bool isDepth, bool isFiltered)
	{
		auto& texProps = texture->getProperties();

		Rect2 fArea((float)area.x, (float)area.y, (float)area.width, (float)area.height);
		if (area.width == 0 || area.height == 0)
		{
			fArea.x = 0.0f;
			fArea.y = 0.0f;
			fArea.width = (float)texProps.getWidth();
			fArea.height = (float)texProps.getHeight();
		}

		BlitMat* blitMat = BlitMat::getVariation(texProps.getNumSamples(), !isDepth, isFiltered);
		blitMat->execute(texture, fArea, flipUV);
	}

	void RendererUtility::drawScreenQuad(const Rect2& uv, const Vector2I& textureSize, UINT32 numInstances, bool flipUV)
	{
		// Note: Consider drawing the quad using a single large triangle for possibly better performance
		// Note2: Consider setting quad size in shader instead of rebuilding the mesh every time

		const Conventions& rapiConventions = gCaps().conventions;
		Vector3 vertices[4];

		if (rapiConventions.ndcYAxis == Conventions::Axis::Down)
		{
			vertices[0] = Vector3(-1.0f, -1.0f, 0.0f);
			vertices[1] = Vector3(1.0f, -1.0f, 0.0f);
			vertices[2] = Vector3(-1.0f, 1.0f, 0.0f);
			vertices[3] = Vector3(1.0f, 1.0f, 0.0f);
		}
		else
		{
			vertices[0] = Vector3(-1.0f, 1.0f, 0.0f);
			vertices[1] = Vector3(1.0f, 1.0f, 0.0f);
			vertices[2] = Vector3(-1.0f, -1.0f, 0.0f);
			vertices[3] = Vector3(1.0f, -1.0f, 0.0f);
		}

		Vector2 uvs[4];
		if ((rapiConventions.uvYAxis == Conventions::Axis::Up) ^ flipUV)
		{
			uvs[0] = Vector2(uv.x, uv.y + uv.height);
			uvs[1] = Vector2(uv.x + uv.width, uv.y + uv.height);
			uvs[2] = Vector2(uv.x, uv.y);
			uvs[3] = Vector2(uv.x + uv.width, uv.y);
		}
		else
		{
			uvs[0] = Vector2(uv.x, uv.y);
			uvs[1] = Vector2(uv.x + uv.width, uv.y);
			uvs[2] = Vector2(uv.x, uv.y + uv.height);
			uvs[3] = Vector2(uv.x + uv.width, uv.y + uv.height);
		}

		for (int i = 0; i < 4; i++)
		{
			uvs[i].x /= (float)textureSize.x;
			uvs[i].y /= (float)textureSize.y;
		}

		SPtr<MeshData> meshData = bs_shared_ptr_new<MeshData>(4, 6, mFullscreenQuadVDesc);

		auto vecIter = meshData->getVec3DataIter(VES_POSITION);
		for (UINT32 i = 0; i < 4; i++)
			vecIter.addValue(vertices[i]);

		auto uvIter = meshData->getVec2DataIter(VES_TEXCOORD);
		for (UINT32 i = 0; i < 4; i++)
			uvIter.addValue(uvs[i]);

		UINT32 bufferSize = meshData->getStreamSize(0);
		UINT8* srcVertBufferData = meshData->getStreamData(0);

		void* dstData = mFullScreenQuadVB->lock(mNextQuadVBSlot * bufferSize, bufferSize, GBL_WRITE_ONLY_NO_OVERWRITE);
		memcpy(dstData, srcVertBufferData, bufferSize);
		mFullScreenQuadVB->unlock();

		RenderAPI& rapi = RenderAPI::instance();

		rapi.setVertexDeclaration(mFullscreenQuadVDecl);
		rapi.setVertexBuffers(0, &mFullScreenQuadVB, 1);
		rapi.setIndexBuffer(mFullScreenQuadIB);
		rapi.setDrawOperation(DOT_TRIANGLE_LIST);
		rapi.drawIndexed(0, 6, mNextQuadVBSlot * 4, 4, numInstances);

		mNextQuadVBSlot = (mNextQuadVBSlot + 1) % NUM_QUAD_VB_SLOTS;
	}

	void RendererUtility::clear(UINT32 value)
	{
		ClearMat* clearMat = ClearMat::get();
		clearMat->execute(value);
	}

	RendererUtility& gRendererUtility()
	{
		return RendererUtility::instance();
	}

	BlitMat::BlitMat()
	{
		mParams->getTextureParam(GPT_FRAGMENT_PROGRAM, "gSource", mSource);
		mIsFiltered = mVariation.getInt("MODE") == 1;
	}

	void BlitMat::execute(const SPtr<Texture>& source, const Rect2& area, bool flipUV)
	{
		BS_RENMAT_PROFILE_BLOCK

		mSource.set(source);
		bind();

		if(!mIsFiltered)
			gRendererUtility().drawScreenQuad(area, Vector2I(1, 1), 1, flipUV);
		else
			gRendererUtility().drawScreenQuad(Rect2(0, 0, 1, 1), Vector2I(1, 1), 1, flipUV);
	}

	BlitMat* BlitMat::getVariation(UINT32 msaaCount, bool isColor, bool isFiltered)
	{
		if (msaaCount > 1)
		{
			if(isColor)
			{
				switch (msaaCount)
				{
				case 2:
					return get(getVariation<2, 0>());
				case 4:
					return get(getVariation<4, 0>());
				default:
				case 8:
					return get(getVariation<8, 0>());
				}
			}
			else
			{
				switch(msaaCount)
				{
				case 2:
					return get(getVariation<2, 2>());
				case 4:
					return get(getVariation<4, 2>());
				default:
				case 8:
					return get(getVariation<8, 2>());
				}
			}
		}
		else
		{
			if(isFiltered)
				return get(getVariation<1, 1>());
			else
				return get(getVariation<1, 0>());
		}
	}

	ClearParamDef gClearParamDef;

	ClearMat::ClearMat()
	{
		mParamBuffer = gClearParamDef.createBuffer();
		mParams->setParamBlockBuffer("Params", mParamBuffer);
	}

	void ClearMat::execute(UINT32 value)
	{
		BS_RENMAT_PROFILE_BLOCK

		gClearParamDef.gClearValue.set(mParamBuffer, value);

		bind();
		gRendererUtility().drawScreenQuad();
	}

	CompositeParamDef gCompositeParamDef;

	CompositeMat::CompositeMat()
	{
		mParamBuffer = gCompositeParamDef.createBuffer();
		mParams->setParamBlockBuffer("Input", mParamBuffer);

		mParams->getTextureParam(GPT_FRAGMENT_PROGRAM, "gSource", mSourceTex);
	}

	void CompositeMat::execute(const SPtr<Texture>& source, const SPtr<RenderTarget>& target, const Color& tint)
	{
		BS_RENMAT_PROFILE_BLOCK

		// Set parameters
		mSourceTex.set(source);

		gCompositeParamDef.gTint.set(mParamBuffer, tint);

		// Render
		RenderAPI& rapi = RenderAPI::instance();
		rapi.setRenderTarget(target);

		bind();
		gRendererUtility().drawScreenQuad();
	}

	BicubicUpsampleParamDef gBicubicUpsampleParamDef;

	BicubicUpsampleMat::BicubicUpsampleMat()
	{
		mParamBuffer = gBicubicUpsampleParamDef.createBuffer();
		mParams->setParamBlockBuffer("Input", mParamBuffer);

		mParams->getTextureParam(GPT_FRAGMENT_PROGRAM, "gSource", mSourceTex);
	}

	void BicubicUpsampleMat::execute(const SPtr<Texture>& source, const SPtr<RenderTarget>& target, const Color& tint)
	{
		BS_RENMAT_PROFILE_BLOCK

		// Set parameters
		mSourceTex.set(source);

		const TextureProperties& sourceProps = source->getProperties();

		Vector2I texSize(sourceProps.getWidth(), sourceProps.getHeight());
		Vector2 invPixelSize(1.0f / texSize.x, 1.0f / texSize.y);
		Vector2 invTwoPixelSize(2.0f / texSize.x, 2.0f / texSize.y);

		gBicubicUpsampleParamDef.gTint.set(mParamBuffer, tint);
		gBicubicUpsampleParamDef.gTextureSize.set(mParamBuffer, texSize);
		gBicubicUpsampleParamDef.gInvPixel.set(mParamBuffer, invPixelSize);
		gBicubicUpsampleParamDef.gInvTwoPixels.set(mParamBuffer, invTwoPixelSize);

		// Render
		RenderAPI& rapi = RenderAPI::instance();
		rapi.setRenderTarget(target);

		bind();
		gRendererUtility().drawScreenQuad();
	}

	BicubicUpsampleMat* BicubicUpsampleMat::getVariation(bool hermite)
	{
		if (hermite)
			return get(getVariation<true>());

		return get(getVariation<false>());
	}

}}
