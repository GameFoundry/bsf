#pragma once

#include "BsEditorPrerequisites.h"
#include "BsColor.h"
#include "BsMatrix4.h"
#include "BsGpuParam.h"

namespace BansheeEngine
{
	class SelectionRendererCore;

	class SelectionRenderer
	{
		struct ObjectData
		{
			SPtr<MeshCoreBase> mesh;
			Matrix4 worldTfrm;
		};

	public:
		SelectionRenderer();
		~SelectionRenderer();

		void update(const CameraHandlerPtr& camera);

	private:
		friend class SelectionRendererCore;

		void initializeCore(const SPtr<MaterialCore>& mat);
		void destroyCore(SelectionRendererCore* core);

		SelectionRendererCore* mCore;
	};

	class SelectionRendererCore
	{
		friend class SelectionRenderer;
		
		struct PrivatelyConstuct { };

	public:
		SelectionRendererCore(const PrivatelyConstuct& dummy);

	private:
		void initialize(const SPtr<MaterialCore>& mat);

		void render(const CameraHandlerCore& camera);
		void updateData(const SPtr<RenderTargetCore>& rt, const Vector<SelectionRenderer::ObjectData>& objects);

		SPtr<RenderTargetCore> mSceneRenderTarget;
		Vector<SelectionRenderer::ObjectData> mObjects;

		// Immutable
		SPtr<MaterialCore> mMaterial;
		GpuParamMat4Core mMatWorldViewProj;
		GpuParamColorCore mColor;

		static const Color SELECTION_COLOR;
	};
}