#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <sstream>
#include <set>
#include <map>
#include <queue>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

struct Symbol {
public:
    enum EID {ID, ARE, HAVE, CAN, WITH, AND, THAT_CAN, ERROR };
    static string toString(EID id) { 
        static const vector<string> ids= { "ID", "ARE", "HAVE", "CAN", "WITH", "AND", "THAT_CAN", "ERROR"  };
        return ids[id];
    }
    EID id;
    string content;
};

struct Lexer {
    string _text;
    Symbol _current;
    void setText(const string &text) { _text = text; _current = {Symbol::ERROR}; }
    const Symbol &getSymbol() const { return _current; };
    Symbol nextSymbol() {
        int pos = 0;
        _current = { Symbol::ERROR, ""};
        while (_text.size() > 0 && _text[0]==' ') { _text.erase(0,1); } //< SKIP SPACE
        if (_text.size() <= 0) { return _current; }; //< NO MORE CARS
        if (_text[0] >= 'A' && _text[0] <= 'Z') { //< [A-Z]* : id
            pos = 0;
            _current.id = Symbol::ID;
            while (pos < _text.size() && _text[pos] >= 'A' && _text[pos] <= 'Z') {
                _current.content.push_back(_text[pos++]);
            }            
        } else if ( (_text.compare(0,pos =(_current.content = string("are")) .size(),_current.content) ) == 0 ) {
            _current.id = Symbol::ARE;
        } else if ( (_text.compare(0,pos =(_current.content = string("have")) .size(),_current.content) ) == 0 ) {
            _current.id = Symbol::HAVE;
        } else if ( (_text.compare(0,pos =(_current.content = string("can")) .size(),_current.content) ) == 0 ) {
            _current.id = Symbol::CAN;
        } else if ( (_text.compare(0,pos =(_current.content = string("with")) .size(),_current.content) ) == 0 ) {
            _current.id = Symbol::WITH;
        } else if ( (_text.compare(0,pos =(_current.content = string("and")) .size(),_current.content) ) == 0 ) {
            _current.id = Symbol::AND;
        } else if ( (_text.compare(0,pos =(_current.content = string("that can")) .size(),_current.content) ) == 0 ) {
            _current.id = Symbol::THAT_CAN;
        }
        _text.erase(0,pos);
        return _current;
    }
};

struct ClassQuery {
    string name;
    set<string> methods;
    set<string> properties;
    bool isSimple() { return methods.size() == 0 && properties.size() == 0; }
};

struct ClassNode {
    string name;
    set<string> parents;
    set<string> children;
    set<string> methods;
    set<string> properties;
    set<string> owners;

    ClassNode() {}
    ClassNode(const string &name_) { name = name_; }

    bool hasInterface(const set<string> &method_set) {
        for (auto method_name:method_set) {
            if (methods.find(method_name) == methods.end()) return false;
        }
        return true;
    }

    bool hasProperties(const set<string> &property_set) {
        for (auto property_name:property_set) {
            if (properties.find(property_name) == properties.end()) return false;
        }
        return true;
    }

    friend ostream &operator << (ostream &, const ClassNode &);
};
ostream &operator <<(ostream &os, const ClassNode &cl) {
    os << cl.name;
    os << ", m[";
    for (auto parent:cl.parents) {
        os << " " << parent;
    }
    os <<" ], c[";
    for (auto child:cl.children) {
        os << " " <<  child;
    }
    os <<" ], p[";
    for (auto property:cl.properties) {
        os << " " <<  property;
    }
    os << " ], i[";
    for (auto method:cl.methods) {
        os << " " <<  method;
    }
    os << " ], o[";
    for (auto owner:cl.owners) {
        os << " " <<  owner;
    }
    os <<" ]";
    return os;
}

struct ClassDiagram {
    map<string,shared_ptr<ClassNode>> dictionary;

    shared_ptr<ClassNode> find(const string &name) {
        auto found_parent = dictionary.find(name);
        if (found_parent == end(dictionary) || !(*found_parent).second) { return {}; }
        return (*found_parent).second;
    }
    shared_ptr<ClassNode> find(const ClassQuery &query) {
        auto found_parent = find(query.name);
        if (!found_parent) return {};
        queue<shared_ptr<ClassNode>> look_for;
        look_for.push(found_parent);
        while (look_for.size() > 0) {
            shared_ptr<ClassNode> current = look_for.front();
            look_for.pop();
            if (current->hasInterface(query.methods) && current->hasProperties(query.properties)) {
                return current;
            } else {
                for (auto child:current->children) {

                    look_for.push(dictionary[child]);
                }
            }
        }
        return {};
    }
    shared_ptr<ClassNode> createNode(const string &name) {
        shared_ptr<ClassNode> node(new ClassNode(name));
        dictionary[name] = node;
        return node;
    } 

