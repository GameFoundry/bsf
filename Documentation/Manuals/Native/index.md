bs::framework Documentation						{#mainpage}
===============

Welcome to the documentation for bs::framework.

This documentation contains a set of manuals and a complete API reference.

# Manuals # {#mainpage_a}
Manuals should be your primary entry point into learning about bs::f. They can be split into two main categories:
 - [User manuals](@ref manuals)  - They explain how to use the provided API in order to create an application of your own, guiding you from basics through all the important systems. This is what most people will be interested in.
 - [Developer manuals](@ref devManuals) - They will provide you with a view of the general architecture of the framework, as well as the architecture of the more important systems. They give you deeper insight of how everything works, as well as teaching you how to
 extend/modify various parts of the framework.
 
# API Reference # {#mainpage_b}
API reference provides a categorized and hierarchical view of all the framework's classes. 

All classes are categorized into three primary groups:
 - <a class="el" href="group___layers.html">Core API</a> - Contains documentation for the user-facing API. This represents the main API that normal users will work with. 
 - <a class="el" href="group___internals.html">Internals</a> - Reference documentation for internals of the framework core. Primarily useful for those modifying the framework but of less relevance for normal users.
 - <a class="el" href="group___plugins.html">Plugins</a> - Reference documentation for all available plugins. Plugins are various interchangeable libraries that contain high level systems built on top of abstractions defined in the framework core. If you are modifying the framework you might be interested in this documentation, but it can be skipped for most normal users.