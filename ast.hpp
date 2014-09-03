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
        virtual ~Ast() {}
        Ast(Token *token = nullptr)
            : token(token) {}

        virtual std::vector<Ast *> Vectorize() {
            return { this };
        }

    protected:
        virtual void Print(std::ostream &os) const = 0;
        friend std::ostream & operator <<(std::ostream &os, const Ast &ast) {
            ast.Print(os);
            return os;
        }
    };

    struct BinaryAst : public Ast {
        Ast *left;
        Ast *right;

        ~BinaryAst() {}
        BinaryAst(Token *token, Ast *left, Ast *right)
            : Ast(token)
            , left(left)
            , right(right) {}

        std::vector<Ast *> Vectorize() {
            auto lefts = left->Vectorize();
            auto rights = right->Vectorize();
            lefts.insert(lefts.end(), rights.begin(), rights.end());
            return lefts;
        }

    protected:
        virtual void Print(std::ostream &os) const {
            os << "(" << token->value << " " << *left << " " << *right << ")";
        }
    };

    struct VectorAst : public Ast {
        std::vector<Ast *> asts;
        std::string name;

        ~VectorAst() {}
        VectorAst(std::vector<Ast *> asts, std::string name = "")
            : Ast(token)
            , asts(asts)
            , name(name) {}

        std::vector<Ast *> Vectorize() {
            return asts;
        }

    protected:
        virtual void Print(std::ostream &os) const {
            os << "(" << name;
            for (auto ast : asts) {
                os << " " << *ast;
            }
            os << ")";
        }
    };

}
#endif /*__Z2H_AST__*/