    void createInheritance( shared_ptr<ClassNode> parent_node, shared_ptr<ClassNode> child_node)  {
        if (parent_node && child_node) {
            parent_node->children.insert(child_node->name);
            child_node->parents.insert(parent_node->name);
            for (auto method_name:parent_node->methods) {
                addMethod(child_node, method_name);
            }
            for (auto property_name:parent_node->properties) {
                shared_ptr<ClassNode> property_node = dictionary[property_name];                
                addProperty(child_node,property_node);
            }
            for (auto owner_name:child_node->owners) {
                shared_ptr<ClassNode> owner_node = dictionary[owner_name];
                addProperty(owner_node, parent_node);
            }
        }        
    }

    void addProperty( shared_ptr<ClassNode> class_node, shared_ptr<ClassNode> property_node) {
        if (class_node && property_node) {
            class_node->properties.insert(property_node->name);
            property_node->owners.insert(class_node->name);
            for (auto child_name:class_node->children) {
                shared_ptr<ClassNode> child_node = dictionary[child_name];
                addProperty(child_node, property_node);
            } 
            for (auto property_parent_name:property_node->parents) {
                shared_ptr<ClassNode> property_parent_node = dictionary[property_parent_name];
                addProperty(class_node, property_parent_node);
            }
        }
    }

    void addMethod(shared_ptr<ClassNode> class_node, const string &method_name) {
        if (class_node) {
            class_node->methods.insert(method_name);
            for (auto child_name:class_node->children) {
                shared_ptr<ClassNode> child_node = dictionary[child_name];
                addMethod(child_node, method_name);
            } 
        }
    }
};

struct Parser
{
    bool accept(Lexer &lexer,  Symbol &symbol) {
        if (lexer.getSymbol().id == symbol.id) {
            symbol.content = lexer.getSymbol().content;
            return true;
        }
        else return false;
    }

    bool parse(Lexer &lexer, ClassDiagram& graph, const string &text) {
        lexer.setText(text);
        return assertion(lexer,graph);
    }

    bool assertion(Lexer &lexer, ClassDiagram& graph) {
        //cerr << "assertion" << endl;
        ClassQuery class_query = fullId(lexer);
        shared_ptr<ClassNode> class_node = graph.find(class_query);
        if (!class_node && class_query.isSimple()) {
            class_node = graph.createNode(class_query.name); 
        }
        if (!class_node) return false;
        Symbol symbol;
        if ( (symbol.id = Symbol::ARE) && accept(lexer, symbol) ) { 
            ClassQuery parent_query = inherit(lexer);
            shared_ptr<ClassNode> parent_node = graph.find(parent_query);
            if (!parent_node) {
                parent_node = graph.find(parent_query.name);
            }
            if (!parent_node) {
                parent_node = graph.createNode(parent_query.name); 
            }
            graph.createInheritance(parent_node, class_node);
            for (auto property_name:parent_query.properties) {
                shared_ptr<ClassNode> property_node = graph.find(property_name);
                if (!property_node) {
                    property_node = graph.createNode(property_name);                
                }
                graph.addProperty(class_node,property_node);
            }
            for (auto method_name:parent_query.methods) {
                graph.addMethod(class_node, method_name );
            }

        } else if ( (symbol.id = Symbol::HAVE) && accept(lexer, symbol) ) {
            string property_name = property(lexer);
            shared_ptr<ClassNode> property_node = graph.find(property_name);
            if (!property_node) {
                property_node = graph.createNode(property_name); 
            }
            graph.addProperty(class_node, property_node);
        } else if ( (symbol.id = Symbol::CAN) && accept(lexer, symbol) ) {
            string method_name = method(lexer);
            graph.addMethod(class_node, method_name);
        }
        return true;
    } 
    
    ClassQuery inherit(Lexer &lexer) { 
        //cerr << "inherit" << endl;
         return  fullId(lexer);
    } 

    string property(Lexer &lexer) {
        //cerr << "property" << endl; 
        return  id(lexer);
    }
    string method(Lexer &lexer) {
        //cerr << "method" << endl;
        return id(lexer);
    }
    ClassQuery fullId(Lexer &lexer) {
        //cerr << "fullid" << endl;
        //conjunction.clauses.insert(begin(conjunction.clauses), base.getRegisteredClause({Clause::ID,id(lexer)}));
        ClassQuery query;
        query.name = id(lexer);
        lexer.nextSymbol();
        Symbol symbol;
        if ( (symbol.id = Symbol::WITH) && accept(lexer,symbol) ) {
            vector<string> property_names = properties(lexer);
            for (auto property_name:property_names) {
                query.properties.insert(property_name);
            }
        }
        if ( (symbol.id = Symbol::THAT_CAN) && accept(lexer, symbol) ) {
            vector<string> method_names = interface(lexer);
            for (auto method_name:method_names) {
                query.methods.insert(method_name);
            }
        }
        return query;
    }

    vector<string> properties(Lexer &lexer) {
        vector<string> property_names;
        property_names.push_back(id(lexer));
        lexer.nextSymbol();
        Symbol symbol;
        while ( (symbol.id = Symbol::AND) && accept(lexer,symbol) ) {
            //conjunction.clauses.insert(begin(conjunction.clauses), base.getRegisteredClause({Clause::METHOD, id(lexer)}));
            property_names.push_back(id(lexer));
            lexer.nextSymbol();
        }
        return property_names;
    }

