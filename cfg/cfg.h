#include <string>
#include <vector>

using namespace std;

struct TACVar {
  string name;
  int index;
  
  CFGVar(string, int);
};

struct TACOperation {
  TACVar target;
  TACVar left;
  TACVar right;
  int op;
  
  TACExpression(TACVar target, TACVar left, int op, TACVar right);
};

struct BasicBlock {
  string name;
  vector<TACExpression> ops
};
