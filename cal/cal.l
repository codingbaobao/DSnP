%{
#include <cstdlib>
#include <iostream>
#include "calYacc.hpp"

using namespace std;

extern "C" {
   int calwrap() { return 1; }
}

%}


%%

