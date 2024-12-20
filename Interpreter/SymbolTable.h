#ifndef CS460_SYMBOLTABLE_H
#define CS460_SYMBOLTABLE_H

#include<string>
#include<vector>
#include<variant>

using namespace std;

using Value = variant<int, bool, char, string, vector<int>, vector<bool>, vector<char>, vector<string>>;

struct Symbol {
    string name;
    string type;
    string datatype;
    bool isArray;
    int arraySize;
    int scope;
    Value value;

    //Parameter Information
    vector<string> parameterNames;
    vector<string> parameterDatatypes;
    vector<bool> isParameterArray;
    vector<int> parametersArraySizes;
    vector<Value> parameterValues;

    Symbol *next;
    //default constructor
    Symbol () : isArray(false), arraySize(0), scope (0), next(nullptr) {};
    //initializer constructor
    Symbol(string name, string type, string datatype, bool isArray = false, int arraySize = 0, int scope = 0)
            : name(name), type(type), datatype(datatype), isArray(isArray), arraySize(arraySize), scope(scope), next(nullptr) {};
    
    //Getters
    string getName() {return name;}
    string getType() {return type;}
    string getDatatype() {return datatype;}
    bool getIsArray() {return isArray;}
    int getArraySize() {return arraySize;}
    int getScope() {return scope;}
    Value getValue() {return value;}
    vector<Value> getParameterValues() {return parameterValues;}
    vector<string>getParameterNames(){return parameterNames;}

    //Setters
    void setName(const string& newName) {name = newName;}
    void setType(const string& newType) {type = newType;}
    void setDatatype(const string& newDatatype) {datatype = newDatatype;}
    void setIsArray(bool &newIsArray) {isArray = newIsArray;}
    void setArraySize(int newArraySize) {arraySize = newArraySize;}
    void setScope(int newScope) {scope = newScope;}
    void setValue(const Value& newValue) {value = newValue;}
    void setParameterValues(const vector<Value>& paramValues) {parameterValues = paramValues;}
};

class SymbolTable{
    Symbol* Root;
    Symbol* Tail;
    Symbol* Traversal;
    LCRSTree *CST;
public:
    SymbolTable(LCRSTree *inputtree){Root= nullptr;Tail= nullptr;Traversal= nullptr; CST=inputtree;}
    /* list of Functions inside SymbolTable
     * -----------------------------------------
     * void addSymbol(Symbol *entry)    //adds a Symbol object to the Symbol Table linked list
     * void printSymbolTable()          // prints all Symbols inside the Symbol Table()
     * void BuildTable()                //DFA that runs through the private member CST to build the Symbol Table
     * void populateDeclaredFunction(int currentScope)          //populates the Symbol table with a Function Declaration
     * void populateDeclaredvariable(int currentscope)          //populates the Symbol table with a Declared Variable
     * bool existsInTable(int currentScope, const string& name) // checks whether the Symbol already exists in Symbol Table
     * void populateDeclaredFunctionParameter(Symbol *symbol)   //Assistant Function that directly handles the parameters inside procedures and functions
     * void populateDeclaredProcedure(int currentScope)          // populates the Symbol Table with a Declared procedure
     * void assignSymbolValue(int currentscope, const string& name)             //Assigns a Symbol in table with a data value
     * void errorStatement(string fromwhere, int currentScope, Node* node)      //Assistant Function that prints an error statement called from one of the other functions and exits
     * Symbol* searchSymbol(int currentScope, const string& name)               // returns a Symbol if it exists in the Symbol Table
     * Symbol* searchSymbol(const string& name)                                 // returns a Symbol if it is a exists as a function/procedure in the Symbol Table
     * string getFuncProcName(const string& name)                               // gets functions/procedures name
     * void setFuncProcName(const string& oldName, string newName)              // sets name of a function/procedure
     * */

