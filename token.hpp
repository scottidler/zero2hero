#ifndef __Z2H_TOKEN__
#define __Z2H_TOKEN__ = 1

#include <string>
#include <iostream>

#include "symbol.hpp"

namespace z2h {

    class Symbol;

    struct Token {
        Symbol          *symbol;
        std::string     value;
        size_t          position;
        size_t          length;
        bool            skip;

        Token()
            : symbol(new Symbol())
            , value("")
            , position(0)
            , length(0)
            , skip(false) {
        }

        Token(Symbol *symbol, std::string value, size_t position, size_t length, bool skip = false)
            : symbol(symbol)
            , value(value)
            , position(position)
            , length(length)
            , skip(skip) {
        }

        operator bool() {
           return symbol != nullptr;
        }

        friend std::ostream & operator<<(std::ostream &out, const Token &token) {
            return out
                << "(Token: value=" << escape(token.value)
                << " position=" << token.position
                << " length=" << token.length
                << " skip=" << (token.skip ? "true" : "false")
                << " symbol=" << *token.symbol
                <<  ")";
        }
    };

}

#endif /*__Z2H_TOKEN__*/
