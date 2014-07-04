#ifndef __Z2H_GRAMMAR__
#define __Z2H_GRAMMAR__ = 1

#include <vector>

namespace z2h {

    class Symbol;

    template<typename TAst>
    typedef struct Grammar {
        
        virtual std::vector<Symbol *> Symbols() = 0;

    } Grammar;

}

#endif /*__Z2H_GRAMMAR__*/
