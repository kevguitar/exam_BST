#include <iostream>
#include <memory>   // std::unique_ptr
#include <utility>  // std::pair, std::less
#include <list>      
#include <vector>

/* test flags */ 
//#define test_put_to
//#define test_ctor //node and BST
//#define test_ctor_IT
//#define test_ctor_cIT
//#define test_deepcopy
//#define test_insert
//#define test_emplace
//#define test_find
//#define test_square_bra
//#define test_clear
//#define test_balance
//#define test_erase

/* Takes list `remember` and recursively constructs a permutation of it which 
   will serve as a blueprint for the insertion order of a balanced tree.  */
template<class K, class V>
void sorted_vector_for_balance_recursively(std::vector<std::pair<K,V>>& new_vec, 
                                           std::vector<std::pair<K,V>> old_vec, 
                                           int first, int last){  
  if(new_vec.size() < old_vec.size()){
    if (last < first){ /* "Come back" statement for when you arive in a leaf. */
      #ifdef test_balance
        std::cout<<"first = " << first << "\nlast = " << last <<std::endl;
        std::cout<<"end of (sorted_vector_for_balance_recursively)"<<std::endl;
      #endif
    }
    else{ 
      /* "int" automatically round-off floats, so we don't need "even" and "odd"
         cases. */
      int mid = (first + last)/2;  
      new_vec.push_back(old_vec[mid]); 
      #ifdef test_balance 
        std::cout<<"first = " << first << "\nlast = " << last << "\nmid = "
                 << mid << std::endl;
        std::cout << "push_back in the new_vec the key: " 
                  << (old_vec[mid]).first << std::endl;
      #endif
    
      /* Recursively constructs the left children. */
      sorted_vector_for_balance_recursively(new_vec,old_vec, first, mid - 1);  
      /* Recursively construct the right children. */
      sorted_vector_for_balance_recursively(new_vec,old_vec, mid + 1, last);  
    }
  }
  else{ /* Exit-statement from the recursive cycle. */
    #ifdef test_balance
      std::cout<<"Exit from the recursive cicle."<<std::endl;
    #endif
  }
}  

// #############################################################################
//                                     node
// #############################################################################
template<class T>
struct _node {
  T content;
  std::unique_ptr<_node> Lchild;
  std::unique_ptr<_node> Rchild; 
  _node* parent; 

  _node(const T& c, _node* L, _node* R, _node* p) 
  #ifndef test_ctor 
    noexcept  
  #endif
  : content{c}, Lchild{L}, Rchild{R}, parent{p} {
    #ifdef test_ctor
      std::cout << "node: copy ctor" << std::endl;
    #endif
  }

  _node(T&& c, _node* L, _node* R, _node* p) 
  #ifndef test_ctor 
    noexcept  
  #endif
  : content{std::move(c)}, Lchild{L}, Rchild{R}, parent{p}{
    #ifdef test_ctor
      std::cout << "node: move ctor" << std::endl;
    #endif
  }

  ~_node() 
  #ifndef test_ctor 
    noexcept  
  #endif 
  {    
    #ifdef test_ctor
      std::cout << "node: dctor" << std::endl;
    #endif
  }

};

// #############################################################################
//                                   Iterator
// #############################################################################
template<class node, class value>
class _Iterator{
  node* current;

  bool is_rmost(){
    if (!get_Rchild()) { return 0; } // If I am rmost, header is my Rchild.
    _Iterator rc{get_Rchild().get()};
    _Iterator prc{rc.get_parent()};
    _Iterator pprc{prc.get_parent()};
    return pprc == rc;
  }
  bool is_root(){ 
    _Iterator p{get_parent()};
    _Iterator pp{p.get_parent()};
    return ( (*this == pp) && get_Rchild() ); // As root, I have an Rchild. 
  }
  bool is_header(){
    _Iterator p{get_parent()};
    _Iterator pp{p.get_parent()};
    return ( (*this == pp) && !get_Rchild() ); // As heade, I have no Rchild.
  }

 public:
  using value_type = value; 
  using difference_type = std::ptrdiff_t; 
  using iterator_category = std::forward_iterator_tag; 
  using reference = value_type&; 
  using pointer = value_type*;

  _Iterator() 
  #ifndef test_ctor_IT 
    noexcept  
  #endif
  : current{} { 
    #ifdef test_ctor_IT
      std::cout << "Iterator: default ctor" << std::endl;
    #endif
  }

  _Iterator(node* pNode) 
  #ifndef test_ctor_IT 
    noexcept  
  #endif
  : current{pNode} { 
    #ifdef test_ctor_IT
      std::cout << "Iterator: custom copy ctor" << std::endl;
    #endif
  }

  _Iterator(const _Iterator& It) 
  #ifndef test_ctor_IT 
    noexcept  
  #endif
  : current{It.current} { 
    #ifdef test_ctor_IT
      std::cout << "Iterator: deep copy ctor" << std::endl;
    #endif
  }

  ~_Iterator() 
  #ifndef test_ctor_IT 
    noexcept  
  #endif
  {
    #ifdef test_ctor_IT
      std::cout<< "Iterator: dtor" << std::endl;
    #endif
  }

  _Iterator& operator=(node* pNode) 
  #ifndef test_ctor_IT 
    noexcept  
  #endif
  {
    #ifdef test_ctor_IT
      std::cout << "Iterator: copy assignment (=)" << std::endl;
    #endif
    this->current = pNode; // equivalent to: (*this).current = pNode
    return *this;  
  } 

  _Iterator& go_left() noexcept { 
    if (current) { current = current->Lchild.get(); }
    return *this; 
  } 

  _Iterator& go_right() noexcept { 
    if (current) { current = current->Rchild.get(); }
    return *this; 
  } 

  _Iterator& go_up() noexcept { 
    if (current) { current = current->parent; } 
    return *this; 
  } 

  friend bool operator!=(const _Iterator& iterator, const _Iterator& iterator2) { 
    return iterator.current != iterator2.current; 
  }

  friend bool operator==(const _Iterator& iterator, const _Iterator& iterator2) { 
    return iterator.current == iterator2.current; 
  }

  value& operator*() const noexcept {
    return current->content;
  }

  node* get_ptr() noexcept {  // This is a pointer to the node (NOT operator->()).
    return current;
  }

  _Iterator& operator++(){
    if (is_header()){
      std::cout<<"ERROR: you are trying to go to do ++ on header"<<std::endl;
      exit (EXIT_FAILURE);
     }
    if ((get_Rchild()).get() ) {
      go_right();
      while ((get_Lchild()).get()){
        go_left();
      }
    } 
    else {
      _Iterator tmp{get_parent()}; 
      if( (tmp.get_Rchild()).get() ){
        while( get_ptr() == (tmp.get_Rchild()).get() ){  // travel to "left-parent"
          go_up();
          tmp.go_up();
        }
        go_up(); // travel to the "right-parent"
      }
      else{ go_up(); } // travel to the "Right-parent"
    }
    return *this;
  } 

  _Iterator& operator--(){
    if( is_header() ){ // If in header, return rightmost.
      go_up();
      while(!(is_rmost())){
        go_right();
      }
      return *this; 
    }
    if ((get_Lchild()).get()) {
      go_left();
      while ((get_Rchild()).get()){
        go_right();
      }
    }
    else{
      _Iterator tmp{get_parent()};
      if( (tmp.get_Rchild()).get() ){
        if( get_ptr() == (tmp.get_Rchild()).get() ){
          go_up();
          return *this;
        }
      }
      if ((tmp.get_Lchild()).get()){
        while ( get_ptr() == (tmp.get_Lchild()).get() ){
          go_up();
          tmp.go_up();
          if ( is_header() ){ 
            std::cout<<"ERROR: You are trying to do -- on leftmost node."
                     <<std::endl;
            exit (EXIT_FAILURE);
          }
        }
        go_up();
      }
    }
    return *this;
  }

  std::unique_ptr<node>& get_Lchild() noexcept {
    return current->Lchild;
  }
  std::unique_ptr<node>& get_Rchild() noexcept {
    return current->Rchild;
  }
  node* get_parent() noexcept {
    return current->parent;
  }
  // Is used like :  Iterator->first, that gives you the key inside the content 
  // of the node or Iterator->second, that gives you the value inside the 
  // content of the node is equivalent to (*Iterator).first or (*Iterator).second 
  value* operator->() noexcept { 
    return &(*(*this));
  }
};

// #############################################################################
//                                const_Iterator
// #############################################################################
/* Like Iterator, but through `const_Iterator` we cannot change the node 
    itself, whereas through `Iterator` we can. */
template<class node, class value>
class _const_Iterator{  
  using Iterator=_Iterator<node,value>;
  const node* current;

  bool is_rmost(){
    if (!get_Rchild()) { return 0; } // If I am rmost, header is my Rchild.
    _const_Iterator rc{(get_Rchild()).get()};
    _const_Iterator prc{rc.get_parent()};
    _const_Iterator pprc{prc.get_parent()};
    return pprc == rc;
  }
  bool is_root(){ 
    _const_Iterator p{get_parent()};
    _const_Iterator pp{p.get_parent()};
    return ( (*this == pp) && get_Rchild() ); // As root, I have an Rchild. 
  }
  bool is_header(){
    _const_Iterator p{get_parent()};
    _const_Iterator pp{p.get_parent()};
    return ( (*this == pp) && !get_Rchild() ); // As header, I have no Rchild.
  }

 public:
  using value_type = value; 
  using difference_type = std::ptrdiff_t; 
  using iterator_category = std::forward_iterator_tag; 
  using reference = value_type&; 
  using pointer = value_type*;
  
