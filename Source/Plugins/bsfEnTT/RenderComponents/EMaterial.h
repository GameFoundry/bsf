// #pragma once

// #include "BsCorePrerequisites.h"
// // #include "Resources/BsIResourceListener.h"
// // #include "Math/BsBounds.h"
// // #include "Math/BsAABox.h"
// // #include "Scene/BsSceneActor.h"
// // #include "Material/BsShader.h"
// // #include "Material/BsMaterial.h"
// #include "Material/BsMaterialParams.h"
// #include "Material/BsShader.h"
// #include "RenderAPI/BsGpuBuffer.h"
// #include "RenderAPI/BsVertexDeclaration.h"
// #include "RenderAPI/BsVertexBuffer.h"
// #include "Mesh/BsMesh.h"
// #include "Math/BsBounds.h"

// namespace bs::ecs {

// enum class RenderableAnimType
// {
// 	None,
// 	Skinned,
// 	Morph,
// 	SkinnedMorph,
// 	Count // Keep at end
// };

// struct EBoneMatrix {
//   SPtr<bs::GpuBufferProperties> mBoneMatrixBuffer;
// };

// struct EMorphable {
//   SPtr<bs::VertexBufferProperties> mMorphShapeBuffer;
//   SPtr<bs::VertexDeclarationProperties> mMorphVertexDeclaration;
// };

// struct MaterialParamStructData
// {
// 	UINT8* data;
// 	UINT32 dataSize;
// };

// /** Data for a single texture parameter. */
// struct MaterialParamTextureData
// {
// 	HTexture texture;
// 	HSpriteTexture spriteTexture;
// 	bool isLoadStore;
// 	TextureSurface surface;
// };

// struct MaterialParamBufferData
// {
// 	SPtr<GpuBuffer> value;
// };

// struct MaterialParamSamplerStateData
// {
// 	SPtr<SamplerState> value;
// };

// class EMaterialParams : public bs::MaterialParamsBase {
// 	MaterialParamStructData* mStructParams = nullptr;
// 	MaterialParamTextureData* mTextureParams = nullptr;
// 	MaterialParamBufferData* mBufferParams = nullptr;
// 	MaterialParamSamplerStateData* mSamplerStateParams = nullptr;
// 	Texture* mDefaultTextureParams = nullptr;
// 	SamplerState* mDefaultSamplerStateParams = nullptr;
// public:
// 	TMaterialParams(const ShaderType& shader, UINT64 initialParamVersion);
// 	void getStructData(const String& name, void* value, UINT32 size, UINT32 arrayIdx) const;
// 	void setStructData(const String& name, const void* value, UINT32 size, UINT32 arrayIdx);
// 	void getTexture(const String& name, TextureType& value, TextureSurface& surface) const;
// 	void setTexture(const String& name, const TextureType& value,
// 					const TextureSurface& surface = TextureSurface::COMPLETE);
// 	void getSpriteTexture(const String& name, SpriteTextureType& value) const;
// 	void setSpriteTexture(const String& name, const SpriteTextureType& value);
// 	void getLoadStoreTexture(const String& name, TextureType& value, TextureSurface& surface) const;
// 	void setLoadStoreTexture(const String& name, const TextureType& value, const TextureSurface& surface);
// 	void getBuffer(const String& name, BufferType& value) const;
// 	void setBuffer(const String& name, const BufferType& value);
// 	void getSamplerState(const String& name, SamplerType& value) const;
// 	void setSamplerState(const String& name, const SamplerType& value);
// 	bool isAnimated(const String& name, UINT32 arrayIdx = 0);

// 	/**
// 	 * Equivalent to getStructData(const String&, UINT32, void*, UINT32) except it uses the internal parameter reference
// 	 * directly, avoiding the name lookup. Caller must guarantee the parameter reference is valid and belongs to this
// 	 * object.
// 	 */
// 	void getStructData(const ParamData& param, void* value, UINT32 size, UINT32 arrayIdx) const;

// 	/**
// 	 * Equivalent to setStructData(const String&, UINT32, void*, UINT32) except it uses the internal parameter reference
// 	 * directly, avoiding the name lookup. Caller must guarantee the parameter reference is valid and belongs to this
// 	 * object.
// 	 */
// 	void setStructData(const ParamData& param, const void* value, UINT32 size, UINT32 arrayIdx);

// 	/**
// 	 * Returns a size of a struct parameter in bytes, using the internal parameter reference. Caller must guarantee the
// 	 * parameter reference is valid and belongs to this object.
// 	 */
// 	UINT32 getStructSize(const ParamData& param) const;

// 	/**
// 	 * Equivalent to getTexture(const String&, HTexture&, TextureSurface&) except it uses the internal parameter
// 	 * reference directly, avoiding the name lookup. Caller must guarantee the parameter reference is valid and belongs
// 	 * to this object.
// 	 */
// 	void getTexture(const ParamData& param, TextureType& value, TextureSurface& surface) const;

