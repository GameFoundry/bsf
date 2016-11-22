@page queuesDoc Device queues

Using a non-default queue index allows the GPU to perform operations on different queues in parallel (for example, streaming texture data from the CPU while the GPU is rendering).

When a write operation is being performed it is the responsibility of the caller not to use that resource until that write has completed. This can be ensured by providing a sync mask to commands that accept it (such as RenderAPICore::executeCommands) with the bit corresponding to the queue index set. This way queues will know to wait until write completes. This is not required if the resource is being used on the same queue the write is being performed on, as operations within a queue are sequential.

This value is a global queue index which encodes both the queue type and queue index. Retrieve it from CommandSyncMask::getGlobalQueueIdx().