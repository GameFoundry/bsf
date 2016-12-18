GUI layouts									{#guiLayouts}
===============

In previous chapter we talked about how **GUIPanel** is a special type of a GUI element called a "layout". Layouts serve as containers for GUI elements (including other layouts), and they may also control how are elements within them positioned and sized.

There are three types of layouts:
 - @ref bs::GUIPanel "GUIPanel" - Does no automatic positioning and sizing of child GUI elements, instead user can set positions and sizes manually, as we have already seen. Each panel can be placed at a different depth, allowing GUI elements to overlay eachother.
 - @ref bs::GUILayoutX "GUILayoutX" - Automatically positions and sizes child GUI elements horizontally one next to each other, left to right. User is able to request minimum/maximum allowed size, but is otherwise unable to manually position or exactly size the element.
 - @ref bs::GUILayoutY "GUILayoutY" - Same as **GUILayoutX** only elements are positioned vertically, top to bottom.

You will find that vertical and horizontal layouts come in handy when you need to design GUI that needs to scale across various screen sizes/resolutions. By adding your GUI elements to such layouts instead of manually positioning them, ensures the GUI system can always keep them at optimal position and size, regardless of the available screen area.

# Adding/removing elements
In the previous chapter we have already seen how to add a GUI element to a layout (a panel, more specifically). Here is the entire interface for dealing with element addition/removal, shared by all layout types:
 - @ref bs::GUILayout::addNewElement<Type, Args...> "GUILayout::addNewElement<T>" - Adds a GUI element of the specified type to the end of the layout's element list.
 - @ref bs::GUILayout::insertNewElement<Type, Args...> "GUILayout::insertNewElement<T>" - Inserts a GUI elements of the specified type at a specific position in the layout's element list.
 - @ref bs::GUILayout::removeElement "GUILayout::removeElement" - Removes a GUI element from the layout's element list.

Here's an example of retrieving the primary GUI panel and adding some elements to it: 
 
~~~~~~~~~~~~~{.cpp}
// Assuming "gui" is a GUI widget component
GUIPanel* mainPanel = gui->getPanel();

// Add a horizontal layout that will automatically position child elements
GUILayoutX layout = mainPanel->addNewElement<GUILayoutX>();

// Add a text label, and a button to the right of it
layout->addNewElement<GUILabel>(HString(L"This is a button: "));
layout->addNewElement<GUIButton>(HString(L"Click me"));
~~~~~~~~~~~~~ 

GUI elements can also be created separately and then added to layouts later using these methods:
 - @ref bs::GUILayout::addElement "GUILayout::addElement" - Adds an existing GUI element at the end of the layout's element list.
 - @ref bs::GUILayout::insertElement "GUILayout::insertElement" - Inserts an existing GUI elements at a specific position in the layout's element list.

Same example as above, only written differently:
 
~~~~~~~~~~~~~{.cpp}
// Assuming "gui" is a GUI widget component
GUIPanel* mainPanel = gui->getPanel();

// Add a horizontal layout that will automatically position child elements
GUILayoutX layout = GUILayoutX::create();
mainPanel->addElement(layout);

// Add a text label, and a button to the right of it
GUILabel* label = GUILabel::create(HString(L"This is a button: "));
GUIButton* button = GUIButton::create(HString(L"Click me"));

layout->addNewElement(label);
layout->addNewElement(button);
~~~~~~~~~~~~~  

The matter which style you use is personal preference.

# Horizontal layout
// TODO

@ref TODO_IMAGE

# Vertical layout
// TODO

@ref TODO_IMAGE

# Customizing
## Styles
// TODO - GUIOptions, mention styles

## Spaces
// TODO

# Compound layouts
// TODO

@ref TODO_IMAGE

# Panels
// TODO - Depth