    vector<string> interface(Lexer &lexer) {
        //cerr << "interface" << endl;
        //conjunction.clauses.insert(begin(conjunction.clauses), base.getRegisteredClause({Clause::METHOD, id(lexer)}));
        vector<string> method_names;
        method_names.push_back(id(lexer));
        lexer.nextSymbol();
        Symbol symbol;
        while ( (symbol.id = Symbol::AND) && accept(lexer,symbol) ) {
            //conjunction.clauses.insert(begin(conjunction.clauses), base.getRegisteredClause({Clause::METHOD, id(lexer)}));
            method_names.push_back(id(lexer));
            lexer.nextSymbol();
        }
        return method_names;
    }

    string id(Lexer &lexer) {
        lexer.nextSymbol();
        Symbol symbol;
        symbol.id = Symbol::ID;
        accept(lexer,symbol);
        //cerr << "id" << " "<< symbol.content << endl;
        return symbol.content;
    }
};


int main()
{
    Lexer lexer;
    Parser parser;
    ClassDiagram graph;
    /*
    int N;
    cin >> N; cin.ignore();

    for (int i = 0; i < N; i++) {
        string S;
        getline(cin, S);
        text.push_back(S);
        cerr << S << endl;
    }
    */
    vector<vector<string>> tests =
    {
        {
            "PIGS are BACONS",
            "BACONS are GODS",
            "GODS can FLY"
        },
        {
            "PIGS have FEET",
            "PIGS are ANIMALS",
            "ANIMALS with FEET that can RUN can FLY",
            "ANIMALS can RUN"
        },
        {
            "BACONS with WINGS can FLY",
            "HAMS are PIGS",
            "HAMLETS are PIGLETS",
            "BACONS are GODS",
            "PIGLETS are PIGS",
            "HAMS are BACONS",
            "GODS have WINGS"            
        },
        {
            "PIGS have FEET",
            "FEET are LIMBS",
            "CATS are POPTARTS",
            "CATS have LIMBS",
            "PIGS are MAMMALS",
            "CATS are MAMMALS",
            "POPTARTS with LIMBS can FLY",
            "CATS are PIGS"
        },
        {
            "PIGS have MOUSTACHES",
            "ANIMALS with MOUSTACHES can FLY",
            "SQUIRRELS have MOUSTACHES",
            "SQUIRRELS can SCREECH",
            "SQUIRRELS are PIGS",
            "REINDEER are SQUIRRELS",
            "REINDEER are MAMMALS",
            "MAMMALS that can SCREECH are ANIMALS",
            "PIGS are ANIMALS",
        },
        {
            "BANANAS with HANDS are BUNNIES with LIMBS",
            "BANANAS have HANDS",
            "BUNNIES with LIMBS can FLY",
            "PIGS are BUNNIES",
        },
        {
            "COWS are BURGERS",
            "COWS can EAT",
            "COWS that can MOO can FLY",
            "PIGS are COWS",
            "PIGLETS are PIGS",
            "PIGLETS are MUSHROOMS",
            "MUSHROOMS have SPORES",
            "SPORES are ROCKETSHIPS",
            "PIGLETS with ROCKETSHIPS can MOO",
        },
        {
            "BONES are ITEMS",
            "METALS are ITEMS",
            "ITEMS have COST",
            "ITEMS have DEMAND",
            "BLACKSMITHS can CRAFT",
            "BLACKSMITHS have MATERIALS",
            "PIGS have ARMOR",
            "BLACKSMITHS with HAMMERS can SYNTHESIZE",
            "BEETLES have HUSKS",
            "BEETLES are INSECTS",
            "INSECTS with HUSKS can FLY",
        },
        {
            "GEESE are CHICKENS",
            "CHICKENS with BEAKS are LLAMAS with MOUTHS",
            "MOUTHS are HOLES",
            "GEESE have BEAKS",
            "LLAMAS with TOENAILS are PIGS",
            "PIGS are TREES that can WALK",
            "CHICKENS with EYES and TOENAILS are TREES that can FLY",
            "BEAKS are TOENAILS",
            "CHICKENS have EYES",
        },
    };
    queue<string> text_queue;
    auto test = tests[8];
    sort(test.begin(),test.end(),[](auto a, auto b) { return a.size() < b.size() ;}); 
    for (auto item : test ) {
        text_queue.push(item);
    }
    set<string> redo;
    while (text_queue.size() > 0) {
        string text = text_queue.front();
        text_queue.pop();
        if (!parser.parse(lexer,graph,text) && redo.find(text) != end(redo) ) {
            text_queue.push(text);
            redo.insert(text);
        } else {
            redo.clear();
        }        
    }
    for (auto item:graph.dictionary) {
        cerr << item.first << "=>" << (*item.second) << endl;        
    }

    ClassQuery query;
    query.name = "PIGS";
    query.methods.insert("FLY");
    shared_ptr<ClassNode> result = graph.find(query);
    if (!result) {
        cout << "No pigs can fly" << endl;
    } else if (result->name != "PIGS") {
        cout << "Some pigs can fly" << endl;
    } else {
        cout << "All pigs can fly" << endl;
    }
}