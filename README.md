#Exam Advanced Programming 2019/2020
This project had the goal to implement a binary search tree (BST)  in modern C++14 language.
##How to compile and run:
- Change to source directory.
- In Terminal, write `make` and enter.
- Write `./exam_Kevin_Mattia.x` and enter.

## What we have implemented:
### Struct `node`
- It has a `content` (`std::pair<const key, value>`), and two `std::unique_ptr<node>` pointing to the left child and the right child, respectively, and one `node*` pointing to the parent.
- Copy and move constructor

### Class `Iterator`
- It is templated with the types `node` and `value`.
- Default copy constructor, custom copy constructor and deep copy constructor.
- Only private member is a pointer to the node that it points to.
- Three auxiliary functions that help with identifying the tree structure.
- Public aliases (using `using`) are: `value_type`, `difference_type`, `iterator_category`, `reference` and `pointer`
- Public functions are `go_left()`, `go_right()` and `go_up()` to travel the tree, `get_Lchild()`, `get_Rchild()`, `get_parent()` to access the members of `node`, moreover `get_ptr()` to get the pointer to the associated node.
- Operations are `operator==` and `operator!=`, `operator++`, `operator--`, `operator*` and `operator->`.

### Class `const_Iterator`
- Like Iterator, but through `const_Iterator` we cannot change the node itself, whereas through `Iterator` we can.
- Use `using Iterator=_Iterator<node,value>` to construct a `const_Iterator` from an `Iterator`.
- Necessary for `BST::operator<<`.

### Class `BST` 
- It is templated with key type `K`, value type `V`, and comparison operation of type `CO`, with a default of `std::less<K>`.
- Via the `using` keyword, we published the Iterator and the const_Iterator with the BST templates as a public member alias. 
- Four private members: `std::unique_ptr<node> root`; `node header{pair(), nullptr, nullptr, nullptr}`,
  `CO lesscompare=CO()`, and a list of `std::pair` called `remember`, which keeps record of the insertion order of the nodes into the BST.
- The node `header` is a particular node needed for `operator++`, `operator--`, and for the generic work with the tree structure. It is connected the root and the rightmost node.
- Default copy constructor, move default constructor and custom destructor (which uses `BST::clear()`, see below). 
- Custom deep copy constructor: Uses list `remember` of the tree to be copied, and inserts the nodes in the same order in a new tree.



#### Public functions of `BST`
All the functions which may accept r- and l-values as arguments are implemented with universal references. 
- `get_root()`: gives us a (const or non-const) Iterator to the root node.
- `begin()`: gives us a (const or non-const) Iterator to the leftmost node.
- `end()`: gives us a (const or non-const) Iterator to `header`.
- `insert()`: Takes an `std::pair<const key_type, value_type>` as input argument. Travels inside the tree via Iterators and uses `std::make_unique<node>` to insert a new node at the right place. Returns an `std::pair` with the Iterator of the inserted node and a boolean indicating whether the node was inserted or not.
- `emplace()`: Takes either key and value as separate arguments, or a pair containing a key and a value. Constructs a pair from them and inserts it through `insert()`.
- `clear()`: First, resets the Rchild of the rightmost node, preventing double deletion of `header`. After that, resets the root, envoking the destruction method from `std::unique_ptr`. Every consecutive child resets their children, causing all nodes to be deleted. The fact that `parent` is a `node*` prevents double deletion of nodes. 
- `find()`: Takes as input the key. Travels the tree like `insert()`. Once the Iterator encounters a node with the corresponding key, it returns itself (the Iterator).
- `operator[]`: Takes a key as an argument, locates the corresponding node using `find()`, and returns the associated value.
- `operator<<`: Takes as an argument a `const BST<>&` Uses `operator++` on an iterator to loop through all nodes of the tree.
- `erase()`: Takes as an argument a key and removes the corresponding pair from `remember. Then, it reconstructs the entire tree from this modified list.
- `balance()`: Uses the external function `sorted_vector_for_balance_recursively()` to construct an `std::vector<std::pair>`, corresponding to the nodes in the original tree, but ordered in a specific way. The pairs are then inserted in the cleared tree, according to the order in the vector, to construct a balanced tree.
- `insertion_order()`: Auxiliary function. Prints the keys of the pairs inside `remember`.



### External functions
- `sorted_vector_for_balance_recursively()`: Takes list `remember` and recursively constructs a permutation of it which will serve as a blueprint for the insertion order of a balanced tree. 



### General comments on implementation
- We used the keyword `noexcept` for any function that does not assign new memory (stack or heap) inside the function body.
- For visual clarity, we decided to use out-of-class definitions for all member functions and operators (except for `BST::operator<<`) that are not constructors/destructors, or that need only one line. 
- We did not add an out-of-class definition of `BST::operator<<` since we could not resolve related compiler errors.
- For debugging purposes, we chose to enclose additional information between `#ifdef [flag]` and `#endif`, where the flag names the respective subject, e.g. "find", "operator[]", etc. In contrast to that, we did not use `catch` and `try` statements. 

 