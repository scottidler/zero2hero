#ifndef __Z2H_AST__
#define __Z2H_AST__ = 1

#include <string>
#include <vector>
#include <iostream>

namespace z2h {

    class Token;

    struct Ast {
        Token *token;
        
        virtual ~Ast() {}
        Ast(Token *token = nullptr)
            : token(token) {}

        virtual size_t Size() const {
            return 1;
        }
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

        virtual size_t Size() const {
            return (left ? left->Size() : 0) + (right ? right->Size() : 0);
        }
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

        virtual size_t Size() const {
            size_t result = 0;
            for (auto ast : asts)
                result += ast->Size();
            return result;
        }

        std::vector<Ast *> Vectorize() {
            std::vector<Ast *> results;
            for (auto ast : asts) {
                auto astv = ast->Vectorize();
                results.insert(results.end(), astv.begin(), astv.end());
            }
            return results;
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
