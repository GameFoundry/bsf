#include <entt/entt.hpp>
#include "./entActor.h"
#include "Components/ParentChild.h"

namespace ecs {

	void removeChildFromCurrentParentIfExists(Registry& reg, ParentChild& child) {
		if (child.parent != entt::null) {
			if (child.nextSibling == entt::null) {
				// then current parent will no longer have any children.
				reg.reset<ParentChild>(child.parent);
			} else {
				// otherwise parent will have child be the next sibling.
				reg.get<ParentChild>(child.parent).child = child.nextSibling;
			}
		}
	}

	void addChildToParent(Registry& reg, EntityType parent, EntityType child) {
		auto& p = reg.get_or_assign<ParentChild>(parent);
		auto& c = reg.get_or_assign<ParentChild>(child);

		removeChildFromCurrentParentIfExists(reg, c);

		c.parent = parent;
		// if parent doesn't have existing child, then
		// nextSibling will just be entt::null which is fine.
		c.nextSibling = p.child;
		p.child = child;
		// assume child doesn't belong to any parent.
		reg.assign<ParentChild>(child, c);
	}

	void addToSibling(Registry& reg, EntityType child, EntityType sibling) {

		auto& c = reg.get_or_assign<ParentChild>(child);
		auto& s = reg.get_or_assign<ParentChild>(sibling);

		removeChildFromCurrentParentIfExists(reg, c);

		if (s.prevSibling != entt::null) {
			reg.get<ParentChild>(s.prevSibling).nextSibling = child;
			c.prevSibling = s.prevSibling;
		}
		c.nextSibling = sibling;
		s.prevSibling = child;
	}
}