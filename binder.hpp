#ifndef __Z2H_BINDER__
#define __Z2H_BINDER__ = 1

#include <stddef.h>
#include <functional>

using namespace std::placeholders;

namespace z2h {

    class Ast;
    class Token;
    class Symbol;

    template <typename TParser>
    class Parser;

    using StdFunc = std::function<Ast *()>;
    using NudFunc = std::function<Ast *(Token *token)>;
    using LedFunc = std::function<Ast *(Ast *left, Token *token)>;
    using ScanFunc = std::function<Token *(Symbol *symbol, const std::string &source, size_t lbp)>;

    template <typename TParser>
    using StdPtr = Ast * (TParser::*)();

    template <typename TParser>
    using NudPtr = Ast * (TParser::*)(Token *token);

    template <typename TParser>
    using LedPtr = Ast * (TParser::*)(Ast *left, Token *token);

    template <typename TParser>
    using ScanPtr = Token * (TParser::*)(Symbol *symbol, const std::string &source, size_t lbp);

    template <typename TParser>
    struct Binder {

        static constexpr nullptr_t Nullptr = nullptr;

        StdFunc BindStd(nullptr_t method) {
            return nullptr;
        }

        StdFunc BindStd(const nullptr_t *method) {
            return nullptr;
        }

        StdFunc BindStd(StdPtr<TParser> method) {
            return std::bind(method, static_cast<TParser *>(this));
        }

        NudFunc BindNud(nullptr_t method) {
            return nullptr;
        }

        NudFunc BindNud(const nullptr_t *method) {
            return nullptr;
        }

        NudFunc BindNud(NudPtr<TParser> method) {
            return std::bind(method, static_cast<TParser *>(this), _1);
        }

        LedFunc BindLed(nullptr_t method) {
            return nullptr;
        }

        LedFunc BindLed(const nullptr_t *method) {
            return nullptr;
        }

        LedFunc BindLed(LedPtr<TParser> method) {
            return std::bind(method, static_cast<TParser *>(this), _1, _2);
        }

        ScanFunc BindScan(nullptr_t method) {
            return nullptr;
        }

        ScanFunc BindScan(const nullptr_t *method) {
            return nullptr;
        }

        ScanFunc BindScan(ScanPtr<TParser> method) {
            return std::bind(method, static_cast<TParser *>(this), _1, _2, _3);
        }
    };

    template <typename TParser>
    constexpr nullptr_t Binder<TParser>::Nullptr;

}

#endif /*__Z2H_BINDER__*/
