#ifndef EXPRPARSER_CPP
#define EXPRPARSER_CPP

#include <cstdio>
#include <cctype>
#include <cstring>
#include <cmath>
#include <cstdlib>

#include "Tree.h"
#include "Tree.cpp"

#include "ExprParser.h"

Parser::Parser():
  Expression_(NULL),
  ExIndex_(0)
  {}

Parser::~Parser()
  {
  if(!Expression_)delete [] Expression_;
  ExIndex_ = 0;
  }

Node* Parser::operator () (char* expression)
  {
  size_t size = strlen(expression);
  Expression_ = new char [size + 1];
  strcpy(Expression_, expression);
  Node* res = G();
  delete [] Expression_;
  return res;
  }

Node* Parser::G()
  {
  Node* res = E();
  if(Expression_[ExIndex_] != '\0')
    {
    printf("ERROR:Can't interpret the input\n");
    exit(1);
    }
  return res;
  }

Node* Parser::E()
  {
  Node* val = T();
  Node* prev = val;
  while(Expression_[ExIndex_] == '+' || Expression_[ExIndex_] == '-')
    {
    char oper = Expression_[ExIndex_];
    ExIndex_++;
    Node* val2 = T();
    Node* res = new Node;
    res->setleft(prev);
    res->setright(val2);
    if(oper == '+')res->setdata("o:+");
    if(oper == '-')res->setdata("o:-");
    prev = res;
    }
  return prev;
  }


Node* Parser::T()
  {
  Node* val = P();
  Node* prev = val;
  while(Expression_[ExIndex_] == '*' || Expression_[ExIndex_] == '/' 
                                          || Expression_[ExIndex_] == '^')
    {
    char oper = Expression_[ExIndex_];
    ExIndex_++;
    Node* val2 = P();
    Node* res = new Node;
    res->setleft(prev);
    res->setright(val2);
    if(oper == '*')res->setdata("o:*");
    if(oper == '/')res->setdata("o:/");
    if(oper == '^')res->setdata("o:^");
    prev = res;
    }
  return prev;
  }

Node* Parser::P()
  {
  char symbol = Expression_[ExIndex_];
  Node* res = NULL;
  if(symbol == '(')
    {
    ExIndex_++;
    res = E();
    if(Expression_[ExIndex_] != ')')
      {
      printf("ERROR:brace is lost\n");
      exit(1);
      }
    ExIndex_++;
    }
  else if(isalpha(symbol))
    {
    char func[PARSER_SIGNSIZE];
    sscanf(Expression_ + ExIndex_, "%3[a-z]", func);
    if(0) {}     //KOSTYL
    #define OPERATORS
    #define OPERATOR(sign, name, code) \
      else if(!strcmp(func, #sign)) \
        { \
        res = new Node; \
        ExIndex_ += strlen(func); \
        if(Expression_[ExIndex_] != '(') \
          { \
          printf("ERROR:lost brace\n"); \
          exit(1); \
          } \
        ExIndex_++; \
        Node* val = E(); \
        if(Expression_[ExIndex_] != ')') \
          { \
          printf("ERROR:lost brace\n"); \
          exit(1); \
          } \
        res->setdata("o:"#sign); \
        res->setright(val); \
        ExIndex_++; \
        }
    #include "Operators.txt"  
    #undef OPERATOR
    #undef OPERATORS
    else
      {
      res = N();
      }
    }
  else res = N();
  return res;
  }

Node* Parser::N()
  {
  if(!isdigit(Expression_[ExIndex_]) && Expression_[ExIndex_] != 'x')
    {
    printf("ERROR:can't interpret the input\n");
    exit(1);
    }
  Node* res = new Node;
  if(Expression_[ExIndex_] == 'x')
    {
    res->setdata("v:x");
    ExIndex_++;
    return res;
    }
  double val = 0, exp = 0;
  while(isdigit(Expression_[ExIndex_]) || Expression_[ExIndex_] == '.')
    {
    if(Expression_[ExIndex_] == '.' && exp == 0)exp = 1;
    else if(Expression_[ExIndex_] == '.' && exp != 0)
      {
      printf("ERROR:extra point in number\n");
      exit(1);
      }
    else if(exp == 0)
      {
      val = val * 10 + Expression_[ExIndex_] - '0';
      }
    else if(exp > 0)
      {
      exp /= 10;
      val += exp * (Expression_[ExIndex_] - '0');
      }
    ExIndex_++;
    }
  char strnumber[PARSER_DOUBLE_DIGITS];
  char data[PARSER_DOUBLE_DIGITS + 2];
  sprintf(strnumber, "%f", val);
  strcpy(data, "c:");
  strcat(data, strnumber);
  res->setdata(data);
  return res;
  }
#endif

