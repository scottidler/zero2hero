#ifndef __Z2H_AST__
#define __Z2H_AST__ = 1

#include <string>
#include <vector>
#include <iostream>

namespace z2h {

    class Token;

    struct Ast {
        Token *token;
        
        //virtual ~Ast() = 0;
        Ast(Token *token = nullptr)
            : token(token) {}

    protected:
        virtual void Print(std::ostream &os) const = 0;
        friend std::ostream & operator <<(std::ostream &os, const Ast &ast) {
            ast.Print(os);
            return os;
        }
    };

}
#endif /*__Z2H_AST__*/
