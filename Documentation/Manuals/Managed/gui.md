Every GUIElement must be attached at one GUILayout to be visible. There are a few different types of GUILayout and they handle how elements are positioned:
 - GUILayoutX - Child elements are aligned left to right
 - GUILayoutY - Child elements are aligned top to bottom
 - GUIPanel - User is allowed to manually position elements.

GUILayout elements can also be parented to each other, e.g. a GUILayoutX can have two GUILayoutY children to create something similar to a two column table. This kind of layout system allows you to easily position GUI elements from within code, and ensures that the result looks good with different resolutions / window sizes.

Every GUI element has a set of properties the user can change:
 - Its dimensions (these can be fixed, or flexible so they adjust automatically with the size of the parent)
 - Its position (this is automatically set for elements part of a GUILayoutX/GUILayoutY but can be used for manual positioning in GUIPanel)
 - Style that controls the look of the element, this includes the graphics, text font, color, margins and similar