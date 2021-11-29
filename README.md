# Arch_Project
Multicore-Pipelined CPU with MESI Bus for coherency.  
Each core has a single data cache and a singles instruction cache.  
Assembly function for testing are written as well.  
<br>

divided into the following modules:
## Core
core.c & core.h:     design & behavioural of pipelined cores with cache

## Memory
memory.c & memory.h: design & behavioural of caches, MESI bus and main memory

## Sim
sim.h & sim c:       design & behavioural of complete CPU  
                     simulation interface (IO files)

Gai Greenberg & Iris Taubkin & Omri Elad
