#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include "CommentRemove.h"
#include "Token.h"

using namespace std;

std::string typekeyword[]={"int","bool","char","void"};
std::string conditionalkeyword[]={"if","for","while","else"};
vector<string> operandlist={"PLUS","MINUS","ASTERISK","DIVIDE","MODULO","LT_EQUAL","LT","GT_EQUAL","GT","BOOLEAN_AND","BOOLEAN_OR","BOOLEAN_NOT_EQUAL"};
//will need an array of reserved functions in the future

enum keywords{Identifier=0,Type=1,Conditional=2,RETURN=3,Function=4};
// Node of the LCRS Tree
struct Node {
    Token data;
    Node* leftChild;
    Node* rightSibling;

    Node(Token token) : data(token), leftChild(nullptr), rightSibling(nullptr) {}
};



class LCRSTree {
private:
    Node* currentNode;

    // Recursive helper function to delete nodes
    void deleteNode(Node* node) {
        if (!node) return;
        deleteNode(node->leftChild);
        deleteNode(node->rightSibling);
        delete node;
    }

public:
    Node* root;

    LCRSTree() : root(nullptr), currentNode(nullptr) {}

    ~LCRSTree() { deleteNode(root); }

    // Insert child under the given parent
    void insertChild(Node* child) {
        if(root == nullptr) {
            root = child;
            currentNode=root;
            return;
        }
       currentNode->leftChild=child;
       currentNode=child;
    }

    //Purpose: inserts a sibling into the LCRS tree
    //input:  a Node
    //output:  the node is attached to the current node's sibling pointer.  Current node is changed to the sibling node
    void insertSibling(Node* sibling)
    {
        if(root == nullptr ) {
            root = sibling;
            currentNode=root;
            return;
        }
        currentNode->rightSibling=sibling;
        currentNode=sibling;
    }

    // Get the current node (most recently visited in traversal)
    Node* getCurrentNode() {
        return currentNode;
    }


    // Breadth-First Traversal
    void breadthFirstTraversal() {
        if (!root) return;
        Node* traversal=root;

        while(traversal)
        {
            cout<<traversal->data.getName()<<" ";
            if(traversal->rightSibling)
                traversal=traversal->rightSibling;
            else if(traversal->leftChild) {
                cout<<endl;
                traversal = traversal->leftChild;
            }
            else
                break;
        }
    }
};




class Parser {
public:
    std::vector<Token> tokens;
    int current = 0;
    LCRSTree tree;

    Parser(const std::vector<Token>& tokenList) : tokens(tokenList), current(0), tree() {}

    Token& peek() {
        return tokens[current];
    }

    Token& nextToken() {
        current++;
        if(current<tokens.size())
            return tokens[current];
        else
            return tokens[current-1];
    }

    bool match(const string& type) {
        if (peek().getType() == type) {
            return true;
        }
        return false;
    }


    //PURPOSE: checks the parameter string  for what type of identifier it is and returns an int
    //INPUT:  string name (ideally the string passed is from Token.name
    //Output: an enum which describes the identifier.
    int keywordcheck(string name)
    {
        for(auto &word: typekeyword)
        {
            if(name==word)
                return Type;
        }
        for(auto &word:conditionalkeyword)
        {
            if(name==word)
                return Conditional;
        }
        if(name == "return")
        {
            return RETURN;
        }
        if(name == "printf")//will need to change this to a for loop array check in the future
        {
            return Function;
        }
        return Identifier;
    }

