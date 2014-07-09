#ifndef __Z2H_SYMBOL__
#define __Z2H_SYMBOL__ = 1

#include <string>
#include <iostream>
#include <functional>

#include "token.hpp"
#include "parser.hpp"

namespace z2h {
   
    template <typename TAst> 
    class Token;

    template <typename TAst> 
    class Parser;

    template <typename TAst> 
    class Symbol;

    template <typename TAst>
    using ScanFunc = std::function<long(Symbol<TAst> *, const std::string &, size_t)>;
    template <typename TAst>
    using StdFunc = std::function<TAst()>;
    template <typename TAst>
    using NudFunc = std::function<TAst(Token<TAst> *)>;
    template <typename TAst>
    using LedFunc = std::function<TAst(TAst left, Token<TAst> *)>;

    template <typename TAst>
    class Symbol {
    public:

        size_t          type;
        size_t          lbp;
        std::string     pattern;

        ScanFunc<TAst>  Scan;
        StdFunc<TAst>   Std;
        NudFunc<TAst>   Nud;
        LedFunc<TAst>   Led;

        Symbol()
            : type(0)
            , lbp(0)
            , pattern("")
            , Scan(nullptr)
            , Std(nullptr)
            , Nud(nullptr)
            , Led(nullptr) {
        }

        Symbol(size_t type, size_t lbp, std::string pattern, ScanFunc<TAst> scan, StdFunc<TAst> std, NudFunc<TAst> nud, LedFunc<TAst> led)
            : type(type)
            , lbp(lbp)
            , pattern(pattern)
            , Scan(scan)
            , Std(std)
            , Nud(nud)
            , Led(led) {
        }

        operator bool() {
            return type;
        }

        bool operator==(const Symbol &rhs) {
            return type == rhs.type;
        }

        bool operator!=(const Symbol &rhs) {
            return type != rhs.type;
        }

        /*
        std::ostream & operator<<(std::ostream &out, const Symbol &symbol) {
            return out << "Symbol(type=" << symbol.type << ", pattern=" << symbol.pattern << ", lbp=" << symbol.lbp <<  ")";
        }
        */

    };
}

#endif /*__Z2H_SYMBOL__*/