  _const_Iterator() 
  #ifndef test_ctor_IT 
    noexcept  
  #endif
  : current{} { 
    #ifdef test_ctor_cIT
      std::cout << "const_Iterator: default copy ctor" << std::endl;
    #endif
  }
  _const_Iterator(const node* pNode) 
  #ifndef test_ctor_IT 
    noexcept  
  #endif
  : current{pNode} { 
    #ifdef test_ctor_cIT
      std::cout << "const_Iterator: custom copy ctor" << std::endl;
    #endif
  }
  _const_Iterator(const Iterator& It) 
  #ifndef test_ctor_IT 
    noexcept  
  #endif
  : current{It.current} { 
    #ifdef test_ctor_cIT
      std::cout << "const_Iterator: deep copy ctor" << std::endl;
    #endif
  }
  _const_Iterator(const _const_Iterator& It) 
  #ifndef test_ctor_IT 
    noexcept  
  #endif
  : current{It.current} { 
    #ifdef test_ctor_cIT
      std::cout << "const_Iterator: deep copy ctor" << std::endl;
    #endif
  }
  ~_const_Iterator() 
  #ifndef test_ctor_IT 
    noexcept  
  #endif
  {
    #ifdef test_ctor_cIT
      std::cout<< "const_Iterator: d_ctor" << std::endl;
    #endif
  }

  _const_Iterator& operator=(node* pNode) 
  #ifndef test_ctor_IT 
    noexcept  
  #endif
  {
    #ifdef test_ctor_cIT
      std::cout << "const_Iterator: copy assignment (=)" << std::endl;
    #endif
    this->current = pNode; // equivalent to: (*this).current = pNode
    return *this;  
  } 

  friend bool operator!=(const _const_Iterator& iterator, 
                         const _const_Iterator& iterator2) { 
    return iterator.current != iterator2.current; 
  }
  friend bool operator==(const _const_Iterator& iterator, 
                         const _const_Iterator& iterator2) { 
    return iterator.current == iterator2.current; 
  }
  const value& operator*() const noexcept {
    return current->content;
  }
  const node* get_ptr() const noexcept {
    return current;
  }

  _const_Iterator& operator++(){
    if (is_header()){
      std::cout<<"ERROR: you are trying to go to do ++ on header"<<std::endl;
      exit (EXIT_FAILURE);
     }
    if (get_Rchild().get()) {
      go_right();
      while ((get_Lchild()).get()){
        go_left();
      }
    } 
    else {
      _const_Iterator tmp{get_parent()}; 
      if( (tmp.get_Rchild()).get() ){
        while( get_ptr() == (tmp.get_Rchild()).get() ){  // travel to "Left-parent"
          go_up();
          tmp.go_up();
        }
        go_up(); // travel to the "Right-parent"
      }
      else{ go_up(); } // travel to the "Right-parent"
    }
    return *this;
  } 

  _const_Iterator& operator--(){
    if( is_header() ){ //-- from header
      go_up();         // I'm in root now
      while(!(is_rmost())){
        go_right();
      }
      return *this; // return rightmost
    }
    if ((get_Lchild()).get()) {
      go_left();
      while ((get_Rchild()).get()){
        go_right();
      }
    }
    else{
      _const_Iterator tmp{get_parent()};
      if( (tmp.get_Rchild()).get() ){
        if( get_ptr() == (tmp.get_Rchild()).get() ){
          go_up();
          return *this;
        }
      }
      if ((tmp.get_Lchild()).get()){
        while ( get_ptr() == (tmp.get_Lchild()).get() ){
          go_up();
          tmp.go_up();
          if ( is_header() ){ 
            std::cout<<"ERROR: You are trying to do -- on leftmost node."
                     <<std::endl;
            exit (EXIT_FAILURE);
          }
        }
        go_up();
      }
    }
    return *this;
  }

  _const_Iterator& go_left() noexcept { 
    if (current) { current = current->Lchild.get(); }
    return *this; 
  } 
  _const_Iterator& go_right() noexcept { 
    if (current) { current = current->Rchild.get(); }
    return *this; 
  } 
  _const_Iterator& go_up() noexcept { 
    if (current) { current = current->parent; } 
    return *this; 
  } 

  const std::unique_ptr<node>& get_Lchild() const noexcept {
    return current->Lchild;
  }
  const std::unique_ptr<node>& get_Rchild() const noexcept {
    return current->Rchild;
  }
  const node* get_parent() const noexcept {
    return current->parent;
  }  
  const value* operator->() noexcept {
    return &(*(*this));
  }
};


// #############################################################################
//                                     BST
// #############################################################################
template <class K, class V, class CO=std::less<K>>
class BST {
  using pair=std::pair<const K, V>;
  using node=_node<pair>;

  std::list<pair> remember; 
  std::unique_ptr<node> root;
  node header{pair(), nullptr, nullptr, nullptr};
  CO lesscompare=CO();

 public:
  using Iterator=_Iterator<node,pair>;
  using const_Iterator=_const_Iterator<node,pair>;

  // "= default" means use the compiler default function in a ctor without body.
  BST() noexcept = default;
  BST(BST&& bst) noexcept = default;
  BST& operator=(BST&& bst);
  BST& operator=(const BST& bst);
  
