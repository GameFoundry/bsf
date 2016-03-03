Scripting									{#scripting}
===============
[TOC]

Often when you extend the native portion of the engine in some way you might want to expose that functionality to the managed (i.e. scripting) code. This guide will show you how to create C++ objects that communicate with C# code and vice versa. 

Before we delve into the specifics of Banshee’s scripting you should understand how the scripting system works in general. All C# script code is ran from the C++ part of the engine using the Mono runtime. Mono runtime allows you to communicate with C# code (and for the C# code to communicate with C++), query class/method/field information and pass data between the two languages.

# Scripting system #	{#scripting_a}

Because using Mono directly is complex because its lack of documentation, Banshee provides a set of easy to use wrappers for almost all of Mono related functionality. 

BansheeMono is a plugin that wraps the functionality of the Mono runtime. Here is where the Mono runtime is started and assemblies are loaded (and unloaded), while also giving you detailed information about all loaded classes and a way to communicate with managed code.

The main entry point is the MonoManager class which allows you to start the runtime and load assemblies. The most important method here is MonoManager::loadAssembly. It takes as assembly path to load and returns a MonoAssembly object. 

## MonoAssembly 	{#scripting_a_a}
MonoAssembly gives you access to all classes in the assembly. You can retrieve all clases using MonoAssembly::getAllClasses, or retrieve a specific one by calling MonoAssembly::getClass(namespace, typename). Both of these methods return a MonoClass object.

## MonoClass 	{#scripting_a_b}
MonoClass gives you access to all methods, fields, properties and attributes of a specific class. It also allows you to register “internal” methods. These methods allow the managed code to call C++ code, and we’ll go into them later.

Classes also allow you to create object instances of their type. Use MonoClass::createInstance to create a new object instance. All managed objects are referenced using a MonoObject type (more on that later), which is returned from the MonoClass::createInstance call. When creating an instance you may choose whether to construct it or not, and to provide constructor signature if you need a specific one.

To retrieve a method from a class call MonoClass::getMethod(), accepting a name (without parameter types) and a number of parameters. If your method is overloaded you can use MonoClass::getMethodExact which accepts a method name, and a comma separated list of parameter types. You may also use MonoClass::getAllMethods to retrieve all methods in a class.

All the above methods return a MonoMethod object.

## MonoMethod {#scripting_a_c}
This class provides information about about a managed method, as well as giving you multiple ways of invoking it.

To invoke a method you may use multiple approaches:
 - MonoMethod::invoke - Calls the exact method on a specific managed object, with the provided parameters. We’ll take about how managed objects are referenced in native code later, as well as how passing data between C++ and managed code works.
 - MonoMethod::invokeVirtual - Calls the method polymorphically. Determines the actual type of the provided managed object instance and calls an overriden method if available.
 - MonoMethod::getThunk - Returns a C++ function pointer accepting a managed object, zero or more parameters and an exception object. You can then call the function pointer like you would a C++ function. This is equivalent to MonoMethod::invoke but is significantly faster. A helper method MonoUtil::invokeThunk is provided - it is suggested you use it instead of calling thunks manually (it handles exceptions without you needing to worry about it).

When calling static methods you should provide a null value for the managed object instance.

## MonoField {#scripting_a_d}
Similar to methods, field information can be retrieved from MonoClass by calling MonoClass::getField or MonoClass::getAllFields. The returned value is a MonoField which provides information about the field and allows you to retrieve and set values in the field using MonoField::getValue/MonoField::setValue. This works similar to how methods are invoked and is explained in more detail later.

## MonoProperty {#scripting_a_e}
Properties are very similar to fields, retrieved from MonoClass by calling MonoClass::getProperty or MonoClass::getAllProperties. The returned value is a MonoProperty which provides information about the property and allows you to retrieve and set values in it. The main difference is that properties in C# can be indexed (like arrays) and therefore a two set of set/get methods are provided, one accepting an index and other one not. It’s up to the user to know which one to call. The methods are MonoPropety::get/MonoProperty::set and MonoProperty::getIndexed/MonoProperty::setIndexed.

## Attributes {#scripting_a_f}
Attributes provide data about a class, method or field provided at runtime, and allows them to be customized in more detail. Attributes don’t have their own wrapper, because they are esentially normal managed objects and you can work with them as such.

To retrieve a list of attributes from a class use: MonoClass::getAllAttributes(), which returns a list of MonoClass objects that identify the attribute types. To get the actual object instance of the attribute you may call MonoClass::getAttribute with the wanted attribute MonoClass. After that you can call methods, work with field values and other, same as you would with a normal object.

Attributes can also be retrieved from MonoMethod using MonoMethod::getAttribute, or from MonoField using MonoField::getAttribute.

## Managed objects {#scripting_a_g}
So far we have talked about invoking methods and retrieving field values, but we haven’t yet explained how to access managed object instances in C++ code. All managed objects in Mono are represented by a MonoObject.

For example, when calling a non-static method the first parameter provided to MonoMethod::invoke is a MonoObject pointer. Same goes for retrieving or setting values on fields, properties. Attributes are also a MonoObject.

Mono also provides two more specialized types: MonoArray for managed arrays, and MonoString for managed strings, but they are both still a MonoObject.

Be aware that all managed objects are garbage collected. This means you should not keep a reference to them unless you are sure they are alive. Just having a pointer to a MonoObject will not keep the object alive and it may go out of scope as soon as the control returns to managed code. A good way to deal with this issue is:
 - Call a native method in the object’s finalizer (~MyObject()) which will notify you when the object is no longer valid. Be aware that finalizer may be called after the object is unusable.
 - Require the user to manually destroy the object by calling a custom Destroy method or similar.
 - Force the garbage collector to keep the object alive by calling “mono_gchandle_new” which will return a handle to the object. The handle will keep the object alive until you release it by calling “mono_gchandle_free”. Be aware if an assembly the object belongs to is unloaded all objects will be destroyed regardless of the kept handles.
 
## Marshalling data {#scripting_a_h}
Mono does not perform automatic marshalling of data when calling managed code from C++ (or vice versa). This is important when calling methods, retrieving/setting field/property values, and responding to calls from managed code, because you need to know in what format to expect the data.

The rules are:
 - All primitive types are passed as in. e.g. an int in C# will be a 4 byte integer in C++, a float will be a float, a bool will be a bool.
 - All reference types (“class” in C#) are passed as a MonoObject*. Strings and arrays are handled specially, where strings are passed as MonoString*, and arrays as MonoArray*.
   - If a reference type parameter in a method is prefixed with an “out” modifier, then the received parameters are MonoObject**, MonoString**, MonoArray** and your method is expected to populate those values.
 - Structs (non-primitive value types) are provided as raw memory. Make sure that all structs in C# have a ”[StructLayout(LayoutKind.Sequential)]” attribute, which ensures they have the same memory layout as C++ structs. This way you can just accept the raw C++ structure and read it with no additional conversion.
  - It is suggested you never pass structures by value, it is known to cause problems in Mono. Instead pass all structures by prefixing them with “ref” which will give you a pointer to the structure in managed code (e.g. MyStruct*). If you need to output a struct use the “out” modifier which you will give you a double pointer (e.g. MyStruct**).
  - In cases where it is not possible to avoid passing structures by value (e.g. when retrieving them from a field, use the MonoField::getValueBoxed method instead MonoField::getValue, which will return a struct in the form of a MonoObject. You can then retrieve the raw struct value by calling mono_object_unbox.
  
Banshee provides a helper code to assist with marshalling string:
 - MonoUtil::toWString/MonoUtil::toString - Converts a MonoString* to a native string
 - MonoUtil::fromWString/MonoUtil::fromString - Converts a native string into a MonoString*

ScriptArray allows you to construct new arrays and read managed arrays, easily. 

To create a new arrays call ScriptArray<Type>::create. Type can be a primitive type like int, float, a native string or a Script* object (more about Script* objects later). You can then fill the array by calling ScriptArray::set and retrieve the managed MonoArray* by calling ScriptArray::getInternal.

To more easily read existing arrays create a new ScriptArray by providing it with a MonoArray* as its parameters. Then you can easily retrieve the size of the array using ScriptArray::size(), and the value of its elements by calling ScriptArray::get<Type>. 

## Internal methods {#scripting_a_i}
So far we have talked about calling managed code, and retrieving information about managed types, but we have yet to show how managed code calls C++ code. This is accomplished using native methods.

The first step is to define a stub method in managed code, like so:
	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern float Internal_GetSomeValue(MyObject obj);

You then hook up this method with managed code by calling MonoClass:addInternalMethod. In this specific case it would be myClass->ddInternalMethod(“Internal_GetSomeValue”, &myNativeFunction), assuming myClass is a MonoClass of the type that contains the stub method. After this call any call to the managed stub method will call the provide native function myNativeFunction. You should take care to properly handle parameter passing as described above.

Take a look at ScriptGUISkin implementation for a simple example of how exactly does this work. 

# Script objects {#scripting_b}
As you can see interaction between the two languages can get a bit cumbersome. For that reason Banshee implements a higher level system built on the functionality shown so far. It provides an universal interface all script objects must implement. It primarily ensures that native and managed code is always linked by keeping a pointer to each other's objects, as well as gracefully handling managed object destruction and handling assembly refresh (due to script hot-swap).

TODO - ScriptObject, SCRIPT_OBJ macro, initRuntimeData, metaData
TODO - Pointer caching (both on managed and native sides)
TODO - Assembly refresh, isPersistent, _beginRefresh(), _endRefresh(), _clearManagedInstance(), _restoreManagedInstance, _onManagedInstanceDeleted

