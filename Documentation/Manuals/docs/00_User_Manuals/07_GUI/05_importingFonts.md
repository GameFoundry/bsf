---
title: Importing fonts
---

Fonts control how text characters look and are used primarily throughout the GUI system. They are represented with the @bs::Font class. A font is a resource, meaning it can be imported, saved and loaded as we described in the resource manual.

Fonts can be imported from .TTF or .OTF formats using the importer.

~~~~~~~~~~~~~{.cpp}
// Import a font named "lato.ttf" from disk
HFont font = gImporter().import<Font>("lato.ttf");
~~~~~~~~~~~~~

# Customizing import
Font import can be customized by providing a @bs::FontImportOptions object to the importer.

~~~~~~~~~~~~~{.cpp}
auto importOptions = FontImportOptions::create();
// Set required options here (as described below)

HFont font = gImporter().import<Font>("lato.ttf", importOptions);
~~~~~~~~~~~~~

A variety of properties can be customized on import, the most important of which being font size, font style, DPI and a range of characters to import.

## Font size
Set @bs::FontImportOptions::fontSizes with a list of font sizes (in point units) to import. Usually whenever you are using a font, you will be also asked for a font size. If a font is imported without the requested size, the nearest size will be used instead.

~~~~~~~~~~~~~{.cpp}
// Import font of size 11, 12 and 16
importOptions->fontSizes = { 11, 12, 16 };
~~~~~~~~~~~~~

## Font style
When it comes to font style, you can control three options: bold, italic and rendering mode.

### Bold
When bold font is enabled, the font characters will be rendered thicker than normal. Set @bs::FontImportOptions::bold to enable/disable this option.

~~~~~~~~~~~~~{.cpp}
importOptions->bold = true;
~~~~~~~~~~~~~

### Italic
When italic font is enabled, the font characters will be rendered slightly skewed. Set @bs::FontImportOptions::italic to enable/disable this option.

~~~~~~~~~~~~~{.cpp}
importOptions->italic = true;
~~~~~~~~~~~~~

### Render mode
Allows you to control should smoothing be applied to rendered fonts, and what kind. See the @bs::FontRenderMode enumeration for more information. Set the render mode through @bs::FontImportOptions::renderMode.

~~~~~~~~~~~~~{.cpp}
importOptions->renderMode = FontRenderMode::HintedSmooth;
~~~~~~~~~~~~~

## DPI
DPI stands for dots per inch. It depends on the resultion of the user's display device versus the physical size of the display surface. Devices that have more pixels per inch will have higher DPI. 

Setting a higher DPI will make your fonts be bigger (in terms of pixels). This ensures the physical size of the displayed characters remains the same over devices with different resolution vs. size ratios.

~~~~~~~~~~~~~{.cpp}
// Set to extremely high DPI used by a full HD phone screen
importOptions->setDPI(450);

// Or DPI used by most full HD monitors
importOptions->setDPI(96);
~~~~~~~~~~~~~

## Character range
When importing a font you must specify a set of characters to import. By default all english letters and other common characters are imported, but if you also require other character sets (e.g. Cyrillic, Japanese, Korean) you must specify them by setting @bs::FontImportOptions::charIndexRanges.

The provided range uses Unicode character codes. You can find indices for various sets at the official [Unicode page](http://www.unicode.org/charts/).

~~~~~~~~~~~~~{.cpp}
// Add Cyrillic characters
importOptions->charIndexRanges = { CharRange(0x400, 0x4FF) };
~~~~~~~~~~~~~
