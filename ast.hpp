#ifndef __Z2H_AST__
#define __Z2H_AST__ = 1

#include <string>
#include <vector>
#include <iostream>

namespace z2h {

    class Token;

    struct Ast {
        //Token *token;
        std::string value;
        
        virtual ~Ast() {}
        Ast(std::string value = "")
            : value(value) {}
        Ast(Token *token = nullptr)
            : Ast(token ? token->value : "") {}

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
            os << "(" << value << " " << *left << " " << *right << ")";
        }
    };

    struct VectorAst : public Ast {
        std::vector<Ast *> asts;

        ~VectorAst() {}
        VectorAst(std::string value, std::vector<Ast *> asts)
            : Ast(value)
            , asts(asts) {}
        VectorAst(Token *token, std::vector<Ast *> asts)
            : Ast(token)
            , asts(asts) {}

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
            os << "(" << value;
            for (auto ast : asts) {
                os << " " << *ast;
            }
            os << ")";
        }
    };

}
#endif /*__Z2H_AST__*/
