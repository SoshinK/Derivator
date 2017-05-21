//! Derivator v1.0. SoshinK 2017 MIPT

#define NDBUG

#include <cstdio>
#include <cstring>
#include <cassert>
#include <cstdlib>

#include "Tree.h"
#include "Tree.cpp"

#include "ExprParser.h"
#include "ExprParser.cpp"


enum CONSTANTS
  {
  DOUBLE_DIGITS = 32,
  EXPR_SIZE = 1024,
  };

#define FORMATLINE "%1023s"

enum OPERATORS
  {
  #define OPERATORS
  #define OPERATOR(sign, name, code) \
    OPERATOR_##name,
  #include "Operators.txt"
  #undef OPERATOR
  #undef OPERATORS
    OPERATOR_NULL,
  };


class Derivator
  {
  public:
    Derivator();
    explicit Derivator(Tree & source);
    ~Derivator();

    void dump();
    void prepareExp();                        //! f(x)^g(x) -> e^(g(x)*ln(f(x)))
    void simplifyer();                        //! simplify expressions(watch below)
    void TeX(const char* filename);           //! put derivative in TeX-file
    bool derivative();                        //! make a deriavtive

    bool dprint();                            //! print a derivative

  private:
    Node* prepexp(Node* cur);
    Node* simplify(Node* cur);
    OPERATORS getoper(char* oper);
    Node* d(Node* cur);
    void dprintNode(Node* cur);    
    void dprintTeX(FILE* file, Node* cur); 

    bool IsSimplifyable;

    Tree Func_;
    Tree Derivative_;
  };

Derivator::Derivator():
  IsSimplifyable(true),
  Func_(),
  Derivative_()
  {
  FILE* func = fopen("Function.txt", "rt");
  if(!func)
    {
    printf("Derivator:ERROR: file 'Function.txt' doesnt exist\n");
    exit(1);
    }
  fclose(func);
  Func_.fscan("Function.txt");
  }

Derivator::Derivator(Tree & source):
  IsSimplifyable(true),
  Func_(source)
  {}

Derivator::~Derivator() {}

void Derivator::dump()
  {
  printf("Derivator:dump [%p]\n", this);
  printf("Tree Func_ = %p\n", &Func_);
  printf("Tree Derivative_ = %p\n", &Derivative_);
  printf("df / dx =  ");
  if(!dprint())printf("Not exist\n");
  printf("\n");
  Func_.dump();
  Func_.dotdump("Function.gv");
  system("xdot Function.gv");
  }

