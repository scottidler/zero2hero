#ifndef __Z2H_AST__
#define __Z2H_AST__ = 1

#include <string>
#include <vector>
#include <iostream>

namespace z2h {

    class Token;

    struct Ast {
        virtual ~Ast() {}
        Ast() {}

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

    struct NullAst : public Ast {
        virtual ~NullAst() {}
        NullAst() {}
    protected:
        void Print(std::ostream &os) const {
            os << "( )";
        }
    };

    struct ValueAst : public Ast {
        std::string name;
        Token *token;
        virtual ~ValueAst () {}
        ValueAst(Token *token)
            : name("")
            , token(token) {}
        ValueAst(std::string name, Token *token)
            : name(name)
            , token(token) {}
    protected:
        void Print(std::ostream &os) const {
            os << "(";
            if (name.empty())
                os << token->symbol->pattern;
            else
                os << name;
            os << ")";
        }
    };

    struct BinaryAst : public ValueAst {
        Ast *left;
        Ast *right;

        ~BinaryAst() {}
        BinaryAst(Token *token, Ast *left, Ast *right)
            : ValueAst(token)
            , left(left)
            , right(right) {}
        BinaryAst(std::string name, Token *token, Ast *left, Ast *right)
            : ValueAst(name, token)
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

    struct VectorAst : public ValueAst {
        std::vector<Ast *> asts;

        ~VectorAst() {}
        VectorAst(std::vector<Ast *> asts)
            : ValueAst(token)
            , asts(asts) {}
        VectorAst(std::string name, std::vector<Ast *> asts)
            : ValueAst(name, token)
            , asts(asts) {}

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