    void addSymbol(Symbol *entry)
    {
        if(Root== nullptr)
        {
            Root=entry;
            Tail=entry;
            Traversal=Root;
            return;
        }
        Tail->next=entry;
        Tail=entry;


    }
    void printSymbolTable()
    {
        Traversal=Root;
        while(Traversal !=nullptr)
        {
            cout<<"IDENTIFIER_NAME: "<<Traversal->name<<endl
            << "IDENTIFIER_TYPE: "<<Traversal->type<<endl
            << "DATATYPE: "<<Traversal->datatype<<endl
            <<"DATATYPE_IS_ARRAY: ";
            if(Traversal->isArray)
               {cout<<"yes";}
            else
               {cout<<"no";}
            cout<<endl
            <<"DATATYPE_ARRAY_SIZE: "<<Traversal->arraySize<<endl
            <<"SCOPE: "<<Traversal->scope<<endl<<endl;

            if(!Traversal->parameterNames.empty())
            {
                for(int i=0;i < Traversal->parameterNames.size(); i++) {
                    cout << "Parameter for: "<<Traversal->name<<endl
                    <<"IDENTIFIER_NAME: "<<Traversal->parameterNames[i]<<endl
                    << "DATATYPE: "<<Traversal->parameterDatatypes[i]<<endl
                    <<"DATATYPE_IS_ARRAY: ";
                    if(Traversal->isParameterArray[i])
                    {cout<<"yes";}
                    else
                    {cout<<"no";}
                    cout<<endl
                    <<"DATATYPE_ARRAY_SIZE: "<<Traversal->parametersArraySizes[i]<<endl
                    <<"SCOPE: "<<Traversal->scope<<endl<<endl;
                }
            }

            Traversal=Traversal->next;
        }
    }

    // DFA FOR BUILDING THE SYMBOL TABLE
    // USES LOCAL VARIABLES currentscope,bracecounter, and scopenum to track current scope (currentscope tracks how deep we are in a function scopenum tracks how many functions/procedures there are)
    //please consider 0 to be global scope when making your functions
    void BuildTable()
    {
        int currentscope =0,scopenum=0,bracecounter=0;//not finalized need to think about whether this 2 int system is good enough
        while(!CST->EOT())
        {
            Node* current=CST->getCurrentNode();
            if(current->data.getType()=="FUNCTION")
            {
                scopenum+=1;
                populateDeclaredFunction(scopenum);
            }
            else if(current->data.getType()=="PROCEDURE")
            {
                scopenum+=1;
                populateDeclaredProcedure(scopenum);
            }
            else if(current->data.getType()=="IDENTIFIER" && find(begin(typekeyword),end(typekeyword),current->data.getName()) != end(typekeyword))
            {
                populateDeclaredvariable(currentscope);
            }
            else if(current->data.getType()=="L_BRACE" )
            {
                bracecounter+=1;
                currentscope=scopenum;
                CST->nextNode();
            }
            else if(current->data.getType()=="R_BRACE")
            {
                bracecounter -=1;
                if(bracecounter == 0)
                    currentscope=0;

                CST->nextNode();
            }
            else if(existsInTable(currentscope, current->data.getName()))
            {
                assignSymbolValue(currentscope, current->data.getName());
            }
            else
                CST->nextChild(); //skips over the entire Sibling chain into the next Child
        }
    }

    // PA4: populateDeclaredFunction();
    // This function populates a parameter within a function and passes an incomplete symbol to the populateDeclaredFunctionParameter()
    // Parameters: currentScope, the scope/number the symbol table is currently in.
    void populateDeclaredFunction(int currentScope) {

        // Create a symbol
        Symbol* currentSymbol = new Symbol;
        currentSymbol->scope = currentScope;

        // At this point move down the CST and do the thing below
        // Populate current symbol with name, type, datatype, and the current scope.
        CST->nextNode(); // move past the function declaration
        currentSymbol->datatype = CST->getCurrentNode()->data.getName();
        CST->nextNode();
        currentSymbol->name = CST->getCurrentNode()->data.getName();
        CST->nextNode(); // move past its type and onto the name.
        currentSymbol->type ="FUNCTION";
        if(CST->getCurrentNode()->data.getType()=="L_PAREN") {
            CST->nextNode();
            populateDeclaredFunctionParameter(currentSymbol);
        }
        else
            errorStatement("missing L_PAREN in Function",currentScope,CST->getCurrentNode());
        addSymbol(currentSymbol);
        CST->nextNode();
    }