OPERATORS Derivator::getoper(char* oper)
  {
  #define OPERATORS
  #define OPERATOR(sign,name,code); \
    if(!strcmp(#sign, oper + 2) && *oper == 'o') \
      return OPERATOR_##name;
  #include "Operators.txt"
  #undef OPERATOR
  #undef OPERSTORS
  return OPERATOR_NULL;
  }


Node* Derivator::prepexp(Node* cur)
  {
  if(!strcmp(cur->data(), "o:^") && (*(cur->right()->data()) == 'o'
                                   || *(cur->right()->data()) == 'v'))
    {
    Node* exp = new Node;
    Node* mul = new Node;
    Node* ln = new Node;
    ln->setdata("o:ln");
    ln->setright(prepexp(cur->left()));
    mul->setdata("o:*");
    mul->setleft(prepexp(cur->right()));
    mul->setright(ln);
    exp->setdata("o:exp");
    exp->setright(mul);
    return exp;
    }
  Node* e = new Node;
  e->setdata(cur->data());
  if(cur->left())e->setleft(prepexp(cur->left()));
  if(cur->right())e->setright(prepexp(cur->right()));
  return e;
  }

void Derivator::prepareExp()
  {
  Node* func = prepexp(Func_.Root());
  Tree NewTree(func);
  Func_ = NewTree;
  }

Node* Derivator::d(Node* cur)
  {
  if(!cur)
    {
    printf("Derivator:d:cur ptr not exist\n");
    assert(0);
    }
  char* data = cur->data();
  if(*(data) == 'c')
    {
    Node* newnode = new Node;
    newnode->setdata("c:0");
    return newnode;
    }
  if(*(data) == 'v')
    {
    Node* newnode = new Node;
    newnode->setdata("c:1");
    return newnode;
    }
  else
    {
    int oper = getoper(data);
    switch(oper)
      {
      #define OPERATORS
      #define OPERATOR(sign, name, code); \
        case OPERATOR_##name: \
          code \
          break;
      #include "Operators.txt"
      #undef OPERATOR 
      #undef OPERATORS
      default:
        printf("Derivator:d:ERROR:bad operator '%s'\n", data + 2);
        assert(0);
      }
    }
  return NULL;
  }

bool Derivator::derivative()
  {
  Node* der = d(Func_.Root());
  Tree tree(der);
  Derivative_ = tree;
  return true;
  }

void Derivator::dprintNode(Node * cur)
  {
  if(*(cur->data()) == 'o' && cur != Derivative_.Root())printf("(");
  if(cur->left())dprintNode(cur->left());
  printf("%s", cur->data() + 2);
  if(cur->right())dprintNode(cur->right());
  if(*(cur->data()) == 'o' && cur != Derivative_.Root())printf(")");
  }


bool Derivator::dprint()
  {
  if(Derivative_.Root()->data())
    {
    dprintNode(Derivative_.Root());
    return true;
    }
  else return false;
  }

void Derivator::dprintTeX(FILE* file, Node* cur)
  {
  assert(cur);
  if(*(cur->data()) == 'o')fprintf(file, "\\left(");
  if(!strcmp(cur->data(), "o:/"))                       // /
    {
    fprintf(file, "\\frac");
    fprintf(file, "{");
    dprintTeX(file, cur->left());
    fprintf(file, "}{");
    dprintTeX(file, cur->right());
    fprintf(file, "}");
    }
  if((*(cur->data()) == 'o') && (isalpha(*(cur->data() + 2)))) // sin, cos, ln etc
    {
    fprintf(file, "%s", cur->data() + 2);
    fprintf(file, "{");
    dprintTeX(file, cur->right());
    fprintf(file, "}");
    }
  if(strcmp(cur->data(), "o:/") && (*(cur->data()) == 'o')      // +,-,*
                                   && (!isalpha(*(cur->data() + 2))))
    {
    fprintf(file, "{");
    dprintTeX(file, cur->left());
    fprintf(file, "}%s{", cur->data() + 2);
    dprintTeX(file, cur->right());
    fprintf(file, "}");
    }
  if(*(cur->data()) == 'v' || *(cur->data()) == 'c')fprintf(file, "%s", cur->data() + 2);

  if(*(cur->data()) == 'o')fprintf(file, "\\right)");
  }

void Derivator::TeX(const char* filename)
  {
  if(Derivative_.Root() == NULL)
    {
    printf("ERROR: derivative is lost\n");
    exit(1);
    }
  FILE* file = fopen(filename, "wt");
  if(!file)
    {
    printf("ERROR:Can't open output file\n");
    exit(1);
    }
  fprintf(file, "\\documentclass{article}\n\
\\usepackage[a4paper, margin=6mm]{geometry}\n\
\\usepackage{amsmath}\n\
\\usepackage[utf8]{inputenc}\n\
\\usepackage[T2A]{fontenc}\n\
\\usepackage[russian]{babel}\n\
\\usepackage{hyperref}\n\
\\begin{document}\n\n\
\\fontsize{8}{8pt}\\selectfont\n\
\\begin{equation}\n");
  fprintf(file, "\\frac{d}{dx}");
  dprintTeX(file, Func_.Root());
  fprintf(file, " = ");
  dprintTeX(file, Derivative_.Root());
  fprintf(file, "\n\\end{equation}\n\\end{document}\n");
  fclose(file);
  }


#define DEFAULT \
 Node* copy = new Node; \
 copy->setdata(cur->data()); \
 copy->setleft(simplify(cur->left())); \
 copy->setright(simplify(cur->right())); \
 return copy;



Node* Derivator::simplify(Node* cur)
  {
  if(!cur)return NULL;
  if(cur->left() && cur->right())
    {
    double l = atof(cur->left()->data() + 2);
    double r = atof(cur->right()->data() + 2);


//! Simplifying #1: a +/-/* b where a, b are numbers
    if(*(cur->left()->data()) =='c' && *(cur->right()->data()) == 'c')
      {
      OPERATORS oper = getoper(cur->data());
      switch(oper)
        {
        case OPERATOR_ADD:
          l += r;
          break;
        case OPERATOR_SUB:
          l -= r;
          break;
        case OPERATOR_MUL:
          l *= r;
          break;
        default:
          break;
        }
      Node* newnode = new Node;
      char strexp[DOUBLE_DIGITS];
      char str[DOUBLE_DIGITS + 2];                                   //CHECK!
      sprintf(strexp, "%f", l);                               
      strcpy(str, "c:");
      strcat(str, strexp);
      newnode->setdata(str);
      IsSimplifyable = true;
      return newnode;
      }

//! Simplifying #2: f(x) * 0 -> 0
    else if((!strcmp(cur->data(), "o:*") && (*(cur->left()->data()) =='c' || *(cur->right()->data()) == 'c')) &&
       ((l == 0 && *(cur->left()->data()) == 'c') || (r == 0 && *(cur->right()->data()) == 'c')))
      {
        Node* newnode = new Node;
        newnode->setdata("c:0");
        IsSimplifyable = true;
        return newnode;
      }
      
//! Simplifying #3: f(x) + 0 -> f(x)
    else if(!strcmp(cur->data(), "o:+") && 
            ((*(cur->left()->data()) == 'c' && l == 0 ) || (*(cur->right()->data()) == 'c' && r == 0)))
      {
      IsSimplifyable = true;
      if(*(cur->left()->data()) == 'c' && l == 0)
        {
        Node* newnode = nodecopy(cur->right());
        return newnode;
        }
      else
        {
        Node* newnode = nodecopy(cur->left());
        return newnode;
        }
      }
//! Simplifying #4: f(x) - 0 -> f(x)
    else if(!strcmp(cur->data(), "o:-") && *(cur->right()->data()) == 'c' && r == 0 )
      {
      IsSimplifyable = true;
      Node* newnode = nodecopy(cur->left());
      return newnode;
      }
//! Simplifying #5: f(x) * 1 -> f(x)
    else if(!strcmp(cur->data(), "o:*") && 
            ((*(cur->left()->data()) == 'c' && l == 1) || (*(cur->right()->data()) == 'c' && r == 1) ))
      {
      IsSimplifyable = true;
      if(*(cur->left()->data()) == 'c' && l == 1)
        {
        Node* newnode = nodecopy(cur->right());
        return newnode;
        }
      else
        {
        Node* newnode = nodecopy(cur->left());
        return newnode;
        }
      }
//! Simplifying #6: f(x) ^ 1 -> f(x)
    else if(!strcmp(cur->data(), "o:^") && 
            (*(cur->right()->data()) == 'c' && r == 1))
      {
      IsSimplifyable = true;
      Node* newnode = nodecopy(cur->left());
      return newnode;
      }
//! ------------------------------------
    else
      {
      DEFAULT;
      }
    }
  else
    {
    Node* newnode = new Node;
    newnode->setdata(cur->data());
    newnode->setleft(nodecopy(cur->left()));
    newnode->setright(nodecopy(cur->right()));
    return newnode;
    }
  }
#undef DEFAULT


void Derivator::simplifyer()
  {
  Node* newroot;
  while(IsSimplifyable)
    {
    IsSimplifyable = false;
    newroot = simplify(Derivative_.Root());
    Tree newt(newroot);
    Derivative_ = newt;
    }
  }

int main(int argc, char* argv[])
  {
  printf("===========================================\n");
  printf("Derivator v1.0 SoshinK 2017 MIPT\n");
  printf("f(x)=");
  char Expression[EXPR_SIZE];
  scanf(FORMATLINE, Expression);
  printf("===========================================\n");
  printf("d/dx [%s] = ", Expression);
  
  Parser parser;
  Tree expr(parser(Expression));

  Derivator d(expr);
  d.prepareExp();
  d.derivative();
  d.simplifyer();
  d.dprint();
  
  if(argc == 3 && !strcmp(argv[1], "-tex"))
    {
    d.TeX(argv[2]);
    printf("\n\nDerivative has been successfully written in %s\n", argv[1]);
    }
  else if(argc == 1)
    {
    printf("\n\nNote: you can call: ./d -tex <filename> to write derivative into TeX-file\n");
    }
  else
    {
    printf("\n\nUnknown call for Derivator\n");
    printf("Note: you can call: ./d -tex <filename> to write derivative into TeX-file\n");
    }
  return 0;
  }

#undef FORMATLINE

