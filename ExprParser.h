#ifndef EXPRPARSER_H
#define EXPRPARSER_H

#include <cstdio>
#include <cctype>
#include <cstring>
#include <cmath>
#include <cstdlib>

#include "Tree.h"
#include "Tree.cpp"

enum CONSTANTS_PARSER
  {
  PARSER_EXSIZE = 1024,
  PARSER_DOUBLE_DIGITS = 32,
  PARSER_SIGNSIZE = 8,
  };

#define FORMATLINE "%1023s"


class Parser
  {
  public:
    Parser();
    ~Parser();

    Node* operator () (char* expression); //functor

    Node* G();
    Node* E();
    Node* T();
    Node* P();
    Node* N();
  private:
    char* Expression_;
    size_t ExIndex_;
  };
#endif