    //THE MAIN FUNCTION FOR BUILDING THE CONCRETE SYNTAX TREE
    //Input : Parser.tokens[] , local int bracecounter, local vector<int> bracelocation
    //Output : populates Parser.tree
    void buildCST()
    {
        int bracecounter=0;
        vector<int> bracelocation;

        Token currenttoken=peek();
        while(current < tokens.size())
        {
            if(match("FUNCTION"))
                parseFunctionDeclaration();

            else if(match("IDENTIFIER"))
            {
                int keyword=keywordcheck(peek().getName());
                switch(keyword)
                {
                    case(Type):
                    {
                        parseVariableDeclaration();
                        break;
                    }
                    case(Identifier):
                    case(Function):
                    {
                       parseVariableOperation();
                       break;
                    }
                    case(RETURN):
                    {
                        parseReturn();
                        break;
                    }
                }
            }
           else if(match("PROCEDURE"))
            {
                parseProcedure();
            }
           else if(match("IF")||match("WHILE"))
            {
                parseIfWhileStatement();
            }
           else if(match("ELSE"))
            {
                parseElseStatement();
            }
           else if(match("FOR"))
            {
               cout<<"Have not build the parseFor fuction yet"<<endl;
               exit(0);
               //parseForStatement();
            }

            //procedure declaration
            if(match("SEMICOLON"))
            {parseSemicolon();}

            if(match("L_BRACE") || match("R_BRACE"))
            {parseBrace(bracecounter,bracelocation);}

        }

        if(bracecounter>0)
        {
            cout<<"missing R_BRACE for "<<bracecounter<<" L_BRACE at lines: ";
            for(auto &number : bracelocation)
            {
                cout<<number<<" ";
            }
            exit(-1);
        }
    }
    // parses tokens expecting the syntax for procedure when the token found is PROCEDURE
    void parseProcedure()
    {
        tree.insertChild(new Node(peek()));
        Token tokenused=nextToken();

        if(match("IDENTIFIER") && keywordcheck(peek().getName())==Identifier)
        {
            tree.insertSibling(new Node(tokenused));
            tokenused=nextToken();
        }
        else
            Errorstatement("Procedure Identifier",tokenused);

        if(match("L_PAREN"))
        {
            tree.insertSibling(new Node(tokenused));
            tokenused=nextToken();
        }
        else
            Errorstatement("Procedure L_PAREN",tokenused);

        if(match("IDENTIFIER"))//EDIT EMERGENCY
        {
            //THIS ONLY WORKS IF THE PROCEDURE IS MAIN.  IF HE MAKES A PROCEDURE THAT HAS DATA TYPES OR IS A FUNCTION DECLARATION CHANGE THIS
            tree.insertSibling(new Node(tokenused));
            tokenused=nextToken();
        }
        else
            Errorstatement("Procedure FunctionDeclaration",tokenused);

        if(match("R_PAREN"))
        {
            tree.insertSibling(new Node(tokenused));
            tokenused=nextToken();
        }
        else
            Errorstatement("Procedure R_PAREN",tokenused);


    }


    //this function is called when the token is found to be FUNCTION
    //input: local variable Token tokenused, local variablle bool multipleparameters
    //output: creates a whole sibling chain out of a function declaration and adds it to the tree
    void parseFunctionDeclaration()
    {

        bool multipleparameters=true;
        tree.insertChild(new Node(peek())); //add the function to the tree
        nextToken();

        if(match("IDENTIFIER")  && keywordcheck(peek().getName())==Type)
            tree.insertSibling(new Node(peek()));
        else
            Errorstatement("FunctionDeclaration Keyword",peek());

        nextToken();

        if(match("IDENTIFIER")  && keywordcheck(peek().getName())==Identifier)
            tree.insertSibling(new Node(peek()));
        else
            Errorstatement("FunctionDeclaration Identifier",peek());
        nextToken();

        if(match("L_PAREN") )
            tree.insertSibling(new Node(peek()));
        else
            Errorstatement("FunctionDeclaration LPAREN",peek());
        nextToken();

        while(multipleparameters==true)
        {
            multipleparameters = false;
            parseFunctionDeclarationParameter();

            if(match("COMMA"))
            {
                tree.insertSibling((new Node(peek())));
                nextToken();
                multipleparameters = true;
            }
        }
        if(match("R_PAREN"))
            tree.insertSibling(new Node(peek()));
        else
            Errorstatement("FunctionDeclaration R_PAREN",peek());

        nextToken();
    }

