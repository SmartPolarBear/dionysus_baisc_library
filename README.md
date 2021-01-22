# Data Structure to Be Used in Project-Dionysus

- **list.h** linked list with the interface of locking 
- **avl_tree.h** avl tree , providing the similar interface with STL map or set (working) 
- **hash_table.h** a hash table providing similar interface with STL unordered_map (working) 
- **skip_list.h** a skip list providing the similar interface with STL map or set (working) 
- **priority_queue.h** binary heap, similar to STL priority_queue

For the sake of performance and the requirement of kernel development, all the classes above are designed to be *intrusive*. 

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