  /* BST custom deep copy constructor: Uses list `remember` of the tree to be 
    copied, and inserts the nodes in the same order in a new tree. */
  BST(const BST& bst){ 
    #ifdef test_ctor
      std::cout<< "BST: deep copy ctor" << std::endl;
    #endif
    std::pair<Iterator,bool> very_useless;
    for ( std::pair<K,V> n : bst.remember ){
      very_useless = insert( n );
    }
  }

  ~BST(){ 
    clear();
    #ifdef test_ctor
      std::cout<< "BST: d_ctor" << std::endl;
    #endif
  }

  Iterator get_root() noexcept { return Iterator(root.get()); }  
  const_Iterator get_root() const noexcept { return const_Iterator(root.get()); }  

  Iterator begin();
  const_Iterator begin() const;
  const_Iterator cbegin() const; 

  Iterator end() { return Iterator{&header}; }
  const_Iterator end() const { return const_Iterator{&header}; }
  const_Iterator cend() const { return const_Iterator{&header}; }

  template <class FT> 
  std::pair<Iterator, bool> insert(FT&& );

  template<class... TS>
  std::pair<Iterator, bool> emplace(TS&&...);

  void clear() 
  #ifndef test_clear
    noexcept 
  #endif
  ;

  template <class FT>
  Iterator find(FT&&);

  template <class FT>
  V& operator[](FT&&);

  template <class FT>
  void erase(FT&&);

  void balance();
  
  void insertion_order();

  // THIS DOESN'T WORK OUT-OF-CLASS, LEFT IT INSIDE
  /* `BST::operator<<`: Takes as an argument a `const BST<>&`. Uses 
     op++ to print a tree. */
  /* "friend" is needed to let "std::ostream" know the private members
      of BST. That way we can use the operator<< on the left-hand side
      of an operation (that is the standard way to print stuff)*/
  template <class OK, class OV, class OCO>
  friend std::ostream& operator<<(std::ostream& os, 
                                  const BST<OK,OV,OCO>& tree) {
    if(!(tree.root)){
      std::cout << "the tree is empty" << std::endl;
    }
    else{
      const_Iterator It{tree.begin()};
      #ifdef test_put_to
        std::cout << "using put_to with operator++() \n";
      #endif
      while ( It != tree.end() ){
        os << (*(It)).first << " ";
        ++It;
      }
/*
      const_Iterator It{tree.end()};  // TEST FOR operator--()  [it works]
      #ifdef test_put_to
        std::cout << " using put_to with operator--() ";
      #endif
      while (It!=tree.begin()){
        --It;
        os << (*(It)).first << " ";
      }
*/
    }
    return os;
  }

}; 

//##############################################################################
//                            BST Member functions
//##############################################################################

template<class K, class V, class CO>
typename BST<K,V,CO>::Iterator BST<K,V,CO>::begin() {
  Iterator tmp{get_root()};
  if(tmp.get_ptr()){ 
    while ((tmp.get_Lchild()).get()){
      tmp.go_left();
    }
    return tmp;
  }
  return tmp;
}
template<class K, class V, class CO>
typename BST<K,V,CO>::const_Iterator BST<K,V,CO>::begin() const {
  const_Iterator tmp{get_root()};
  if(tmp.get_ptr()){ 
    while ((tmp.get_Lchild()).get()){
      tmp.go_left();
    }
    return tmp;
  }
  return tmp;
}
template<class K, class V, class CO>
typename BST<K,V,CO>::const_Iterator BST<K,V,CO>::cbegin() const {
  const_Iterator tmp{get_root()};
  if(tmp.get_ptr()){ 
    while ((tmp.get_Lchild()).get()){
      tmp.go_left();
    }
    return tmp;
  }
  return tmp;
}

/* BST::insert
    Takes an `std::pair<const key_type, value_type>` as input argument. Travels
    inside the tree via Iterators and uses ``std::make_unique<node>` to insert 
    a new node at the right place. */