    void populateDeclaredvariable(int currentscope)
    {

        bool multiplevariables=true;
        string varname;
        string vartype=CST->getCurrentNode()->data.getName();
        CST->nextNode();
        while(multiplevariables)
        {
            multiplevariables=false;
            varname=CST->getCurrentNode()->data.getName();
            if(existsInTable(currentscope,varname))
                  errorStatement("DeclaredVariable alreadyexists", currentscope,CST->getCurrentNode());
            CST->nextNode();// move to the L_BRACKET OR COMMA OR SEMICOLON NODE
            if(CST->getCurrentNode()->data.getType()=="L_BRACKET")
            {
                CST->nextNode();     //moves to the arraysize node
                addSymbol(new Symbol(varname,"datatype",vartype,true,stoi(CST->getCurrentNode()->data.getName()),currentscope));
                CST->nextNode();     //move to R_BRACKET
                CST->nextNode();     //move to COMMA OR SEMICOLON NODE
            }
            else
            {
                addSymbol(new Symbol(varname,"datatype",vartype,false,0,currentscope));
            }
            if(CST->getCurrentNode()->data.getType()=="COMMA")
            {
                multiplevariables=true;
                CST->nextNode();
            }
        }
        if(CST->getCurrentNode()->data.getType()=="SEMICOLON")
        {
            CST->nextNode();
        }
        else
        {
            cout<<"THERE IS SUPPOSED TO BE A SEMICOLON HERE.  INSTEAD IT'S A "<<CST->getCurrentNode()->data.getName() <<" TOKEN ON LINE: "<<CST->getCurrentNode()->data.getLine();
            exit(41);
        }
    }

    // assigns the first node after ASSIGNMENT_OPERATOR, skips to SEMICOLON after first node
    // sum = 1; sum = n; WORKS
    // sum = n + 1;  Assigns sum's value to the value of n and skips to SEMICOLON
    // sum = -1; DOESN'T WORK
    void assignSymbolValue(int currentscope, const string& name)
    {
        CST->nextNode(); // moves to ASSIGNMENT_OPERATOR
        
        Symbol* localSymbol = searchSymbol(currentscope, name);
        string vartype = localSymbol->getDatatype();
        
        // on ASSIGNMENT_OPERATOR assign value to Symbol accordingly
        if(CST->getCurrentNode()->data.getType()=="ASSIGNMENT_OPERATOR") {
            if (vartype == "int")
            {
                CST->nextNode(); // move to integer
                if(CST->getCurrentNode()->data.getType() == "MINUS") // sets value to be negative
                {
                    CST->nextNode();

                    localSymbol->setValue(-stoi(CST->getCurrentNode()->data.getName()));
                }
                else if(existsInTable(currentscope, CST->getCurrentNode()->data.getName())) // if currentNode is in SymbolTable, set value from table
                {
                    Symbol *tempSymbol = searchSymbol(currentscope, CST->getCurrentNode()->data.getName());
                    localSymbol->setValue(tempSymbol->getValue());
                }
                else if (CST->getCurrentNode()->data.getType() == "IDENTIFIER") // skips function calls
                {
                    CST->nextNode();
                }
                else
                {
                    localSymbol->setValue(stoi(CST->getCurrentNode()->data.getName()));
                }
            }
            else if (vartype == "bool")
            {
                CST->nextNode(); //move to boolean value
                if (CST->getCurrentNode()->data.getName() == "true")
                {
                    localSymbol->setValue(true);
                }
                else
                {
                    localSymbol->setValue(false);
                }

            }
            else if (vartype == "char")
            {
                vector<char> string;
                CST->nextNode(); // moves to opening SINGLE_QUOTE
                CST->nextNode(); // moves to string
                // stores characters from string in vector
                for (auto &c : CST->getCurrentNode()->data.getName())
                {
                    if (c == '\\') break; // breaks on any occurrence of '\'
                    string.push_back(c);
                }
                localSymbol->setValue(string);
                CST->nextNode(); // moves to closing SINGLE_QUOTE
            }

            CST->nextNode();
            if(CST->getCurrentNode()->data.getType()=="SEMICOLON")
            {
                CST->nextNode();
            }
            else
            {
                CST->nextNode();
                while(CST->getCurrentNode()->data.getType() != "SEMICOLON")
                {
                    CST->nextNode();
                }
                CST->nextNode();
            }
        }
    }

