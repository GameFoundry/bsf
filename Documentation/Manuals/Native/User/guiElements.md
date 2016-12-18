GUI elements									{#guiElements}
===============

A GUI element is a basic primitive GUI is constructed out of. They can be text, buttons, input boxes, images, scroll areas and more. We'll explain what the individual GUI element types are later, but initially we'll focus on functionality common to all GUI elements.

# Displaying a GUI element
In order to display a GUI element we must first create it. All GUI elements are created using a static *create* method.

~~~~~~~~~~~~~{.cpp}
// GUILabel is a type of GUI element that displays the provided text on screen
GUILabel* label = GUILabel::create(HString(L"Hello!"));
~~~~~~~~~~~~~

But just creating the element is not enough. We must also register it with our **GUIWidget**. To do that we must first retrieve the primary @ref bs::GUIPanel "GUIPanel" from the widget. **GUIPanel** serves as an element container, and by default every widget has one. Use @ref bs::CGUIWidget::getPanel() "CGUIWidget::getPanel()" to retrieve the panel.

~~~~~~~~~~~~~{.cpp}
GUIPanel* mainPanel = gui->getPanel();
~~~~~~~~~~~~~

**GUIPanel** is a special type of a GUI element called a "layout". We'll discuss layouts in more detail in the next chapter, but for now all you need to know is that they are element containers you can add and remove other GUI elements to/from.

To add an element to the panel use @ref bs::GUIPanel::addElement "GUIPanel::addElement".

~~~~~~~~~~~~~{.cpp}
mainPanel->addElement(label);
~~~~~~~~~~~~~

At this point our GUI element will be displayed.

@ref TODO_IMAGE

# Customizing GUI elements
## Changing position
// TODO

## Changing size
// TODO

## Changing color
// TODO

## Hiding
// TODO (Both active and visible)

# GUI element types
## Label
// TODO

## Button
// TODO

## Toggle
// TODO

## Input box
// TODO

## List box
// TODO

## Texture
// TODO

## Slider
// TODO

## Scroll area
// TODO