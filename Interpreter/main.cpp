#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "CommentRemove.h"
#include "Token.h"
#include "RecursiveDescentParser.h"
#include "SymbolTable.h"
#include "AbstractSyntaxTree.h"

using namespace std;


// Main function
int main() {
    vector<Token> tokenlist;
    string fileName ="programming_assignment_5-test_file_";
    string tokenizefile="test_file";
    Parser *CST;
    SymbolTable *Table;

//    for(int i=1; i < 2; i++) {
//        ignoreComments(fileName + std::to_string(i) + ".c", tokenizefile + std::to_string(i) + ".c");
//        tokenlist = Tokenize(tokenizefile + std::to_string(i) + ".c");
//        CST= new Parser(tokenlist);
//        CST->buildCST();
//        cout<<"CST built successfully"<<endl;
//        CST->tree->breadthFirstTraversal();
//        CST->tree->resetCurrentNode();
//        Table=new SymbolTable(CST->getCST());
//        Table->BuildTable();
//        Table->printSymbolTable();
//    }

    tokenlist = Tokenize("parseExpressionTest.c");
    CST= new Parser(tokenlist);
    CST->buildCST();
    cout<<"CST built successfully"<<endl;
    CST->tree->breadthFirstTraversal();
    CST->tree->resetCurrentNode();
    Table=new SymbolTable(CST->getCST());
    Table->BuildTable();
    Table->printSymbolTable();
    AbstractSyntaxTree expressionTestTree(CST->tree);
    expressionTestTree.buildAST();

//    int i=1;
//    ignoreComments(fileName + std::to_string(i) + ".c", tokenizefile + std::to_string(i) + ".c");
//    tokenlist = Tokenize(tokenizefile + std::to_string(i) + ".c");
//    Parser CST(tokenlist);
//    CST.buildCST();
//    cout<<"CST built successfully"<<endl;
//    CST.tree.breadthFirstTraversal();

    return 0;
}
