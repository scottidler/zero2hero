#ifndef __Z2H_SYMBOL__
#define __Z2H_SYMBOL__ = 1

#include <string>
#include <iostream>
#include <functional>

namespace z2h {

    class Ast;
    class Token;

    using StdFunc = std::function<Ast *()>;
    using NudFunc = std::function<Ast *(Token *token)>;
    using LedFunc = std::function<Ast *(Ast *left, Token *token)>;

    struct Symbol {

        size_t      lbp;
        StdFunc     Std;
        NudFunc     Nud;
        LedFunc     Led;

        virtual ~Symbol() {}

        Symbol()
            : lbp(0)
            , Std(nullptr)
            , Nud(nullptr)
            , Led(nullptr) {
        }

        Symbol(size_t lbp, StdFunc std, NudFunc nud, LedFunc led)
            : lbp(lbp)
            , Std(std)
            , Nud(nud)
            , Led(led) {
        }

        friend std::ostream & operator<<(std::ostream &out, const Symbol &symbol) {
            return out
                << "(Symbol: lbp=" << symbol.lbp
                << " Std=" << (symbol.Std != nullptr ? "true" : "null")
                << " Nud=" << (symbol.Nud != nullptr ? "true" : "null")
                << " Led=" << (symbol.Led != nullptr ? "true" : "null")
                << ")";
        }
    };
}

#endif /*__Z2H_SYMBOL__*/
