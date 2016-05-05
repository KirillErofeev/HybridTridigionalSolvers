#include "TridigionalEquation.hpp"
void testWithFullOutput(TridigionalEquation& eq, std::ostream& os){

//    os << eq;
    eq.inverse();
    os << eq;
}