    void parseFunctionDeclarationParameter()
    {
        Token tokenused=peek();

        if(match("IDENTIFIER") && keywordcheck(tokenused.getName())==Type)
        {
            tree.insertSibling(new Node(tokenused));
            tokenused=nextToken();
        }
        else
            Errorstatement("FunctionDeclarationParameter Keyword",tokenused);

        if (match("IDENTIFIER") && keywordcheck(tokenused.getName()) == Identifier) {
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();
        } else
            Errorstatement("FunctionDeclarationParameter Identifier", tokenused);

        if (match("L_BRACKET")) {
            parseBracket();
        }

        if(match("ASSIGNMENT_OPERATOR")){
            parseExpression();
        }

    }

    //Fuction purpose:  parses a snippet of code following the format keyword->identifier->(optional []) -> optional(=)
    void parseVariableDeclaration()
    {
        bool loop=true;
        tree.insertChild(new Node(peek()));
        nextToken();

        while(loop) {
            loop = false;

            if (match("IDENTIFIER") && keywordcheck(peek().getName()) == Identifier) {
                tree.insertSibling(new Node(peek()));
                nextToken();
            } else
                Errorstatement("VariableDeclaration IDENTIFIER", peek());

            if (match("L_BRACKET")) {
                parseBracket();
            }
            if (match("ASSIGNMENT_OPERATOR")) {
                parseExpression();
            }
            if(match("COMMA"))
            {
                loop = true;
                tree.insertSibling(new Node(peek()));
                nextToken();
            }
        }

    }
    //PURPOSE: parses variable operations like sum = x+3
    //Input: token starts on an identifier that is not in a keyword category
    //Output: a full code line stopped by a Semicolon should be pushed to output
    //Might need to account for a bracket call after every identifier
    void parseVariableOperation()
    {
        tree.insertChild(new Node(peek()));
        nextToken();

        if(match("ASSIGNMENT_OPERATOR"))
        {
            tree.insertSibling(new Node(peek()));
            nextToken();
            if(match("DOUBLE_QUOTE")||match("SINGLE_QUOTE"))
                parseString();
            else
                parseExpression();//or boolean
        }
        //the identifier was a function instead of a variable
        else if(match("L_PAREN"))
        {
            tree.insertSibling(new Node(peek()));
            nextToken();

            parseFunctionCallParameters();

            if(match("R_PAREN"))
            {
                tree.insertSibling(new Node(peek()));
                nextToken();
            }
            else
                Errorstatement("VariableOperation R_PAREN",peek());

        }
        if(!match("SEMICOLON"))
            Errorstatement("VariableOperation SEMICOLON",peek());

    }
    void parseFunctionCallParameters()
    {
        bool loop=true;
        while(loop)
        {
            loop = false;
            if(match("DOUBLE_QUOTE")||match("SINGLE_QUOTE"))
                parseString();
            else
            {
                tree.insertSibling(new Node(peek()));
                nextToken();
            }
            if(match("COMMA"))
            {
                loop = true;
                tree.insertSibling(new Node(peek()));
                nextToken();
            }

        }
        if(!match("R_PAREN"))
            Errorstatement("FunctionCallParameter R_PAREN",peek());
    }
    //Purpose: parses and inserts the return statement into the tree without the semicolon
    //Input: current token should be on a RETURN type
    //OUTPUT:  return statement added to the CST
    // might need to add the ability to parse array identifiers
    void parseReturn()
    {
        tree.insertChild(new Node(peek()));
        nextToken();

        if(match("L_PAREN"))
        {
            tree.insertSibling(new Node(peek()));
            nextToken();

            parseExpression(); //or boolean

            if(match("R_PAREN"))
            {
                tree.insertSibling(new Node(peek()));
                nextToken();
            }
            else
                Errorstatement("Return R_PAREN",peek());
        }
        else if(match("IDENTIFIER") && keywordcheck(peek().getName())==Identifier)//EDIT EMERGENCY might need to add parse brackets
        {
            tree.insertSibling(new Node(peek()));
            nextToken();
        }
        if(!match("SEMICOLON"))
            Errorstatement("Return semicolon",peek());
    }

