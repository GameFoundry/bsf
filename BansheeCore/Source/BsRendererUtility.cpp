#include "BsRendererUtility.h"
#include "BsRenderAPI.h"
#include "BsMesh.h"
#include "BsVertexDataDesc.h"
#include "BsMaterial.h"
#include "BsPass.h"
#include "BsBlendState.h"
#include "BsDepthStencilState.h"
#include "BsRasterizerState.h"

namespace BansheeEngine
{
	RendererUtility::RendererUtility()
	{
		VertexDataDescPtr vertexDesc = bs_shared_ptr_new<VertexDataDesc>();
		vertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);

		MeshDataPtr meshData = bs_shared_ptr_new<MeshData>(4, 6, vertexDesc);

		auto vecIter = meshData->getVec3DataIter(VES_POSITION);
		vecIter.setValue(Vector3(-1.0f, 1.0, 0));
		vecIter.setValue(Vector3(1.0f, 1.0f, 0));
		vecIter.setValue(Vector3(-1.0f, -1.0f, 0));
		vecIter.setValue(Vector3(1.0f, -1.0f, 0));

		auto indices = meshData->getIndices32();
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 1;
		indices[4] = 3;
		indices[5] = 2;

		mFullScreenQuadMesh = MeshCore::create(meshData);
	}

	RendererUtility::~RendererUtility()
	{
		
	}

	void RendererUtility::setPass(const SPtr<MaterialCore>& material, UINT32 passIdx)
	{
		RenderAPICore& rs = RenderAPICore::instance();

		SPtr<PassCore> pass = material->getPass(passIdx);
		SPtr<PassParametersCore> passParams = material->getPassParameters(passIdx);

		struct StageData
		{
			GpuProgramType type;
			bool enable;
			SPtr<GpuParamsCore> params;
			SPtr<GpuProgramCore> program;
		};

		const UINT32 numStages = 6;
		StageData stages[numStages] =
		{
			{
				GPT_VERTEX_PROGRAM, pass->hasVertexProgram(),
				passParams->mVertParams, pass->getVertexProgram()
			},
			{
				GPT_FRAGMENT_PROGRAM, pass->hasFragmentProgram(),
				passParams->mFragParams, pass->getFragmentProgram()
			},
			{
				GPT_GEOMETRY_PROGRAM, pass->hasGeometryProgram(),
				passParams->mGeomParams, pass->getGeometryProgram()
			},
			{
				GPT_HULL_PROGRAM, pass->hasHullProgram(),
				passParams->mHullParams, pass->getHullProgram()
			},
			{
				GPT_DOMAIN_PROGRAM, pass->hasDomainProgram(),
				passParams->mDomainParams, pass->getDomainProgram()
			},
			{
				GPT_COMPUTE_PROGRAM, pass->hasComputeProgram(),
				passParams->mComputeParams, pass->getComputeProgram()
			}
		};

		for (UINT32 i = 0; i < numStages; i++)
		{
			const StageData& stage = stages[i];

			if (stage.enable)
			{
				rs.bindGpuProgram(stage.program);
				rs.setGpuParams(stage.type, stage.params);

			}
			else
				rs.unbindGpuProgram(stage.type);
		}

		// Set up non-texture related pass settings
		if (pass->getBlendState() != nullptr)
			rs.setBlendState(pass->getBlendState());
		else
			rs.setBlendState(BlendStateCore::getDefault());

		if (pass->getDepthStencilState() != nullptr)
			rs.setDepthStencilState(pass->getDepthStencilState(), pass->getStencilRefValue());
		else
			rs.setDepthStencilState(DepthStencilStateCore::getDefault(), pass->getStencilRefValue());

		if (pass->getRasterizerState() != nullptr)
			rs.setRasterizerState(pass->getRasterizerState());
		else
			rs.setRasterizerState(RasterizerStateCore::getDefault());
	}

	void RendererUtility::draw(const SPtr<MeshCoreBase>& mesh, const SubMesh& subMesh)
	{
		RenderAPICore& rs = RenderAPICore::instance();
		const MeshProperties& meshProps = mesh->getProperties();
		std::shared_ptr<VertexData> vertexData = mesh->getVertexData();

		rs.setVertexDeclaration(vertexData->vertexDeclaration);
		auto& vertexBuffers = vertexData->getBuffers();

		if (vertexBuffers.size() > 0)
		{
			SPtr<VertexBufferCore> buffers[MAX_BOUND_VERTEX_BUFFERS];

			UINT32 endSlot = 0;
			UINT32 startSlot = MAX_BOUND_VERTEX_BUFFERS;
			for (auto iter = vertexBuffers.begin(); iter != vertexBuffers.end(); ++iter)
			{
				if (iter->first >= MAX_BOUND_VERTEX_BUFFERS)
					BS_EXCEPT(InvalidParametersException, "Buffer index out of range");

				startSlot = std::min(iter->first, startSlot);
				endSlot = std::max(iter->first, endSlot);
			}

			for (auto iter = vertexBuffers.begin(); iter != vertexBuffers.end(); ++iter)
			{
				buffers[iter->first - startSlot] = iter->second;
			}

			rs.setVertexBuffers(startSlot, buffers, endSlot - startSlot + 1);
		}

		rs.setDrawOperation(subMesh.drawOp);

		SPtr<IndexBufferCore> indexBuffer = mesh->getIndexBuffer();

		UINT32 indexCount = subMesh.indexCount;

		rs.setIndexBuffer(indexBuffer);
		rs.drawIndexed(subMesh.indexOffset + mesh->getIndexOffset(), indexCount, mesh->getVertexOffset(), vertexData->vertexCount);

		mesh->_notifyUsedOnGPU();
	}

	void RendererUtility::drawScreenQuad(const ViewportCore& viewport)
	{
		// Note: Consider drawing the quad using a single large triangle for possibly better performance
		Vector3 vertices[4];

		Rect2I viewArea = viewport.getArea();

		vertices[0] = Vector3((float)viewArea.x, (float)viewArea.y, 0.0f);
		vertices[1] = Vector3((float)viewArea.x + (float)viewArea.width, (float)viewArea.y, 0.0f);
		vertices[2] = Vector3((float)viewArea.x, (float)viewArea.y + (float)viewArea.height, 0.0f);
		vertices[3] = Vector3((float)viewArea.x + (float)viewArea.width, (float)viewArea.y + (float)viewArea.width, 0.0f);

		auto targetProps = viewport.getTarget()->getProperties();;

		RenderAPICore& rapi = RenderAPICore::instance();
		for (int i = 0; i < 4; i++)
		{
			vertices[i].x = -1.0f + 2.0f * (vertices[i].x + rapi.getHorizontalTexelOffset()) / targetProps.getWidth();
			vertices[i].y = 1.0f - 2.0f * (vertices[i].y + rapi.getVerticalTexelOffset()) / targetProps.getHeight();
		}

		SPtr<VertexBufferCore> vb = mFullScreenQuadMesh->getVertexData()->getBuffer(0);
		vb->writeData(0, sizeof(vertices), vertices, BufferWriteType::Discard);

		draw(mFullScreenQuadMesh, mFullScreenQuadMesh->getProperties().getSubMesh());
	}

	RendererUtility& gRendererUtility()
	{
		return RendererUtility::instance();
	}
}