template <class K, class V, class CO>
template <class FT> 
std::pair<typename BST<K,V,CO>::Iterator, bool> BST<K,V,CO>::insert(FT&& v) {
  std::pair<Iterator,bool> ret;
  Iterator It{get_root()};
  #ifdef test_insert
    K c1;
    V c2;
  #endif

  if (!root) {
    root = std::make_unique<node>(std::forward<FT>(v), nullptr, &header, 
                                  &header);
    header.parent = root.get();

    remember.push_back(v);
    It = get_root();
    ret.first = It;
    ret.second = true;
    #ifdef test_insert
      c1=(*It).first;
      c2=(*It).second;
      std::cout << "Insert Root. (" << c1  << ","<< c2 
                << ") ***************" << std::endl;
    #endif
    return ret;
  }
  while (true) {
    if ( lesscompare( v.first,(*It).first ) ) { 
      if ( !((It.get_Lchild()).get()) ) { 
        It.get_Lchild() = std::make_unique<node>(std::forward<FT>(v), 
                                                  nullptr, 
                                                  nullptr, 
                                                  It.get_ptr()); 
        remember.push_back(std::forward<FT>(v));
        It.go_left();
        #ifdef test_insert
          c1=(*It).first;
          c2=(*It).second;
          std::cout << "Insert left child. (" << c1  << "," << c2 
                    << ") ***************" << std::endl;
        #endif
        ret.first=It;
        ret.second=true;
        return ret;
      }
      It.go_left();
      #ifdef test_insert
        std::cout << "Go to the left." << std::endl;
      #endif
    }
    else if ( lesscompare( (*It).first,v.first ) ) { 
      if( It.get_ptr() == --end() ){
        (It.get_Rchild()).release();
        It.get_Rchild() = std::make_unique<node>(std::forward<FT>(v), nullptr, 
                                                  &header, It.get_ptr());
        remember.push_back(std::forward<FT>(v));
        It.go_right();
        #ifdef test_insert
          c1 = (*It).first;
          c2 = (*It).second;
          std::cout << "Insert right child. [RMOST] (" << c1  << ","<< c2 
                    << ") ***************" << std::endl;
        #endif
        ret.first = It;
        ret.second = true;
        return ret;
      }
      else if ( !((It.get_Rchild()).get()) ) { 
        It.get_Rchild() = std::make_unique<node>(std::forward<FT>(v), nullptr, 
                                                  nullptr, It.get_ptr());
        remember.push_back(std::forward<FT>(v));
        It.go_right();
        #ifdef test_insert
          c1 = (*It).first;
          c2 = (*It).second;
          std::cout << "Insert right child. (" << c1  << ","<< c2 
                    << ") ***************" << std::endl;
        #endif
        ret.first = It;
        ret.second = true;
        return ret;
      }
      It.go_right();
      #ifdef test_insert
        std::cout << "Go to the right." << std::endl;
      #endif
    } 
    else{ 
      #ifdef test_insert
        c1 = (*It).first;
        std::cout << "Key (" << c1 
                  << ") already present in the BST. It will be not inserted."  
                  << std::endl;
      #endif
      ret.first = It;
      ret.second = false;
      return ret;
    } 
  }
}

/* `emplace`: Takes key and value separately, constructs a pair from them and 
    inserts it through `insert()`. The following four functions cover all four
    l-/r-value combinations for the key and value arguments. */
template <class K, class V, class CO>
template<class... TS>
std::pair<typename BST<K,V,CO>::Iterator, bool> BST<K,V,CO>::emplace(TS&&... args) {   
  #ifdef test_emplace   
    std::cout << "Templated emplace ->  \n";
  #endif
  std::pair<Iterator,bool> ret{insert(std::pair<K,V>(std::forward<TS>(args)...))};
  return ret;
}

template <class K, class V, class CO>
void BST<K,V,CO>::clear() 
  #ifndef test_clear
    noexcept 
  #endif
{ 
  if (root){
    Iterator tmp{end()};
    --tmp;                        // Travel to rmost.
    (tmp.get_Rchild()).release(); // Release Rchild of rmost because header 
                                  // dtor is managed by ~node().
    root.reset();
    remember.clear();
    #ifdef test_clear
      std::cout << "Tree cleared!\n";
    #endif
  }
  else{
    #ifdef test_clear
      std::cout << "The tree is already clear\n";
    #endif
  }
}

/* `BST::find`: Takes as input the key. Travels the tree like `insert()`. 
    Once the Iterator encounters a node with the corresponding key, it returns
    itself (the Iterator). */
template <class K, class V, class CO>
template <class FT>
typename BST<K,V,CO>::Iterator BST<K,V,CO>::find(FT&& k) {
  Iterator It{get_root()};
  if (!root) {
    #ifdef test_find
      std::cout<<"the tree is empty"<<std::endl;
    #endif
    return It;
  }
  if(std::forward<FT>(k)==(*It).first){
    #ifdef test_find
      std::cout << "you find the root (" << k << ","
                << (*It).second << ")" << std::endl;
    #endif
    return It;
  }
  while (true){
    if(lesscompare( std::forward<FT>(k),(*It).first )){
      if ( (It.get_Lchild()).get() ){ 
        It.go_left();
        if( std::forward<FT>(k)==(*It).first ){
          #ifdef test_find
            std::cout << "you find the pair (" << k << "," 
                      << (*It).second << ")" << std::endl;
          #endif
          return It;
        }
      }
      else{ 
          std::cout << "the key (" << k 
                    << ") is absent, so end() is returned" << std::endl;
        return end();
      }
    }
    if(lesscompare( (*It).first, std::forward<FT>(k))){
      if ( ((It.get_Rchild()).get()) ){ 
        It.go_right();
        Iterator Pitt{It.get_parent()};
        Iterator Gpitt{Pitt.get_parent()};
        // Exit if you arrive in header (e.g. searching for 16 in a tree with rmost=14).
        if( It == Gpitt && !(It.get_Rchild()) ){ 
          std::cout << "the key (" << k 
                    << ") is absent, so end() is returned" << std::endl;
          return It; 
          } 
        if( std::forward<FT>(k)==(*It).first){
            std::cout << "you find the pair (" << k << "," 
                      << (*It).second << ")" << std::endl;
          return It;
        }
      }
      else{ 
        std::cout << "the key (" << k 
                  << ") is absent, so end() is returned" << std::endl;
        return end();
      }
    }
  }
}

