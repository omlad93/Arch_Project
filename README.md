# Arch_Project
Simulation of a Multicore-Pipelined CPU with MESI Bus for coherency.  
Each core has a single data cache and a singles instruction cache.  
Assembly function for testing are written as well.  
## Gai Greenberg & Iris Taubkin & Omri Elad  
<br>

The simulation is divided into the following modules:
## Core
<pre>
core.c & core.h:     design & behavioural of pipelined cores with cache
</pre>

## Memory
<pre>
memory.c & memory.h: design & behavioural Memory System:
                     caches, MESI bus and main memory
memory_debug.c:      a module holding main() debug of memory system
</pre>

## Sim
<pre>
sim.h & sim c:       design & behavioural of complete CPU  
                     simulation interface (IO files)
                     inludes main();
</pre>

