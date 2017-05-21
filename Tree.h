#ifndef TREE_H
#define TREE_H

#include <cstdio>
#include <cstring>
#include <cassert>
#include <cstdlib>


//!~~~~NODE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class Node
  {
  public:
    Node();
    explicit Node(const char* data);
    ~Node();

    char* data();                                 //! returns data
    Node* right();                                //! returns right leaf
    Node* left();                                 //! ....
    Node* parent();                               //! returns parent
    
    void setright(Node* right);                   //! change/set leaves
    void setparent(Node* parent);   
    void setleft(Node* left);
    void setdata(const char* data);               //! change/set data   

    void kill(FILE*);                             //! calls delete for leaves(used by visitor in
                                                  //! Tree::~Tree())

    void dotprint(FILE* out);                     //! put in *.gv file node descrition
                                                  //! (used by visitor in Tree::dotdump())
 
    void nodeprint(FILE* out);                    //! writes in file current node

    void dump(FILE*);                             //! dumping current node(used by visitor in
                                                  //! Tree::dump())
  
    void fnodescan(FILE*);                        //! reads from file current node

  private:
    Node* Parent_;
    Node* Left_;
    Node* Right_;
    char* Data_;
  };



Node* nodecopy(Node* source);                     //! returns pointer to copy of source
                                                  //! (tree under source)

//!~~~~/NODE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//!~~~~TREE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

enum TREE_CONSTANTS
  {
  DEFAULT_DATA_SIZE = 32,
  };


enum TRAVERSAL                                    //! taversal modes
  {
  PRE_ORDER,                                      //! data - left leaf - right leaf
  IN_ORDER,                                       //! leaft leaf - data - right leaf
  POST_ORDER,                                     //! leaft leaf - right leaf - data
  };


class Tree
  {
  public:
    Tree();
    Tree(Node* root);
    Tree(Tree & that);
    
    Tree& operator = (Tree& that); 

    explicit Tree(const char* filename);
    ~Tree();
    Node* Root();                                 //! returns root
    Node* getcursor();                            //! returns current cursor position
    bool mcleft();                                //! moves cursor to the left leaf
    bool mcright();                              
    bool mcabove();                               //! moves to parent
    bool mcroot();
    bool mcnode(Node* dest);
    void dump();
    bool dotdump(const char* filename);           //! put commands in *.gv file for Dotter

    void addl(Node* newnode);                     //! add new left leaf(old one 
                                                  //! wont be deleted)
     
    void addr(Node* newnode);

    bool fscan(const char* filename);             //! reads tree from the file

    bool fprint(const char* filename);            //! writes tree to the file
    
    void visitor/*your mum*/(TRAVERSAL mode,      //! visits every node according to
         Node* curnode, void (Node::*func)(FILE*),//! traversal mode
         FILE* file);
    
    Node* search(const char* data);               //! search node with specified data,
                                                  //! put cursor on it and returns its address

  private:
    Node* Root_;                                  //! Root
    Node* Cursor_;                                //! Cursor (thx, Cap)
  };

char* getstr(FILE* in);

//void treePrint(Tree tree);

//!~~~~/TREE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif

