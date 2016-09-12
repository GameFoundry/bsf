//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsMorphShapes.h"
#include "BsMorphShapesRTTI.h"

namespace BansheeEngine
{
	MorphShape::MorphShape()
	{ }

	MorphShape::MorphShape(const String& name, const Vector<MorphVertex>& vertices)
		:mName(name), mVertices(vertices)
	{ }

	/** Creates a new morph shape from the provided set of vertices. */
	SPtr<MorphShape> MorphShape::create(const String& name, const Vector<MorphVertex>& vertices)
	{
		return bs_shared_ptr_new<MorphShape>(name, vertices);
	}

	RTTITypeBase* MorphShape::getRTTIStatic()
	{
		return MorphShapeRTTI::instance();
	}

	RTTITypeBase* MorphShape::getRTTI() const
	{
		return getRTTIStatic();
	}

	MorphShapes::MorphShapes()
	{ }

	MorphShapes::MorphShapes(const Vector<SPtr<MorphShape>>& shapes)
		:mShapes(shapes)
	{ }

	SPtr<MorphShapes> MorphShapes::create(const Vector<SPtr<MorphShape>>& shapes)
	{
		MorphShapes* raw = new (bs_alloc<MorphShapes>()) MorphShapes(shapes);
		return bs_shared_ptr(raw);
	}

	SPtr<MorphShapes> MorphShapes::createEmpty()
	{
		MorphShapes* raw = new (bs_alloc<MorphShapes>()) MorphShapes();
		return bs_shared_ptr(raw);
	}

	RTTITypeBase* MorphShapes::getRTTIStatic()
	{
		return MorphShapesRTTI::instance();
	}

	RTTITypeBase* MorphShapes::getRTTI() const
	{
		return getRTTIStatic();
	}
}