/* `BST::operator[]`: Takes a key as an argument, locates the corresponding 
  node using `find()`, and returns the associated value. */
template <class K, class V, class CO>
template <class FT>
V& BST<K,V,CO>::operator[](FT&& k) {
  Iterator It{find(std::forward<FT>(k))};
  #ifdef test_square_bra
    std::cout << "operator["<< std::forward<FT>(k) << "] " << std::endl; 
  #endif
  if (It != end()){ return (*It).second; }
  else{ 
    #ifdef test_square_bra
    std::cout << "this key is not present in the tree, a segmentation "
              << "fault is triggered." << std::endl; 
    #endif
    return (*It).second; 
  }
}


/* `BST::erase()`: Takes as an argument a key. First removes the 
    corresponding pair from `remember` to reconstruct the entire tree from 
    this modified list. */
/* `BST::erase`: lvalue overload */
template <class K, class V, class CO>
template <class FT>
void BST<K,V,CO>::erase(FT&& k){
  bool found_key{false};
  for(std::pair<K,V> n : remember){
    if(n.first==std::forward<FT>(k)){
      found_key=true;
      break;
    }
  }
  if(found_key){
    Iterator It{find(std::forward<FT>(k))};
    remember.remove(*It);
    std::list<pair> tmp_list;
    for(pair n : remember){
      tmp_list.push_back(n);
    }
    clear();
    std::pair<Iterator,bool> very_useless_2;
    for ( std::pair<const K,V> n : tmp_list ){
      very_useless_2 = insert( n );
    }
    #ifdef test_erase
      std::cout<<"erased "<<k<<" from tree."<<std::endl;
    #endif
  }
  else{
      std::cout<<"The key "<<k<<" is absent. Don't erase. "<<std::endl;
  }
}

/* `balance()`: Uses the external function 
  `sorted_vector_for_balance_recursively()` to construct an 
  `std::vector<std::pair>`, corresponding to the nodes in the original tree, 
  but ordered in a specific way. The pairs are then inserted in the cleared 
  tree, according to the order in the vector, to construct a balanced tree. */
template <class K, class V, class CO>
void BST<K,V,CO>::balance() {
  auto tmp_list{remember};
  tmp_list.sort();
  std::vector<std::pair<K,V>> tmp_vec{}, final_vec{};
  for (auto n : tmp_list) {
    tmp_vec.push_back(n);
  }
  sorted_vector_for_balance_recursively(final_vec, tmp_vec, 0, 
                                        tmp_vec.size());
  clear();
  std::pair<Iterator,bool> very_useless_3;
  for ( auto n : final_vec ){
    very_useless_3 = insert( n );
  }
  #ifdef test_balance
    std::cout << "--- Balancing done! ---" << std::endl;
  #endif
}

/* `insertion_order()`: Auxiliary function. Prints the keys of the pairs 
  inside `remember`. */
template <class K, class V, class CO>
void BST<K,V,CO>::insertion_order() {
  std::cout << "insertion order (remember): ";
  for (std::pair<K,V> n : remember) {
    std::cout << n.first << " ";
  }
  std::cout<<std::endl;
}

