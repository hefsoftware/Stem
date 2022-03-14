#pragma once
#include "../../Parser/parser.h"
#include "../../Parser/expressionparser.h"
#include "../../Parser/Node/node.h"
namespace QTwig
{
//apply
//autoescape
//cache
//deprecated
//do
//flush
//for
//sandbox
//use
//verbatim
//with
  namespace TagParse
  {
    // Control tags
    Node::Ptr parseSet(Parser &p);
    Node::Ptr parseIf(Parser &p);
    // Template tags
    Node::Ptr parseBlock(Parser &p);
    Node::Ptr parseMacro(Parser &p);
    Node::Ptr parseExtends(Parser &p);
    Node::Ptr parseFrom(Parser &p);
    Node::Ptr parseImport(Parser &p);
    Node::Ptr parseInclude(Parser &p);
    Node::Ptr parseEmbed(Parser &p);
    Node::Ptr parseUse(Parser &p);
  }
};
