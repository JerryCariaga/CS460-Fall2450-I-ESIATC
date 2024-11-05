//
// Created by storm on 10/31/2024.
//

#ifndef INTERPRETER_ABSTRACTSYNTAXTREE_H
#define INTERPRETER_ABSTRACTSYNTAXTREE_H
#include "RecursiveDescentParser.h"
#include "string"
#include "stack"


class AbstractSyntaxTree{
    LCRSTree *AST;
    LCRSTree *CST;
public:

    AbstractSyntaxTree(LCRSTree *builtCST){CST=builtCST;AST=new LCRSTree();}

    void buildAST()
    {
        CST->resetCurrentNode();
        parseForStatement();



    };

    int opPrecedence(const std::string &opType) {
        if (opType == "ASSIGNMENT_OPERATOR") return 1;
        if (opType == "BOOLEAN_OR") return 2;
        if (opType == "BOOLEAN_AND") return 3;
        if (opType == "BOOLEAN_EQUAL" || opType == "BOOLEAN_NOT_EQUAL" ||
            opType == "LT" || opType == "GT" ||
            opType == "LT_EQUAL" || opType == "GT_EQUAL") return 4;
        if (opType == "PLUS" || opType == "MINUS") return 5;
        if (opType == "ASTERISK" || opType == "DIVIDE" || opType == "MODULO") return 6;
        if (opType == "BOOLEAN_NOT") return 7;
        return 0;
    }

    //after this runs, the current node should be the child of the last token
    void parseExpression() {
        std::stack<Node*> operatorStack;
        Node *current = CST->getCurrentNode();

        bool expression = true;
        while (expression) {
            if (current->rightSibling == nullptr) { //end of current expression
                expression = false;
                current = current->leftChild;
                continue;
            }
            if (current->data.getType() == "IDENTIFIER" || current->data.getType() == "INTEGER" ||
                current->data.getType() == "SINGLE_QUOTE" || current->data.getType() == "DOUBLE_QUOTE" ||
                current->data.getType() == "STRING") {
                //cout << current->data.getName() << " ";
                AST->insertSibling(new Node(current->data));
                current = current->rightSibling;
            }
            else if (current->data.getType() == "L_PAREN") {
                operatorStack.push(current);
                current = current->rightSibling;
            }
            else if (current->data.getType() == "R_PAREN") { //dump operator stack until L_PAREN
                while (!operatorStack.empty() && operatorStack.top()->data.getType() != "L_PAREN") {
                    //cout << operatorStack.top()->data.getName() << " ";
                    AST->insertSibling(new Node(operatorStack.top()->data));
                    operatorStack.pop();
                }
                if (!operatorStack.empty()) {
                    operatorStack.pop(); //pop L_PAREN
                }
                current = current->rightSibling;
            }
            else {
                int currentPrecedence = opPrecedence(current->data.getType());
                while (!operatorStack.empty() && opPrecedence(operatorStack.top()->data.getType()) >= currentPrecedence) {
                    //cout << operatorStack.top()->data.getName() << " ";
                    AST->insertSibling(new Node(operatorStack.top()->data));
                    operatorStack.pop();
                }
                operatorStack.push(current);
                current = current->rightSibling;
            }
        }
        while (!operatorStack.empty()) {
            //cout << operatorStack.top()->data.getName() << " ";
            AST->insertSibling(operatorStack.top());
            operatorStack.pop();
        }
    }


    //ASSUME YOUR PARSE FUNCTION ALREADY IDENTIFIED ITSELF.  AKA THE FIRST NODE OF THE SIBLING CHAIN IS IDENTIFIED
    //parseIfandWhile
    //parseFor
    //parseExpression
    //
    //

    // PA5: parseForStatement();
    // Called on top of a for statement
    // convert this part of the for expression into post-fix maybe using external function
    // example: for (i=0;i<something;i++); == for expression 1 i 0 = for expression 2 i something < ...
    void parseForStatement(int forExpression = 1) {

        //modify given curr node to say for expression 'x'
        Node* currNode = CST->getCurrentNode();
        currNode->data.type = "for expression " + to_string(forExpression);

        // This will be called on the for statement,
        // inserting for statement into AST
        AST->insertSibling(new Node(currNode->data));

        // Continue to the right of CST
        currNode = currNode->rightSibling;

        // For AST we ignore the L_PAREN token.
        if (currNode->data.getType() == "L_PAREN") {
            currNode = currNode->rightSibling; // go past the L_PAREN
            parseExpression(); // Should be on top of an expression
            parseForStatement(forExpression+=1); // Go again.
        }

        // On semicolon, insert another for statement and parse expression
        if (currNode->data.getType() == "SEMICOLON") {
            parseExpression();
            currNode = currNode->rightSibling; // Go to the next expression.
            parseForStatement(forExpression+=1); // Go again.
        }

        // end of for statement, stop here.
        if (currNode->data.getType() == "R_PAREN") {
            currNode = currNode->leftChild; // Go down the tree.
        }


    }
};




#endif //INTERPRETER_ABSTRACTSYNTAXTREE_H