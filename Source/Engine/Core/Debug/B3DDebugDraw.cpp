//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Debug/B3DDebugDraw.h"
#include "Mesh/B3DMesh.h"
#include "GpuBackend/B3DVertexDescription.h"
#include "Utility/B3DShapeMeshes3D.h"
#include "Image/B3DSpriteTexture.h"
#include "CoreObject/B3DRenderThread.h"
#include "Material/B3DMaterial.h"
#include "GpuBackend/B3DGpuParameterSet.h"
#include "Material/B3DMaterialParameterAdapter.h"
#include "Renderer/B3DRenderer.h"
#include "Renderer/B3DRendererUtility.h"
#include "Utility/B3DDrawHelper.h"
#include "Renderer/B3DRendererExtension.h"
#include "Resources/B3DBuiltinResources.h"
#include "Components/B3DCamera.h"
#include "Profiling/B3DProfilerGPU.h"
#include "FrameGraph/B3DFrameGraph.h"


using namespace b3d;

DebugDraw::DebugDraw()
{
	mDrawHelper = B3DNew<DrawHelper>();
	mRenderer = RendererExtension::Create<render::DebugDrawRenderer>(nullptr);
}

DebugDraw::~DebugDraw()
{
	B3DDelete(mDrawHelper);
}

void DebugDraw::SetColor(const Color& color)
{
	mDrawHelper->SetColor(color);
}

void DebugDraw::SetTransform(const Matrix4& transform)
{
	mDrawHelper->SetTransform(transform);
}

void DebugDraw::DrawCube(const Vector3& position, const Vector3& extents)
{
	mDrawHelper->Cube(position, extents);
}

void DebugDraw::DrawSphere(const Vector3& position, float radius)
{
	mDrawHelper->Sphere(position, radius);
}

void DebugDraw::DrawCone(const Vector3& base, const Vector3& normal, float height, float radius, const Vector2& scale)
{
	mDrawHelper->Cone(base, normal, height, radius, scale);
}

void DebugDraw::DrawDisc(const Vector3& position, const Vector3& normal, float radius)
{
	mDrawHelper->Disc(position, normal, radius);
}

void DebugDraw::DrawWireCube(const Vector3& position, const Vector3& extents)
{
	mDrawHelper->WireCube(position, extents);
}

void DebugDraw::DrawWireSphere(const Vector3& position, float radius)
{
	mDrawHelper->WireSphere(position, radius);
}

void DebugDraw::DrawWireCone(const Vector3& base, const Vector3& normal, float height, float radius, const Vector2& scale)
{
	mDrawHelper->WireCone(base, normal, height, radius, scale);
}

void DebugDraw::DrawLine(const Vector3& start, const Vector3& end)
{
	mDrawHelper->Line(start, end);
}

void DebugDraw::DrawLineList(const Vector<Vector3>& linePoints)
{
	mDrawHelper->LineList(linePoints);
}

void DebugDraw::DrawWireDisc(const Vector3& position, const Vector3& normal, float radius)
{
	mDrawHelper->WireDisc(position, normal, radius);
}

void DebugDraw::DrawWireArc(const Vector3& position, const Vector3& normal, float radius, Degree startAngle, Degree amountAngle)
{
	mDrawHelper->WireArc(position, normal, radius, startAngle, amountAngle);
}

void DebugDraw::DrawWireMesh(const TShared<MeshData>& meshData)
{
	mDrawHelper->WireMesh(meshData);
}

void DebugDraw::DrawFrustum(const Vector3& position, float aspect, Degree FOV, float near, float far)
{
	mDrawHelper->Frustum(position, aspect, FOV, near, far);
}

Vector<DebugDraw::MeshRenderData> DebugDraw::CreateMeshProxyData(const Vector<DrawHelper::ShapeMeshData>& meshData)
{
	Vector<MeshRenderData> proxyData;
	for(auto& entry : meshData)
	{
		if(entry.Type == DrawHelper::MeshType::Solid)
			proxyData.push_back(MeshRenderData(B3DGetRenderProxy(entry.Mesh), entry.SubMesh, DebugDrawMaterialType::Solid));
		else if(entry.Type == DrawHelper::MeshType::Wire)
			proxyData.push_back(MeshRenderData(B3DGetRenderProxy(entry.Mesh), entry.SubMesh, DebugDrawMaterialType::Wire));
		else if(entry.Type == DrawHelper::MeshType::Line)
			proxyData.push_back(MeshRenderData(B3DGetRenderProxy(entry.Mesh), entry.SubMesh, DebugDrawMaterialType::Line));
	}

	return proxyData;
}

void DebugDraw::Clear()
{
	mDrawHelper->Clear();
}

