#ifndef __Z2H_TOKEN__
#define __Z2H_TOKEN__ = 1

#include <string>
#include <iostream>

namespace z2h {

    class Ast;
    class Token;
    class Parser;

    typedef struct Token {

        Symbol      *symbol;
        std::string source;
        long        postion;
        long        length;
        bool        skip;

        Token()
            : symbol(Symbol())
            , source("")
            , position(0)
            , length(0)
            , skip(false) {
        }

        Token(Symbol *symbol, const std::string &source, size_t position, size_t length, bool skip)
            : symbol(symbol)
            , source(source)
            , position(index)
            , length(length)
            , skip(skip) {
        }

        std::string Value() const {
            return source.substr(position, length);
        }

        Ast * Nud(Parser *parser, Token *token) {
            return symbol->Nud(parser, token);
        }

        Ast * Led(Parser *parser, Ast *ast, Token *token) {
            return symbol->Led(parser, ast, token);
        }

        Ast * Std(Parser *parser, ast, token) {
            return symbol->Std(parser, ast, token);
        }

        operator bool() {
           return symbol == true;
        }

        std::ostream & operator<<(std::ostream &out, const Token &token) {
            return out << "Token(symbol=" << *token.symbol <<  ", Value()=" << token.Value() << ")";
        }
    };

}

#endif /*__Z2H_TOKEN__*/
