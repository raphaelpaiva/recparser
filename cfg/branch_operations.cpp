#include <string>
#include <sstream>
#include "branch_operations.h"

string Br::str()
{
  stringstream ss;
  
  ss << "br label %" << basic_block->name();
  
  return ss.str();
}

string Brc::str()
{
  stringstream ss;
  
  ss << "br i1 " << *cond << ", label %" << true_block->name() << ", label %" << false_block->name();
  
  return ss.str();
}

