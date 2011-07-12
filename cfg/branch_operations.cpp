#include <string>
#include <sstream>
#include "branch_operations.h"

string Br::str()
{
  stringstream ss;
  
  ss << "br " << basic_block->name();
  
  return ss.str();
}

string Brc::str()
{
  stringstream ss;
  
  ss << "brc " << *cond << " " << true_block->name() << " " << false_block->name();
  
  return ss.str();
}

