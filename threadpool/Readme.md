# Thread Pool
Man Andrei Vlad 336CA

## Flow

### Main 
- we first create the Thread Pool using the implemented function.
- then we call thread_start

### Thread Start 
thread_start is a function where we
- initialize the tasks and add them in the pool
- create the threads (in order to start them)
- call the thread_stop function
- join the threads to end execution

### Process Node
- each task has as arguments the pool and a assigned node.
- we first check if the node is visited (using Lock), then we mark it as visited
- then we increment the sum (using another Lock)
- we take each child that isn't visited and create a new task for it, adding it to the pool

### Thread Stop
- busy waits until the processing is done and marks the flag as done

### Processing is done ?
- when all the nodes are visisted we return value 1, else 0

### How the Thread Pool works
- we run in a loop, take tasks using a Lock (if available), execute tasks, and repeat untill the Stack is empty and should stop flag is 1

### The Pool
- we use a linked list with the scope of a Stack, adding tasks in front and taking them from the front (to simulate a DFS because). If it would work as a Queue, there would be no reason to add children nodes inside because all the nodes are already added in the begining (graph isn't conex)