    //this function is called when the token is a string (quote -> string -> quote)
    void parseString() {
        //i assume that at this point, the current token is known to be a DblQuote or SglQuote
        Token tokenused = peek();
        if (match("DOUBLE_QUOTE") ) {
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();

            //invalid: last char of string is "\"

            if ( !match("STRING") || tokenused.getName().back() == '\\')
                Errorstatement("String wrongtokencategory", tokenused);
            else
                tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();

            if (match("DOUBLE_QUOTE")  )
                tree.insertSibling(new Node(tokenused));
            else
                Errorstatement("String Doublequote", tokenused);
            tokenused = nextToken();
        }
        else if (match("SINGLE_QUOTE") ) {
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();

            //invalid: last char of string is "\"

            if (!match("STRING")||  tokenused.getName().back() == '\\'  )
                Errorstatement("String wrongTokenCategory", tokenused);
            else
                tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();

            if (match("SINGLE_QUOTE") )
                tree.insertSibling(new Node(tokenused));
            else
                Errorstatement("String SingleQuote", tokenused);
            tokenused = nextToken();
        }
    }

    //also for while statements?
    void parseIfWhileStatement() {

        //case1: if, case2: while
        if (match("IF")   && keywordcheck(peek().getName()) == Conditional) {
            tree.insertChild(new Node(peek()));
            nextToken();

            if (match("L_PAREN") )
                tree.insertSibling(new Node(peek()));
            else
                Errorstatement("If L_PAREN", peek());
            nextToken();

            parseExpression();

            if (match("R_PAREN")  )
                tree.insertSibling(new Node(peek()));
            else
                Errorstatement("If R_PAREN", peek());
            nextToken();
        }

        else if (match("WHILE")   && keywordcheck(peek().getName()) == Conditional) {
            tree.insertSibling(new Node(peek()));
            nextToken();

            if (match("L_PAREN")  )
                tree.insertSibling(new Node(peek()));
            else
                Errorstatement("While L_PAREN", peek());
             nextToken();

            parseExpression();

            if (match("R_PAREN")  )
                tree.insertSibling(new Node(peek()));
            else
                Errorstatement("While R_PAREN", peek());
            nextToken();
        }

    }

    void parseElseStatement() {
        if (match("ELSE")   && keywordcheck(peek().getName()) == Conditional)
            tree.insertSibling(new Node(peek()));
        else
            Errorstatement("Else ELSE", peek());
        nextToken();

        if (match("L_PAREN")  )
        {
            tree.insertSibling(new Node(peek()));
            nextToken();

            parseExpression();

            if (match("R_PAREN"))
                tree.insertSibling(new Node(peek()));
            else
                Errorstatement("Else R_PAREN", peek());
            nextToken();
        }
        else
            Errorstatement("Else L_PAREN", peek());
    }


    // PA3: RDP - Bracket function
    // Individual function soley designed for handling brackets
    void parseBracket()
    {
        // Add some kind of node
        Token tokenused = peek();

        // Expecting a '['

        if (match("L_BRACKET")) {
            tree.insertSibling(new Node(tokenused));
            tokenused = nextToken();
        }
        else {
            Errorstatement("Bracket L_BRACKET", tokenused);
        }

        // Negative Integer error
        if(match("MINUS")) {
            Errorstatement("Bracket ILLEGALMINUS",tokenused);
        }

        parseExpression();

        // Expecting ']'

        if (match("R_BRACKET")) {
            tree.insertSibling(new Node(tokenused));
        }
        else
        {
            Errorstatement("Bracket R_BRACKET", tokenused);
        }
        nextToken();
    }

        // PA3: RDP - Brace function
    // Individual function soley designed for handling braces and things in-between
    // Parameters: braceCounter, helps keep track of number of '{' there are
    // braceLocation, helps keep track of the line number of a given brace
    void parseBrace(int &braceCounter, vector<int> &braceLocation) {
        // Add some kind of node
        Token tokenused = peek();

        // Expecting a '{'
        if (match("L_BRACE")) {
            braceCounter++; // increment
            braceLocation.push_back(tokenused.getLine()); // Push to vector
            tree.insertChild(new Node(tokenused));
            tokenused = nextToken();
        }
        // Expecting '}'
       else if (match("R_BRACE")) {
            braceCounter--; // decrement
            if(braceCounter<0)
            {
                cout<<"Illegal R_BRACE without accompanying L_BRACE at line:"<<peek().getLine();
                exit(-1);
            }
            braceLocation.pop_back(); // Push to vector
            tree.insertChild(new Node(tokenused));
            tokenused = nextToken();
        }

    }


