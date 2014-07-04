#ifndef __Z2H_GRAMMAR__
#define __Z2H_GRAMMAR__ = 1

#include <vector>

#include "symbol.hpp"

namespace z2h {

    template <typename TAst>
    class Symbol;

    template<typename TAst>
    class Grammar {
    public:    
        virtual std::vector<Symbol<TAst> *> Symbols() = 0;

    };

}

#endif /*__Z2H_GRAMMAR__*/
