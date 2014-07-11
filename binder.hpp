#ifndef __Z2H_BINDER__
#define __Z2H_BINDER__ = 1

#include <stddef.h>
#include <functional>

#include "token.hpp"
#include "symbol.hpp"
#include "parser.hpp"

using namespace std::placeholders;

namespace z2h {

    template <typename TAst> 
    class Token;

    template <typename TAst> 
    class Symbol;

    template <typename TAst, typename TParser> 
    class Parser;

    template <typename TAst>
    using StdFunc = std::function<TAst()>;

    template <typename TAst>
    using NudFunc = std::function<TAst(Token<TAst> *token)>;

    template <typename TAst>
    using LedFunc = std::function<TAst(TAst left, Token<TAst> *token)>;

    template <typename TAst>
    using ScanFunc = std::function<long(Symbol<TAst> *symbol, const std::string &source, size_t lbp)>;

    template <typename TAst, typename TParser>
    using StdPtr = TAst (TParser::*)();

    template <typename TAst, typename TParser>
    using NudPtr = TAst (TParser::*)(Token<TAst> *token);

    template <typename TAst , typename TParser>
    using LedPtr = TAst (TParser::*)(TAst left, Token<TAst> *token);

    template <typename TAst, typename TParser>
    using ScanPtr = long (TParser::*)(Symbol<TAst> *symbol, const std::string &source, size_t lbp);

    template <typename TAst, typename TParser>
    struct Binder {

        StdFunc<TAst> BindStd(nullptr_t method) {
            return nullptr;
        }

        StdFunc<TAst> BindStd(StdPtr<TAst, TParser> method) {
            return std::bind(method, static_cast<TParser *>(this));
        }

        NudFunc<TAst> BindNud(nullptr_t method) {
            return nullptr;
        }

        NudFunc<TAst> BindNud(NudPtr<TAst, TParser> method) {
            return std::bind(method, static_cast<TParser *>(this), _1);
        }

        LedFunc<TAst> BindLed(nullptr_t method) {
            return nullptr;
        }

        LedFunc<TAst> BindLed(LedPtr<TAst, TParser> method) {
            return std::bind(method, static_cast<TParser *>(this), _1, _2);
        }
/*
        ScanFunc<TAst> BindScan(nullptr_t method) {
            return nullptr;
        }
*/
        ScanFunc<TAst> BindScan(ScanPtr<TAst, TParser> method) {
            return std::bind(method, static_cast<TParser *>(this), _1, _2, _3);
        }
    };

}

#endif /*__Z2H_BINDER__*/
