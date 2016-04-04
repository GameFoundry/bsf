Banshee Engine Documentation (Native)						{#mainpage}
===============

[TOC]

Welcome to the documentation for the native (C++) API of the Banshee Engine.

This documentation contains a set of [manuals](@ref manuals), an <a class="el" href="modules.html">API reference</a> and an alphabetical <a class="el" href="annotated.html">class list</a>.

# Manuals # {#mainpage_a}
Manuals should be your primary entry point into learning about Banshee. They will provide you with a view of the general architecture of the engine, as well as the architecture of the more important systems. They will also teach you have to extend/modify various parts of the engine, including adding custom GUI types, script objects, resources, importers, renderers and many more.
 
The manuals generally do not cover user-facing functionality, and focus more on explaining the engine internals. To learn about user-facing functionality read the [scripting documentation](@ref TODO_LINK) - the C++ interfaces are very similar and are usually just a direct reflection of their script counterparts.
 
[Visit](@ref manuals) 
 
# API Reference # {#mainpage_b}
API reference provides a categorized and hierarchical view of all the engine's classes. 

All classes are categorized into two primary groups:
 - **Layers** - Layers make up the core of the engine. Each layer is built directly on top of the previous one. This is what most people will be interested in. Each layer also contains an **INTERNAL** category which contains lower level classes that are not meant for normal use, but can be important for those modifying the engine.
 - **Plugins** - Plugins are various interchangeable libraries that contain high level systems built on top of abstractions defined in the layers. If you are modifying the engine you might be interested in this documentation, but it can be skipped for most normal users. 
 
A separate **IMPLEMENTATION** category is also provided which contains base classes and templates that are used in construction of other types. You will almost never need this documentation as those specialized types inherit the documentation from their parents. 
 
You should read the [architecture](@ref architecture) manual for a more detailed breakdown of the architecture.

<a class="el" href="modules.html">Visit</a>

# Class list # {#mainpage_c}
Contains the same information as the API references, but with all clases listed in a flat list, alphabetically.

<a class="el" href="annotated.html">Visit</a>