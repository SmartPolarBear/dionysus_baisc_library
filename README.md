# Data Structure to Be Used in Project-Dionysus

With the [project-dionysus](https://github.com/SmartPolarBear/project-dionysus) getting progress in future development, there's more and more demands of low-runtime-cost data structure implementations that provide the ability to be thread-safe. 

- **list.h** linked list with the interface of locking. 
- **avl_tree.h** avl tree , providing the similar interface with STL map or set. 
- **hash_table.h** a hash table providing similar interface with STL unordered_map. 
- **skip_list.h** a skip list providing the similar interface with STL map or set. 
- **priority_queue.h** binary heap, similar to STL priority_queue.  

For the sake of performance and the requirement of kernel development, all the classes above are designed to be *intrusive*.  

## Progress

### Containers: 

Feature                  |Finished ?         |Notes
-------------------------|:-----------------:|-----------------
list.h                   |✅                 | Complete lock facility. Lockless interfaces are in plan.
avl_tree.h               |⭕                 |
hash_table.h             |❎                 |
priority_queue.h         |❎                 |
skip_list.h              |❎                 |

### Tools: 

#### utility.h
Feature                  |Finished ?  |Description             | Notes 
-------------------------|:----------:|:-----------------------|-----------------
``` kbl::reversed_iterator```   |⭕           |Universal interface for reversed_range iterators|Only complete for sequential access iterator
```kbl::reversed_range```|✅| reverse container adapter with pipe-like operators supported|
## License
Copyright (c) 2021 SmartPolarBear

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.