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

        size_t      type;
        size_t      lbp;
        StdFunc     Std;
        NudFunc     Nud;
        LedFunc     Led;
        bool        enabled;

        virtual ~Symbol() {}

        Symbol()
            : type(0)
            , lbp(0)
            , Std(nullptr)
            , Nud(nullptr)
            , Led(nullptr)
            , enabled(true) {
        }

        Symbol(size_t type, size_t lbp, StdFunc std, NudFunc nud, LedFunc led)
            : type(type)
            , lbp(lbp)
            , Std(std)
            , Nud(nud)
            , Led(led)
            , enabled(true) {
        }

        friend std::ostream & operator<<(std::ostream &out, const Symbol &symbol) {
            return out
                << "(Symbol: type=" << symbol.type
                << " lbp=" << symbol.lbp
                << " Std=" << (symbol.Std != nullptr ? "true" : "null")
                << " Nud=" << (symbol.Nud != nullptr ? "true" : "null")
                << " Led=" << (symbol.Led != nullptr ? "true" : "null")
                << " enabled=" << (symbol.enabled ? "true" : "false")
                << ")";
        }
    };
}

#endif /*__Z2H_SYMBOL__*/
