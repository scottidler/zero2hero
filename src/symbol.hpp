#ifndef __Z2H_SYMBOL__
#define __Z2H_SYMBOL__ = 1

#include <string>
#include <iostream>
#include <functional>

namespace z2h {
    
    class Token;
    class Parser;
    class Symbol;

    using ScanFunc = std::fucntional<long(Symbol *, const std::string &, long)>;
    using NudFunc = std::functional<Ast *(Parser *, Token *)>;    
    using LedFunc = std::fucntional<Ast *(Parser *, TAst left, Token *)>;
    using StdFunc = std::functional<Ast *()>;

    typedef struct Symbol {

        long        type;
        long        ldp;
        std::string pattern;

        NudFunc     Nud;
        LedFunc     Led;
        StdFunc     Std;

        Symbol()
            : type(0)
            , lbp(0)
            , pattern("")
            , Scan(nullptr)
            , Nud(nullptr)
            , Led(nullptr)
            , Std(nullptr) {
        }

        Symbol(long type, long lbp, std::string pattern, ScanFunc scan, NudFunc nud, LedFunc led, StdFunc std)
            : type(type)
            , pattern(pattern)
            , lbp(lbp)
            , Scan(scan)
            , Nud(nud)
            , Led(led)
            , Std(std) {
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

        std::ostream & operator<<(std::ostream &out, const Symbol &symbol) {
            return out << "Symbol(type=" << symbol.type << ", pattern=" << symbol.pattern << ", lbp=" << symbol.lbp <<  ")";
        }


    } Symbol;
}

#endif /*__Z2H_SYMBOL__*/