    // TODO: Finish me! I'm currently a skeleton!
    // PA4: errorStatement()
    // Prints out and error statement to console and specifies what scope and type the node was.
    // Parameters copy existsInTable() function for now. fromwhere specifies what function it is from i.e. 
    // "populateDeclaredFunction". 
    void errorStatement(string fromwhere, int currentScope, Node* node) {
        cout << fromwhere << " error with " << " Scope was: "
        << currentScope << " variable was: " << node->data.getType()<<" "<<node->data.getName()<<" on line "<<node->data.getLine() << endl;
        exit(41);
    }

    //checks if an identifier exists within the symbol table
    //assumes no duplicate function names, so this only works on variables
    //call this when the current CST node is the identifier's name
    bool existsInTable(int currentScope, const string& name) {
        Symbol* currentSymbol = Root;

        while (currentSymbol!= nullptr) { //loop through the entire Symbol Table

            if (currentSymbol->type == "datatype") {
                //name matches an identifier that exists in global or same scope
                if (name == currentSymbol->name &&
                    (currentSymbol->scope == 0 || currentSymbol->scope == currentScope)) {
                    return true;
                }
            }
            else {
                if(!currentSymbol->parameterNames.empty()) { //checking current CST node against current Symbol's parameters, if any
                    for (const auto &i: currentSymbol->parameterNames) {
                        if (name == i &&
                            (currentSymbol->scope == 0 || currentSymbol->scope == currentScope)) {
                            return true;
                        }
                    }

                }
            }

            currentSymbol = currentSymbol->next;
        }

        return false;
    }


    void populateDeclaredProcedure(int currentScope) {
        CST->nextNode();
        Node* currentNode = CST->getCurrentNode();
        // Extract procedure name and data type

        string procedureName = currentNode->data.getName();
        string procedureType = "PROCEDURE"; // To keep type consistent
        string procedureDataType = "void";  // Default to void for procedures

        // Create a new Symbol entry for the procedure with basic details
        Symbol* newProcedure = new Symbol(procedureName, procedureType, procedureDataType, false, 0, currentScope);

        // Move to the next node to check for parameter list (i.e., an open parenthesis)
        CST->nextNode();
        currentNode = CST->getCurrentNode();
        if (currentNode->data.getType() != "L_PAREN") {
            errorStatement("Expected '(' after procedure name for parameter list.",currentScope,currentNode);
        }

        // Populate parameters by passing the symbol to PopulateDeclaredFunctionParameter
        CST->nextNode(); // Move inside the parentheses
        populateDeclaredFunctionParameter(newProcedure);

        // Add the newly populated procedure entry to the symbol table
        addSymbol(newProcedure);
        CST->nextNode();
    }

