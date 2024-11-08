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
    AbstractSyntaxTree *AST;

    for(int i=4; i < 6; i++) {
        cout<<endl;
        ignoreComments(fileName + std::to_string(i) + ".c", tokenizefile + std::to_string(i) + ".c");
        tokenlist = Tokenize(tokenizefile + std::to_string(i) + ".c");
        CST= new Parser(tokenlist);
        CST->buildCST();
        cout<<"CST built successfully"<<endl;
        CST->tree->breadthFirstTraversal();
        CST->tree->resetCurrentNode();
        Table=new SymbolTable(CST->getCST());
        Table->BuildTable();
        AST=new AbstractSyntaxTree(CST->getCST());
        AST->buildAST();
        cout<<"AST built Successfully"<<endl<<endl;
        AST->getAST()->breadthFirstTraversal();
    }
    return 0;
}