// 	/**
// 	 * Equivalent to setTexture(const String&, const HTexture&, const TextureSurface&) except it uses the internal
// 	 * parameter reference directly, avoiding the name lookup. Caller must guarantee the parameter reference is valid
// 	 * and belongs to this object.
// 	 */
// 	void setTexture(const ParamData& param, const TextureType& value,
// 					const TextureSurface& surface = TextureSurface::COMPLETE);

// 	*
// 	 * Equivalent to getSpriteTexture(const String&, HSpriteTexture&) except it uses the internal parameter reference
// 	 * directly, avoiding the name lookup. Caller must guarantee the parameter reference is valid and belongs to this
// 	 * object.

// 	void getSpriteTexture(const ParamData& param, SpriteTextureType& value) const;

// 	/**
// 	 * Equivalent to setSpriteTexture(const String&, const HSpriteTexture&) except it uses the internal parameter
// 	 * reference directly, avoiding the name lookup. Caller must guarantee the parameter reference is valid and belongs
// 	 * to this object.
// 	 */
// 	void setSpriteTexture(const ParamData& param, const SpriteTextureType& value);

// 	/**
// 	 * Equivalent to getBuffer(const String&, SPtr<GpuBuffer>&) except it uses the internal parameter reference
// 	 * directly, avoiding the name lookup. Caller must guarantee the parameter reference is valid and belongs to this
// 	 * object.
// 	 */
// 	void getBuffer(const ParamData& param, BufferType& value) const;

// 	/**
// 	 * Equivalent to setBuffer(const String&, const SPtr<GpuBuffer>&) except it uses the internal parameter reference
// 	 * directly, avoiding the name lookup. Caller must guarantee the parameter reference is valid and belongs to this
// 	 * object.
// 	 */
// 	void setBuffer(const ParamData& param, const BufferType& value);

// 	/**
// 	 * Equivalent to getLoadStoreTexture(const String&, HTexture&, TextureSurface&) except it uses the internal
// 	 * parameter reference directly, avoiding the name lookup. Caller must guarantee the parameter reference is valid
// 	 * and belongs to this object.
// 	 */
// 	void getLoadStoreTexture(const ParamData& param, TextureType& value, TextureSurface& surface) const;

// 	/**
// 	 * Equivalent to setLoadStoreTexture(const String&, const HTexture&, TextureSurface&) except it uses the internal
// 	 * parameter reference directly, avoiding the name lookup. Caller must guarantee the parameter reference is valid
// 	 * and belongs to this object.
// 	 */
// 	void setLoadStoreTexture(const ParamData& param, const TextureType& value, const TextureSurface& surface);

// 	/**
// 	 * Returns the type of texture that is currently assigned to the provided parameter. This can only be called on
// 	 * on texture parameters. Caller must guarantee the parameter reference is valid, is of a texture type and
// 	 * belongs to this object.
// 	 */
// 	MateralParamTextureType getTextureType(const ParamData& param) const;

// 	/**
// 	 * Checks does the provided parameter have a curve or gradient assigned. This can only be called on data parameters.
// 	 * Caller must guarantee the parameter reference is valid, is of a data type and belongs to this object.

// 	 */
// 	bool isAnimated(const ParamData& param, UINT32 arrayIdx) const;

// 	/**
// 	 * Returns a sprite texture that is used for populating the specified data parameter. This is only relevant
// 	 * for data parameters marked with the ShaderParamAttributeType::SpriteUV attribute.
// 	 */
// 	SpriteTextureType getOwningSpriteTexture(const ParamData& param) const;

// 	/**
// 	 * Equivalent to getSamplerState(const String&, SPtr<SamplerState>&) except it uses the internal parameter reference
// 	 * directly, avoiding the name lookup. Caller must guarantee the parameter reference is valid and belongs to this
// 	 * object.
// 	 */
// 	void getSamplerState(const ParamData& param, SamplerType& value) const;

// 	/**
// 	 * Equivalent to setSamplerState(const String&, const SPtr<SamplerState>&) except it uses the internal parameter
// 	 * reference directly, avoiding the name lookup. Caller must guarantee the parameter reference is valid and belongs
// 	 * to this object.
// 	 */
// 	void setSamplerState(const ParamData& param, const SamplerType& value);

// 	/**
// 	 * Returns the default texture (one assigned when no other is provided), if available for the specified parameter.
// 	 * Parameter is represented using the internal parameter reference and the caller must guarantee the parameter
// 	 * eference is valid and belongs to this object.
// 	 */
// 	void getDefaultTexture(const ParamData& param, TextureType& value) const;

// 	/**
// 	 * Returns the default sampler state (one assigned when no other is provided), if available for the specified
// 	 * parameter. Parameter is represented using the internal parameter reference and the caller must guarantee the
// 	 * parameter reference is valid and belongs to this object.
// 	 */
// 	void getDefaultSamplerState(const ParamData& param, SamplerType& value) const;
// };

// struct EMaterial {
// 	Shader mShader;
// 	SPtr<MaterialParams> mParams;
// 	Vector<SPtr<Technique>> mTechniques;
// 	ShaderVariation mVariation;
// };