    void populateDeclaredFunctionParameter(Symbol *symbol) {
        bool multipleParameters = true;

        //Returns if no parameters
        if (CST->getCurrentNode()->data.getType() == "R_PAREN") {
            return;
        }

        //Moves past loop if parameter is void
        if (CST->getCurrentNode()->data.getName() == "void") {
            multipleParameters = false;
            CST->nextNode();
        }

        while (multipleParameters) {
            multipleParameters = false;
            bool isArray = false;
            int arraySize = 0;

            string paramType = CST->getCurrentNode()->data.getName();
            CST->nextNode();

            string paramName = CST->getCurrentNode()->data.getName();
            CST->nextNode();

            //Updates variables if parameter is an array
            if (CST->getCurrentNode()->data.getType() == "L_BRACKET") {
                isArray = true;
                CST->nextNode(); //Moves to array size
                arraySize = stoi(CST->getCurrentNode()->data.getName());
                CST->nextNode(); //Moves to R_BRACKET
                CST->nextNode(); //Moves to COMMA or R_PAREN
            }

            //Adds parameters to parent Function/Procedure SymbolTable
            symbol->parameterNames.push_back(paramName);
            symbol->parameterDatatypes.push_back(paramType);
            symbol->isParameterArray.push_back(isArray);
            symbol->parametersArraySizes.push_back(arraySize);

            //Loops again if true
            if (CST->getCurrentNode()->data.getType() == "COMMA") {
                multipleParameters = true;
                CST->nextNode();
            }
        }

        //Returns error if not on R_PAREN
        if(CST->getCurrentNode()->data.getType() != "R_PAREN") {

            cout << "THERE IS SUPPOSED TO BE A RIGHT PARENTHESIS HERE.  INSTEAD IT'S A "
                 << CST->getCurrentNode()->data.getName() << " TOKEN ON LINE: "
                 << CST->getCurrentNode()->data.getLine();
            exit(41);
        }
    }

    // works the same as the function existsInTable but returns the found Symbol
    Symbol* searchSymbol(int currentScope, const string& name) {
        Symbol* currentSymbol = Root;

        while (currentSymbol!= nullptr) { //loop through the entire Symbol Table

            if (currentSymbol->type == "datatype") {
                //name matches an identifier that exists in global or same scope
                if (name == currentSymbol->name &&
                    (currentSymbol->scope == 0 || currentSymbol->scope == currentScope)) {
                    return currentSymbol;
                }
            }
            else {
                if(!currentSymbol->parameterNames.empty()) { //checking current CST node against current Symbol's parameters, if any
                    for (const auto &i: currentSymbol->parameterNames) {
                        if (name == i &&
                            (currentSymbol->scope == 0 || currentSymbol->scope == currentScope)) {
                            return currentSymbol;
                        }
                    }
                }
            }

            currentSymbol = currentSymbol->next;
        }

        return nullptr;
    }

    // searches for functions and procedures
    Symbol* searchSymbol(const string& name) {
        Symbol* currentSymbol = Root;

        while (currentSymbol!= nullptr) { //loop through the entire Symbol Table
            // name matches an identifier and type is a function or procedure
            if (currentSymbol->name == name && (currentSymbol->type == "FUNCTION" || currentSymbol->type == "PROCEDURE")) {
                return currentSymbol;
            }

            currentSymbol = currentSymbol->next;
        }

        return nullptr;
    }

    string getFuncProcName(const string& name) {
        Symbol* currentSymbol = searchSymbol(name);
        if (currentSymbol != nullptr) {
            return currentSymbol->name;
        }
        cerr << "Error: Function/Procedure " << name << " not found\n";
        return "";
    }

    void setFuncProcName(const string& oldName, string newName) {
        Symbol* currentSymbol = searchSymbol(oldName);
        if (currentSymbol != nullptr) {
            currentSymbol->name = newName;
            return;
        }
        cerr << "Error: Function/Procedure " << oldName << " not found\n";
    }


};

#endif //CS460_SYMBOLTABLE_H
