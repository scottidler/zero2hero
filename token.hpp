#ifndef __Z2H_TOKEN__
#define __Z2H_TOKEN__ = 1

#include <string>
#include <iostream>

#include "parser.hpp"
#include "symbol.hpp"

namespace z2h {

    template<typename TAst>
    class Ast;

    template<typename TAst>
    class Parser;

    template<typename TAst>
    class Symbol;

    template<typename TAst>
    class Token {
    public:

        Symbol<TAst>    *symbol;
        std::string     source;
        long            position;
        long            length;
        bool            skip;

        Token()
            : symbol(new Symbol<TAst>())
            , source("")
            , position(0)
            , length(0)
            , skip(false) {
        }

        Token(Symbol<TAst> *symbol, const std::string &source, size_t position, size_t length, bool skip)
            : symbol(symbol)
            , source(source)
            , position(position)
            , length(length)
            , skip(skip) {
        }

        std::string Value() const {
            return source.substr(position, length);
        }

        TAst Nud(Parser<TAst> *parser, Token<TAst> *token) {
            return symbol->Nud(parser, token);
        }

        TAst Led(Parser<TAst> *parser, TAst ast, Token<TAst> *token) {
            return symbol->Led(parser, ast, token);
        }

        TAst Std(Parser<TAst> *parser) {
            return symbol->Std(parser);
        }

        operator bool() {
           return symbol == true;
        }

        friend std::ostream & operator<<(std::ostream &out, const Token &token) {
            return out << "Token(symbol=" << *token.symbol <<  ", Value()=" << token.Value() << ")";
        }
    };

}

#endif /*__Z2H_TOKEN__*/