//##############################################################################
//                                  MAIN                                                        
//##############################################################################
int main() {

  BST<int,int> oak{};
  std::cout<<"\ntest insert() and emplace():\n";
  std::pair<BST<int,int>::Iterator,bool> retu{oak.insert(std::pair<int,int>(8,88))};

  std::cout<<"\ntest correct assignment of header and root members:\n";
  std::cout<<"content of root, expect 8: "<< (retu.first)->first <<std::endl;
  std::cout<<"Rchild of root, expect 0: "
           << ( (*( (retu.first).get_Rchild() )).content).first<<std::endl;
  std::cout<<"parent of root, expect 0: "
           << ( (*( (retu.first).get_parent() )).content).first<<std::endl;
  ++(retu.first);
  std::cout<<"content of header, expect 0: "<< (retu.first)->first <<std::endl;
  std::cout<<"parent of header, expect 8: "
           << ( (*( (retu.first).get_parent() )).content).first<<std::endl;
  //++(retu.first);     

  std::pair<const int, int> pair1(3,33);
  std::pair<BST<int,int>::Iterator,bool> retu2{oak.insert(pair1)};
  std::pair<BST<int,int>::Iterator,bool> retu3{oak.emplace(10,1010)};
  std::pair<BST<int,int>::Iterator,bool> retu4{oak.emplace(1,11)};
  std::pair<BST<int,int>::Iterator,bool> retu5{oak.emplace(5,55)};
  std::pair<BST<int,int>::Iterator,bool> retu6{oak.emplace(14,1414)};
  // Test the four ways to use `BST::emplace`. 
  int sev=7, sevsev=77, onth=13, seisei=66; 
  std::pair<BST<int,int>::Iterator,bool> retu7{oak.emplace(4,44)};
  std::pair<BST<int,int>::Iterator,bool> retu8{oak.emplace(sev,sevsev)};
  std::pair<BST<int,int>::Iterator,bool> retu9{oak.emplace(onth,1313)};
  std::pair<BST<int,int>::Iterator,bool> retu10{oak.emplace(6,seisei)};

  std::cout << "\ntest by ALberto: using 'std::vector' and 'std::copy':\n";
  std::vector<std::pair<int,int>> vecoak;
  vecoak.resize(10);
  std::copy(oak.begin(), oak.end(), vecoak.begin());
  std::cout<<"vecoak after 'std::copy':"<<std::endl;
  for (int i=0; i<10; i++){
    std::cout << (vecoak[i]).first << " ";
  }
  std::cout << std::endl;

  // explicit implementation of std::copy
  auto vecoak_iter{vecoak.begin()};
  std::cout<<"vecoak during manual for-loop:"<<std::endl;
  for (auto oak_iter = oak.begin(); oak_iter!=oak.end(); ++oak_iter) {
    *vecoak_iter = *oak_iter;
    std::cout << (*(vecoak_iter)).first << " ";
    ++vecoak_iter;
  }
  std::cout << std::endl;
  
  std::cout << "\ntest operator->() of iterator:" << std::endl;
  std::cout << "Let's see: "<< (*(retu6.first)).second << " = " 
            << (retu6.first)->second << std::endl; 
  std::cout << "Let's see: "<< (*(retu10.first)).first << " = " 
            << (retu10.first)->first << std::endl; 
  std::cout << "Let's see: "<< (*(retu7.first)).second << " = " 
            << (retu7.first)->second << std::endl; 

  std::cout << "\ntest to print begin and end:\nLEFT MOST NODE KEY : " 
            << (*(oak.begin())).first << std::endl;
  std::cout << "RIGHT MOST NODE KEY : " << (*(--oak.end())).first 
            << std::endl;

  std::cout << "\ntest find():\n";
  BST<int,int>::Iterator nemo1{oak.find(8)};
  std::cout << "let's find key 8, the value for him is: " 
            << (*nemo1).second << std::endl;
  BST<int,int>::Iterator nemo2{oak.find(14)};
  std::cout << "let's find key 14, the value for him is: " 
            << (*nemo2).second << std::endl;
  BST<int,int>::Iterator nemo3{oak.find(1)};
  std::cout << "let's find key 1, the value for him is: " 
            << (*nemo3).second << std::endl;
  int a=4, bbb=9;
  BST<int,int>::Iterator nemo4{oak.find(bbb)};
  std::cout << "let's find key "<<bbb<<", the value for him is: " 
            << (*nemo4).second<< std::endl;
  BST<int,int>::Iterator nemo5{oak.find(a)};
  std::cout << "let's find key "<<a<<", the value for him is: " 
            << (*nemo5).second<< std::endl;

  std::cout << "\ntest operator[]:\n";
  int apple{oak[8]};
  std::cout << "the value is: " << apple << std::endl;
  int b=14;
  int pear{oak[b]};
  std::cout << "the value is: " << pear << std::endl;
  
  std::cout << "\ntest operator<<:\nour tree: ";
  std::cout << oak << std::endl;

  std::cout << "\ntest the deep copy for the tree: \n";
  BST<int,int> elm{oak};
  std::cout << "oak = " << oak << std::endl;
  std::cout << "elm = " << elm << std::endl;
  std::pair<BST<int,int>::Iterator,bool> new_retu{elm.emplace(15,1515)};
  std::cout << "oak (after inserting 15 in elm)= " << oak << std::endl;
  std::cout << "elm (after inserting 15 in elm)= " << elm << std::endl;

  std::cout << "\ntest clear() on oak: " << std::endl;
  oak.insertion_order();
  oak.clear();
  std::cout << "print oak after .clear() : " << oak << std::endl;
  std::cout << "oak: test to print root raw pointer after the clear: "
            << "\nwith get_root():  " << (oak.get_root()).get_ptr() << std::endl;
  std::cout << "with begin():  " << (oak.begin()).get_ptr() << std::endl; 
  std::cout << "print oak after clear: " << oak;
  oak.insertion_order(); 

  std::cout << "\ntest erase:\n";
  elm.erase(15); // Easy case. 
  std::cout << "elm (after erasing 15 in elm)= " << elm << std::endl;
  std::pair<BST<int,int>::Iterator,bool> new_retu15{elm.emplace(15,1515)};

  elm.erase(7);
  std::cout << "elm (after erasing 7 in elm)= " << elm << std::endl;
  std::pair<BST<int,int>::Iterator,bool> new_retu7{elm.emplace(7,77)};

  elm.erase(10);
  std::cout << "elm (after erasing 10 in elm)= " << elm << std::endl;
  std::pair<BST<int,int>::Iterator,bool> new_retu10{elm.emplace(10,1010)};

  elm.insertion_order();
  elm.erase(5);
  elm.insertion_order();
  std::cout << "elm (after erasing 5 in elm)= " << elm << std::endl;
  
  BST<int,int> new_oak{};
  std::pair<BST<int,int>::Iterator,bool> new_retu8{new_oak.emplace(8,88)};
  std::pair<BST<int,int>::Iterator,bool> new_retu3{new_oak.emplace(3,33)};
  new_oak.erase(8); // Non-trivial case: deleting the root.
  std::cout << "new_oak (after erasing 8 (root) in new_oak)= " << new_oak 
            << std::endl; // print the root ALONE (special case for ++)

  std::cout << "new_oak (I try to erase 7, that is not present in the tree): " 
            << new_oak << std::endl;
  new_oak.erase(7); // Case with a node that is absent in the tree. 

  std::pair<BST<int,int>::Iterator,bool> new_retu1{new_oak.emplace(1,11)};
  std::pair<BST<int,int>::Iterator,bool> new_retu2{new_oak.emplace(2,22)};
  new_oak.erase(1); // Non-trivial case.
  std::cout << "new_oak (after erasing 1 in new_oak)= " << new_oak << std::endl; 
  
  int ccc=2, ddd=3, aaa=4; // Test the lvalue for erase. 
  new_oak.erase(ccc);
  std::pair<BST<int,int>::Iterator,bool> new_retu4{new_oak.emplace(4,44)};
  new_oak.erase(ddd); 
  std::cout << "new_oak (after erasing 3 in new_oak)= " << new_oak << std::endl; 
  new_oak.erase(aaa);
  std::cout << "new_oak (after erasing 4 (the root alone) in new_oak)= " 
            << new_oak << std::endl; 


  std::cout << "test balance() on a new tree (birch): " << std::endl;
  BST<int,int> birch{};
  std::pair<BST<int,int>::Iterator,bool> pretu{birch.emplace(1,88)};
  std::pair<BST<int,int>::Iterator,bool> pretu2{birch.emplace(7,33)};
  std::pair<BST<int,int>::Iterator,bool> pretu3{birch.emplace(11,1010)};
  std::pair<BST<int,int>::Iterator,bool> pretu4{birch.emplace(17,11)};
  std::pair<BST<int,int>::Iterator,bool> pretu5{birch.emplace(21,55)};
  std::pair<BST<int,int>::Iterator,bool> pretu6{birch.emplace(29,1414)};
  std::pair<BST<int,int>::Iterator,bool> pretu7{birch.emplace(40,44)};
  std::pair<BST<int,int>::Iterator,bool> pretu8{birch.emplace(60,77)};
  std::pair<BST<int,int>::Iterator,bool> pretu9{birch.emplace(5,1313)};
  std::pair<BST<int,int>::Iterator,bool> pretu10{birch.emplace(15,6660)};
  std::pair<BST<int,int>::Iterator,bool> pretu11{birch.emplace(25,146614)};
  std::pair<BST<int,int>::Iterator,bool> pretu12{birch.emplace(50,44)};
  std::pair<BST<int,int>::Iterator,bool> pretu13{birch.emplace(10,776)};
  std::pair<BST<int,int>::Iterator,bool> pretu14{birch.emplace(30,16313)};
  std::pair<BST<int,int>::Iterator,bool> pretu15{birch.emplace(20,666)}; 
  birch.insertion_order();
  birch.balance();
  birch.insertion_order();
  std::cout << "birch after balance= " << birch << std::endl;

  std::cout << "\ntest to see if I can change a value inside the pair of a node."
            << std::endl;
  std::cout << "before changing the value I have the node: (" 
            << (*(pretu.first)).first << "," 
            << (*(pretu.first)).second << ")" << std::endl;
  (*(pretu.first)).second = 182;
  std::cout << "before changing the value I have the node: (" 
            << (*(pretu.first)).first << "," 
            << (*(pretu.first)).second << ")" << std::endl;
  std::cout << "YES! I can :D" << std::endl;

  std::cout<<"\nTest operator= of iterator:" << std::endl;
  BST<int,int>::Iterator names;
  names=pretu.first;
  pretu.first=pretu2.first;
  std::cout<< "names key =" << (*names).first << std::endl;
  std::cout<< "pretu.first key =" << (*(pretu.first)).first << std::endl;
  std::cout<< "pretu1.first key =" << (*(pretu2.first)).first << std::endl;

  std::cout << "\ntest operator== of iterator:" << std::endl;
  std::cout << "I expect a false: " << (pretu10.first==pretu11.first) << std::endl;
  std::cout << "I expect a true: " << (pretu10.first!=pretu11.first) << std::endl;
  std::cout << "I expect a false: " << (pretu11.first!=pretu11.first) << std::endl;
  std::cout << "I expect a true: " << (pretu11.first==pretu11.first) << std::endl;


  std::cout<<"\nTHE END\n\n"; 
  return 0;
}
