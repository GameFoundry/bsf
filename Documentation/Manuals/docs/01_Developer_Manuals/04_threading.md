---
title: Threading
---

In this chapter we'll show how to start new threads of execution and how to safely synchronize between them. We'll start with explanation of the basic threading primitives, and then move onto higher level concepts like the thread pool and task scheduler.

# Primitives
This section describes the most basic primitives you can use to manipulate threads. All threading primitives use the standard C++ library constructs, so for more information you should read their documentation.

## Thread
To create a new thread use @Thread, with its constructor parameter being a function pointer of the function that will execute on the new thread.
~~~~~~~~~~~~~{.cpp}
void workerFunc()
{
	// This runs on another thread
}

Thread myThread(&workerFunc);
~~~~~~~~~~~~~

## Mutex
Use @Mutex and @Lock to synchronize access between multiple threads. **Lock** automatically locks the mutex when it's constructed, and unlocks it when it goes out of scope.

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

If a mutex can be locked recursively, use @RecursiveMutex and @RecursiveLock instead.

## Signal
Use @Signal to pause thread execution until another thread reaches a certain point.

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

// Start executing workerFunc
Thread myThread(&workerFunc);

// Wait until the signal is triggered, or until isReady is set to true, whichever comes first
Lock lock(mutex);
if(!isReady)
	signal.wait_for(lock);
~~~~~~~~~~~~~

## Other
The previous sections covered all the primitives, but there is some more useful functionality to be aware of:
 - @BS_THREAD_HARDWARE_CONCURRENCY - Returns number of logical CPU cores.
 - @BS_THREAD_CURRENT_ID - Returns @ThreadId of the current thread.
 - @BS_THREAD_SLEEP - Pauses the current thread for a set number of milliseconds.

# Thread pool
Instead of using **Thread** as described in the previous section, you can instead use the @bs::ThreadPool module for running threads. It allows you to re-use threads and avoid paying the cost of thread creation and destruction. It keeps any thread that was retired in idle state, and will re-use it when user requests a new thread.

An example:
~~~~~~~~~~~~~{.cpp}
void workerFunc()
{
	// This runs on another thread
}

ThreadPool::instance().run("MyThread", &workerFunc);
~~~~~~~~~~~~~

# Task scheduler
@bs::TaskScheduler module allows even more fine grained control over threads. It ensures there are only as many threads as the number of logical CPU cores. This ensures good thread distribution accross the cores, so that multiple threads don't fight for resources on the same core.

It accomplishes that by storing each worker function as a @bs::Task, which it then dispatches to threads that are free. This ensure you can just queue up as many tasks as required without needing to worry about efficiently utilizing CPU cores.

To create a task call @bs::Task::create with a task name, and a function pointer that will execute the task code.

~~~~~~~~~~~~~{.cpp}
void workerFunc()
{
	// This runs on another thread
}

SPtr<Task> task = Task::create("MyTask", &workerFunc);
~~~~~~~~~~~~~

Then run the task by calling @bs::TaskScheduler::addTask().

~~~~~~~~~~~~~{.cpp}
TaskScheduler::instance().addTask(task);
~~~~~~~~~~~~~

Tasks can also have priorities and dependencies. Normally tasks start executing in the order they are submitted, but tasks with a higher priority will execute sooner than those with a lower priority. In case some tasks depend on another task you can set up a dependency, which will ensure the dependant task only executes after its dependency has finished.

Both priorities and dependencies are provided as extra parameters to the **Task::create()** method.

~~~~~~~~~~~~~{.cpp}
int a;
int b;

void depencyWorkerFunc() 
{
	a = 5 + 3;
}

void workerFunc() 
{
	b = a * 8;
}

SPtr<Task> dependency = Task::create("MyDependency", &depencyWorkerFunc);

// Run task with high priority, and a dependency on another task
SPtr<Task> task = Task::create("MyTask", &workerFunc, TaskPriority::High, dependency);

TaskScheduler::instance().addTask(dependency);
TaskScheduler::instance().addTask(task);
~~~~~~~~~~~~~

You can cancel a task by calling @bs::Task::cancel(). Note this will only cancel it if it hasn't started executing already.

~~~~~~~~~~~~~{.cpp}
task->cancel();
~~~~~~~~~~~~~

Finally, you can block the current thread until a task finished by calling @bs::Task::wait.

~~~~~~~~~~~~~{.cpp}
task->wait();
// Task guaranteed to be finished at this point
~~~~~~~~~~~~~