    // PA3: RDP - Semicolon function
    // Individual function soley designed for handling semicolons
    void parseSemicolon() {
        // Init token
            tree.insertSibling(new Node(peek()));
            nextToken();
    }


// hopefully handles all expressions including strings function calls operators with boolean or arithmetical operands
    void parseExpression() {

        bool loop=true;
        while(loop) {
            loop = false;
            if (match("DOUBLE_QUOTE") || match("SINGLE_QUOTE")) //string
                parseString();
            else if ( (match("MINUS") && tokens[current + 1].getType() == "INTEGER") || match("INTEGER") ) //negative integer
            {
                if(match("MINUS")) {
                    tree.insertSibling(new Node(peek()));
                    nextToken();
                }
                tree.insertSibling(new Node(peek()));
                nextToken();
            }
            else if ( ( match("BOOLEAN_NOT") && tokens[current + 1].getType() == "IDENTIFIER" && keywordcheck(tokens[current+1].getName()) == Identifier)
            || ( match("IDENTIFIER") && keywordcheck(peek().getName())==Identifier) )  // not+identifier or just identifier
            {
                if (match("BOOLEAN_NOT")) //insert ! if there is one
                {
                    tree.insertSibling(new Node(peek()));
                    nextToken();
                }

                tree.insertSibling(new Node(peek())); //insert identifier
                nextToken();

                if (match("L_PAREN")) { //insert function parameters if the identifier is a function
                    tree.insertSibling(new Node(peek()));
                    nextToken();
                    parseFunctionCallParameters();

                    if (match("R_PAREN")) {
                        tree.insertSibling(new Node(peek()));
                        nextToken();
                    } else
                        Errorstatement("ExpressionParse R_PAREN", peek());
                }
            }
            else if (match("L_PAREN"))
            {
                tree.insertSibling(new Node(peek()));
                nextToken();

                parseExpression();

                if (match("R_PAREN")) {
                    tree.insertSibling(new Node(peek()));
                    nextToken();
                } else
                    Errorstatement("ExpressionParse R_PAREN", peek());

            }
            else
                Errorstatement("ExpressionParse Operator", peek());


            if(find(operandlist.begin(),operandlist.end(),peek().getType()) != operandlist.end()) //if the token is in operandlist
            {
                loop = true;
                tree.insertSibling(new Node(peek()));
                nextToken();
            }
        }
        if( !match("R_PAREN") && !match("SEMICOLON") ) {
            Errorstatement("ExpressionParse Operand", peek());
        }
    }



    void Errorstatement(string fromwhere,Token tokenused)
    {
        cout << fromwhere <<" error on line: " << tokenused.getLine() << " Token name: "<<tokenused.getName() <<" Token type: "<<tokenused.getType() << endl;
        exit(-1);
    }

};


// Main function
int main() {
    vector<Token> tokenlist;
    string fileName ="programming_assignment_3-test_file_";
    string tokenizefile="test_file";

//    for(int i=1; i <11; i++) {
//        ignoreComments(fileName + std::to_string(i) + ".c", tokenizefile + std::to_string(i) + ".c");
//        tokenlist = Tokenize(tokenizefile + std::to_string(i) + ".c");
//        continue;
//    }

//file 5: incrorrectly identifies error as line 6, token PLUS (should be missing closing quote on next line)
//all others work as intended
    int i=4;
    ignoreComments(fileName + std::to_string(i) + ".c", tokenizefile + std::to_string(i) + ".c");
    tokenlist = Tokenize(tokenizefile + std::to_string(i) + ".c");
    Parser CST(tokenlist);
    CST.buildCST();
    cout<<"CST built successfully"<<endl;
    CST.tree.breadthFirstTraversal();

    return 0;
}
