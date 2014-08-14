#ifndef __Z2H_SYMBOL__
#define __Z2H_SYMBOL__ = 1

#include <string>
#include <iostream>
#include <functional>

#include "token.hpp"
#include "parser.hpp"
#include "binder.hpp"

namespace z2h {

    inline std::string escape(std::string text) {
        std::string result = "";
        for (char c : text) {
            switch (c) {
                case '\n':
                    result += "\\n";
                    break;
                case '\r':
                    result += "\\r";
                    break;
                case '\t':
                    result += "\\t";
                    break;
                default:
                    result += c;
            }
        }
        return result;
    }

    template <typename TAst> 
    class Token;

    template <typename TAst, typename TParser> 
    class Parser;

    template <typename TAst> 
    class Symbol;

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

        friend std::ostream & operator<<(std::ostream &out, const Symbol &symbol) {
            return out
                << "(Symbol: type=" << symbol.type
                << " lbp=" << symbol.lbp
                << " pattern=" << escape(symbol.pattern)
                << " Scan=" << (symbol.Scan != nullptr ? "true" : "null")
                << " Std=" << (symbol.Std != nullptr ? "true" : "null")
                << " Nud=" << (symbol.Nud != nullptr ? "true" : "null")
                << " Led=" << (symbol.Led != nullptr ? "true" : "null")
                << ")";
        }

    };
}

#endif /*__Z2H_SYMBOL__*/
