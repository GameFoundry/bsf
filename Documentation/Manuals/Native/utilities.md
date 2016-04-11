Utilities									{#utilities}
===============
[TOC]

This manual will quickly go over all the important utility systems commonly used by Banshee. We won't go into major detail about these features, but will rather point you towards the relevant API documentation.

# Module {#utilities_a}
A @ref BansheeEngine::Module<T> "Module<T>" is a specialized form of singleton used for many of Banshee's systems. Unlike normal singletons it requires manual startup and shutdown, which solves many of the problems associated with traditional singletons.

To use it for your own objects, simply inherit from it:
~~~~~~~~~~~~~{.cpp}
class MyModule : public Module<MyModule>
{ };
~~~~~~~~~~~~~

Use @ref BansheeEngine::Module<T>::startUp "Module<T>::startUp" to start it up. Once started use @ref BansheeEngine::Module<T>::instance "Module<T>::instance" to access its instance. Once done with it call @ref BansheeEngine::Module<T>::shutDown "Module<T>::shutDown" to release it. For example:
~~~~~~~~~~~~~{.cpp}
MyModule::startUp();
MyModule::instance().doSomething();
MyModule::shutDown();
~~~~~~~~~~~~~

# Path {#utilities_b}
Use @ref BansheeEngine::Path "Path" to manipulate file/folder paths. Initialize it with a path string and then call various methods to manipulate it. It is recommended to always store paths using @ref BansheeEngine::Path "Path" instead of raw strings.

Some of the things you can do once a @ref BansheeEngine::Path "Path" is constructed:
 - Retrieve the filename using @ref BansheeEngine::Path::getFilename "Path::getFilename"
 - Retrieve the filename extension using @ref BansheeEngine::Path::getExtension "Path::getExtension"
 - Get last element of path, either file or directory using @ref BansheeEngine::Path::getTail "Path::getTail"
 - Iterate over directories, get drive, combine paths, convert relative to absolute paths and vice versa, and more...
 
For example:
~~~~~~~~~~~~~{.cpp}
Path myPath("C:\\Path\\To\\File.txt");

String filename = myPath.getFilename(); // Returns file
myPath.setExtension(".jpg"); // Path is now "C:\Path\To\File.jpg"
myPath.makeRelative("C:\\Path"); // Path is now "To\File.jpg"

Path a("C:\\Path\\To\\");
Path b("File.txt");
Path combined = a + b; // // Path is now "C:\Path\To\File.txt"
~~~~~~~~~~~~~
 
All @ref BansheeEngine::Path "Path" methods that return strings come in two variants, one that returns a narrow (8-bit) character string like @ref BansheeEngine::Path::getFilename "Path::getFilename", and one that contains wide character string like @ref BansheeEngine::Path::getWFilename "Path::getWFilename".

When setting paths be careful with setting backslashes or slashes at the end of the path. Path with a no backslash/slash on the end will be interpreted as a file path, and path with a backslash/slash will be interpreted as a folder path. For example:
 - "C:\MyFolder" - "MyFolder" interpreted as a file, @ref BansheeEngine::Path::getFilename "Path::getFilename" returns "MyFolder"
 - "C:\MyFolder\" - "MyFolder" interpreted as a folder, @ref BansheeEngine::Path::getFilename "Path::getFilename" returns an empty string
 
# File system {#utilities_c}
The @ref BansheeEngine::FileSystem "FileSystem" module allows you to open and create files, create folders, move/copy/remove files and folders, iterate all folder/files in a folder, get file size, check if folder/folder exists, get working path and others.

An example creating a folder and a file:
~~~~~~~~~~~~~{.cpp}
FileSystem::createDir("C:\\Path\\To\\");
SPtr<DataStreamPtr> fileStream = FileSystem::createAndOpenFile("C:\\Path\\To\\File.txt");
... write to data stream...
~~~~~~~~~~~~~
# Data streams {#utilities_d}
@ref BansheeEngine::DataStream "Data streams" allow you to easily write/read binary/text data from/to disk/memory/etc. The two primary types of streams are @ref BansheeEngine::MemoryDataStream "MemoryDataStream" for reading/writing directly to memory, and @ref BansheeEngine::FileDataStream "FileDataStream" for reading/writing to a file.

You create memory streams by providing them with a pointer and size of a memory buffer, while you create file streams by calling @ref BansheeEngine::FileSystem::openFile "FileSystem::openFile" or @ref BansheeEngine::FileSystem::createAndOpenFile "FileSystem::createAndOpenFile". Once you are done with a stream make sure to close it by calling @ref BansheeEngine::DataStream::close "DataStream::close". Stream will also be automatically closed when it goes out of scope.

Once you have a stream you can seek to a position within a stream and read/write to it. For example:
~~~~~~~~~~~~~{.cpp}
SPtr<DataStreamPtr> fileStream = FileSystem::createAndOpenFile("C:\\Path\\To\\File.txt");
// Write some string data
fileStream.writeString("Writing to a file");
// Write some binary data
UINT8* myBuffer = bs_alloc(1024);
... fill up the buffer with some data ...
fileStream.write(myBuffer, 1024);

fileStream.close();
~~~~~~~~~~~~~
 
# Events {#utilities_e}
@ref BansheeEngine::TEvent<RetType, Args> "Events" allow your objects to expose events that may trigger during execution. External objects interested in those events can then register callbacks with those events and be notified when they happen. They are useful because they allow two objects to communicate without necessarily knowing about each other's types, which can reduce class coupling and improve design.

When creating an event, all you need to do it specify a format of the callback it sends out, for example:
~~~~~~~~~~~~~{.cpp}
class MySystem
{
	static Event<void()> myEvent; // Callback with no parameters
	static Event<void(UINT32)> myEvent2; // Callback with a UINT32 parameter
};
~~~~~~~~~~~~~

Then an external object can register itself with an event by calling @ref BansheeEngine::TEvent<RetType, Args> "Event::connect". This method will return an @ref BansheeEngine::HEvent "HEvent" handle. You can use this handle to manually disconnect from the event by calling @ref BansheeEngine::HEvent::disconnect "HEvent::disconnect". For example:
~~~~~~~~~~~~~{.cpp}
// Subscribe to an event we defined previously
// Simply pass a function pointer matching the callback
HEvent eventHandle = MySystem::myEvent2.connect(&myEventReceiver);

void myEventReceiver(UINT32 val)
{
	// Do something
}
~~~~~~~~~~~~~

When using non-static class methods as callbacks, things get a little bit more complicated. This is because each such method by default expects a pointer to an instance of itself (`this` pointer). This is normally hidden from the programmer and happens under the hood, but we must handle it when dealing with callbacks. We can do this by using `std::bind` which allows us to replace function arguments with constant values. For example:
~~~~~~~~~~~~~{.cpp}
class EventReceiver
{
	EventReceiver()
	{
		// Convert a method with signature void(EventReceiver*, UINT32) into void(UINT32) by binding the "EventReceiver*"
		// argument to the value of "this". Read up on the C++ library for more information about std::bind.
		MySystem::myEvent2.connect(std::bind(&EventReceiver::myEventReceiver, this, std::placeholders::_1));
	}
	
	void myEventReceiver(UINT32 val)
	{
		// Do something
	}
};
~~~~~~~~~~~~~

Then when an object is ready to trigger an event simply call it like a functor:
~~~~~~~~~~~~~{.cpp}
MySystem::myEvent(); // Trigger an event with no arguments
MySystem::myEvent2(5); // Trigger an event with a single argument
~~~~~~~~~~~~~

# Any {#utilities_f}
Use the @ref BansheeEngine::Any "Any" type to easily store any kind of object in it. For example:
~~~~~~~~~~~~~{.cpp}
Any var1 = Vector<String>();

struct MyStruct { int a; };
Any var2 = MyStruct();
~~~~~~~~~~~~~

Use @ref BansheeEngine::any_cast "any_cast" and @ref BansheeEngine::any_cast_ref "any_cast_ref" to retrieve valid types from an @ref BansheeEngine::Any "Any" variable. For example:
~~~~~~~~~~~~~{.cpp}
Vector<String> val1 = any_cast<Vector<String>>(var1);
MyStruct& val2 = any_cast_ref<MyStruct>(var2);
~~~~~~~~~~~~~
# Flags {#utilities_g}
@ref BansheeEngine::Flags<Enum, Storage> "Flags" provide a wrapper around an `enum` and allow you to easily perform bitwise operations on them without having to cast to integers. For example when using raw C++ you must do something like this:
~~~~~~~~~~~~~{.cpp}
enum class MyFlag
{
	Flag1 = 1<<0,
	Flag2 = 1<<1,
	Flag3 = 1<<2
};

MyFlag combined = (MyFlag)((UINT32)MyFlag::Flag1 | (UINT32)MyFlag::Flag2);
~~~~~~~~~~~~~

Which is cumbersome. Flags require an additional step to define the enum, but after that allow you to manipulate values much more nicely. 

To create @ref BansheeEngine::Flags<Enum, Storage> "Flags" for an enum simply define a `typedef` with your enum type provided as the template parameter. You must also follow that definition with a @ref BS_FLAGS_OPERATORS macro in order to ensure all operators are properly defined. For example:
~~~~~~~~~~~~~{.cpp}
typedef Flags<MyFlag> MyFlags;
BS_FLAGS_OPERATORS(MyFlag)
~~~~~~~~~~~~~

Now you can do something like this:
~~~~~~~~~~~~~{.cpp}
MyFlags combined = MyFlag::Flag1 | MyFlag::Flag2;
~~~~~~~~~~~~~
# String {#utilities_h}
Banshee uses @ref BansheeEngine::String "String" for narrow character strings (8-bit) and @ref BansheeEngine::WString "WString" for wide character strings. Wide character strings are different size depending on platform.

A variety of string manipulation functionality is provided in @ref BansheeEngine::StringUtil "StringUtil", like matching, replacing, comparing, formating and similar.

Conversion between various types (like int, float, bool, etc.) and string is provided via overloads of @ref BansheeEngine::toString "toString" and @ref BansheeEngine::toWString "toWString". You can also convert strings into different types by calling methods like @ref BansheeEngine::parseINT32 "parseINT32", @ref BansheeEngine::parseBool "parseBool", and similar for other types.

# Threading {#utilities_i}
## Primitives {#utilities_i_a}
The primitives perform the most basic operations related to threading. All threading primitives use the standard C++ library constructs, so for more information you should read their documentation.

### Thread {#utilities_i_a_a}
To create a new thread use @ref BansheeEngine::Thread "Thread", like so:
~~~~~~~~~~~~~{.cpp}
void workerFunc()
{
	// This runs on another thread
}

Thread myThread(&workerFunc);
~~~~~~~~~~~~~

### Mutex {#utilities_i_a_b}
Use @ref BansheeEngine::Mutex "Mutex" and @ref BansheeEngine::Lock "Lock" to synchronize access between multiple threads, like so:
~~~~~~~~~~~~~{.cpp}
Vector<int> output;
int startIdx = 0;
Mutex mutex;

void workerFunc()
{
	// Lock the mutex before modifying either "output" or "startIdx"
	// This ensures only one thread every accesses it at once
	Lock lock(mutex);
	output.push_back(startIdx++);
}

// Start two threads that write to "output"
Thread threadA(&workerFunc);
Thread threadB(&workerFunc);
~~~~~~~~~~~~~

If a mutex can be locked recursively, use @ref BansheeEngine::RecursiveMutex "RecursiveMutex" and @ref BansheeEngine::RecursiveLock "RecursiveLock" instead.

### Signal {#utilities_i_a_c}
Use @ref BansheeEngine::Signal "Signal" to pause thread execution until another thread reaches a certain point. For example:
~~~~~~~~~~~~~{.cpp}
bool isReady = false;
int result = 0;

Signal signal;
Mutex mutex;

void workerFunc()
{
	for(int i = 0; i < 100000; i++)
		result += i; // Or some more complex calculation
	
	// Lock the mutex so we can safely modify isReady
	{
		Lock lock(mutex);
		isReady = true;		
	} // Automatically unlocked when lock goes out of scope
	
	// Notify everyone waiting that the signal is ready
	signal.notify_all();
}

// Wait until the signal is triggered, or until isReady is set to true, whichever comes first
Lock lock(mutex);
if(!isReady)
	signal.wait_for(lock);
~~~~~~~~~~~~~

### Other {#utilities_i_a_d}
The previous sections covered all the primitives, but there is some more useful functionality to be aware of:
 - @ref BS_THREAD_HARDWARE_CONCURRENCY - Returns number of logical CPU cores.
 - @ref BS_THREAD_CURRENT_ID - Returns @ref BansheeEngine::ThreadId "ThreadId" of the current thread.
 - @ref BS_THREAD_SLEEP - Pauses the current thread for a set number of milliseconds.

## Thread pool {#utilities_i_b}
Instead of using thread primitive described in the previous section, you should instead use @ref BansheeEngine::ThreadPool "ThreadPool" for running threads. @ref BansheeEngine::ThreadPool "ThreadPool" allows you to re-use threads and avoid paying the cost of thread creation and destruction. It keeps any thread that was retired in idle state, and will re-use it when user requests a new thread.

An example:
~~~~~~~~~~~~~{.cpp}
void workerFunc()
{
	// This runs on another thread
}

ThreadPool::instance().run("MyThread", &workerFunc);
~~~~~~~~~~~~~

## Task scheduler {#utilities_i_c}

# Math {#utilities_j}
// Math, Vector, Matrix

# Time {#utilities_k}

# Logging {#utilities_l}

# Crash handling {#utilities_m}

# Dynamic libraries {#utilities_n}

# Testing {#utilities_o}

# Allocators {#utilities_p}
## General {#utilities_p_a}

## Frame {#utilities_p_b}

## Stack {#utilities_p_c}

## Static {#utilities_p_d}