void DebugDraw::Update()
{
	mActiveMeshes.clear();
	mActiveMeshes = mDrawHelper->BuildMeshes(DrawHelper::SortType::None);

	Vector<MeshRenderData> proxyData = CreateMeshProxyData(mActiveMeshes);

	render::DebugDrawRenderer* renderer = mRenderer.get();
	GetRenderThread().PostCommand([renderer, proxyData]() { renderer->UpdateData(proxyData); }, "DebugDrawRenderer::UpdateData");
}

namespace b3d { namespace render
{

DebugDrawUniformDefinition gDebugDrawUniformDefinition;

TShared<GpuParameterSet> DebugDrawMaterial::Prepare(FrameGraphPass& pass, const GpuBufferSuballocation& uniformBuffer)
{
	const TShared<GpuParameterSet> set = CreateGpuParameterSet(pass);
	set->SetUniformBuffer("Params", uniformBuffer);

	return set;
}

void DebugDrawMaterial::Execute(FrameGraphPassContext& context, const TShared<GpuParameterSet>& set, const TShared<Mesh>& mesh, const SubMesh& subMesh) const
{
	GpuCommandBuffer& commandBuffer = context.GetCommandBuffer();
	B3D_PROFILE_RENDERER_MATERIAL

	Bind(commandBuffer, set);
	GetRendererUtility().Draw(commandBuffer, mesh, subMesh);
}

DebugDrawMaterial* DebugDrawMaterial::GetVariation(DebugDrawMaterialType material)
{
	if(material == DebugDrawMaterialType::Solid)
		return Get(GetVariation<true, false, false>());

	if(material == DebugDrawMaterialType::Wire)
		return Get(GetVariation<false, false, true>());

	return Get(GetVariation<false, true, false>());
}

DebugDrawRenderer::DebugDrawRenderer()
	: RendererExtension(RenderLocation::PostLightPass, 0)
{
}

void DebugDrawRenderer::Initialize(const Any& data)
{
	ASSERT_IF_NOT_RENDER_THREAD;
}

void DebugDrawRenderer::UpdateData(const Vector<DebugDraw::MeshRenderData>& meshes)
{
	mMeshes = meshes;
}

RendererExtensionRequest DebugDrawRenderer::Check(const Camera& camera)
{
	return mMeshes.empty() ? RendererExtensionRequest::RenderIfTargetDirty : RendererExtensionRequest::ForceRender;
}

void DebugDrawRenderer::Render(const Camera& camera, const RendererViewContext& viewContext)
{
	// Check() lets the renderer call this for every view, also when there is nothing to draw
	TShared<RenderTarget> renderTarget = camera.GetViewport()->GetTarget();
	if(renderTarget == nullptr || mMeshes.empty())
		return;

	Matrix4 viewMatrix = camera.GetViewMatrix();
	Matrix4 projMatrix = camera.GetProjectionMatrix();
	Matrix4 viewProjMat = projMatrix * viewMatrix;

	GpuBufferMappedScope uniforms = gDebugDrawUniformDefinition.AllocateTransient().Map();

	gDebugDrawUniformDefinition.gMatViewProj.Set(uniforms, viewProjMat);
	gDebugDrawUniformDefinition.gViewDir.Set(uniforms, (Vector4)camera.GetWorldTransform().GetForward());

	/** A mesh to draw, with the set of its material variation. */
	struct DebugDrawCall
	{
		DebugDrawMaterial* Material;
		TShared<GpuParameterSet> Set;
		TShared<Mesh> Mesh;
		SubMesh SubMesh;
	};

	// Draws over the lit scene, testing and writing scene depth
	FrameGraphRasterPass pass;
	viewContext.SetAttachments(pass);

	Vector<DebugDrawCall> draws;
	draws.reserve(mMeshes.size());

	for(auto& entry : mMeshes)
	{
		DebugDrawMaterial* material = DebugDrawMaterial::GetVariation(entry.Type);

		// One set per material variation
		auto found = std::find_if(draws.begin(), draws.end(), [material](const DebugDrawCall& draw) { return draw.Material == material; });
		TShared<GpuParameterSet> set = found != draws.end() ? found->Set : material->Prepare(pass, uniforms);

		draws.push_back({ material, std::move(set), entry.Mesh, entry.SubMesh });
	}

	const Area2 viewport = viewContext.Viewport;
	viewContext.Graph.AddRasterPass("DebugDraw", std::move(pass), [draws = std::move(draws), viewport](FrameGraphPassContext& context)
	{
		context.GetCommandBuffer().SetViewport(viewport);

		for(const DebugDrawCall& draw : draws)
			draw.Material->Execute(context, draw.Set, draw.Mesh, draw.SubMesh);
	});
}
}}
