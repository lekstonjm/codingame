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
    vector<string> methods;
    vector<string> properties;
};

struct ClassNode {
    string name;
    vector<weak_ptr<ClassNode>> parents;
    vector<weak_ptr<ClassNode>> children;
    vector<string> methods;
    vector<weak_ptr<ClassNode>> properties;
    vector<weak_ptr<ClassNode>> owner;

    ClassNode() {}
    ClassNode(const string &name_) { name = name_; }

    bool hasParent(weak_ptr<ClassNode> node) {
        return find_if(begin(parents), end(parents),[&](auto parent_node) {return parent_node.lock()->name == node.lock()->name;}) != end(parents);
    }
    bool hasChild(weak_ptr<ClassNode> node) {
        return find_if(begin(children), end(children),[&](auto child_node) {return child_node.lock()->name == node.lock()->name;}) != end(children);
    }
    bool hasProperty(weak_ptr<ClassNode> node) {
        return find_if(begin(properties), end(properties),[&](auto property_node) {return property_node.lock()->name == node.lock()->name;}) != end(properties);
    }
    bool hasProperties(vector<weak_ptr<ClassNode>> nodes) {
        for (auto node:nodes) {
            if (!hasProperty(node)) return false;
        }
        return true;
    }
    bool hasProperty(weak_ptr<ClassNode> node) {
        return find_if(begin(properties), end(properties),[&](auto property_node) {return property_node.lock()->name == node.lock()->name;}) != end(properties);
    }

    bool hasProperty(const string &property_name) {
        return find_if(begin(properties), end(properties),[&](auto property_node) {return property_node.lock()->name == property_name;}) != end(properties);        
    }
    bool hasProperties(const vector<string> &name) {
        for (auto property_name:properties) {
            if (!hasProperty(property_name)) return false;
        }
        return true;
    }

    bool hasMethod(const string &method) {
        return find(begin(methods),end(methods),method) != end(methods);
    }
    bool hasInterface(const vector<string> &methods_) {
        for (auto method:methods_) {
            if (!hasMethod(method)) return false;
        }
        return true;
    }

    void addParent(weak_ptr<ClassNode> node) {
        if (!hasParent(node)) { parents.push_back(node); }
    }
    void addChild(weak_ptr<ClassNode> node) {
        if (!hasChild(node)) { children.push_back(node); }
        for(auto property:properties) {
            node.lock()->addProperty(property);
        }
        for (auto method:methods) {
            node.lock()->addMethod(method);
        }
    }
    void addProperty(weak_ptr<ClassNode> node) {
        if (!hasProperty(node)) { properties.push_back(node); }
        for(auto child:children) { child.lock()->addProperty(node); }
    }
    void addMethod(const string &method) {
        if (!hasMethod(method)) { methods.push_back(method); }
        for(auto child:children) { child.lock()->addMethod(method); }
    }
    friend ostream &operator << (ostream &, const ClassNode &);
};
ostream &operator <<(ostream &os, const ClassNode &cl) {
    os << cl.name;
    os << ", m[";
    for (auto parent:cl.parents) {
        os << " " << parent.lock()->name;
    }
    os <<" ], c[";
    for (auto child:cl.children) {
        os << " " <<  child.lock()->name;
    }
    os <<" ], p[";
    for (auto property:cl.properties) {
        os << " " <<  property.lock()->name;
    }
    os << " ], i[";
    for (auto method:cl.methods) {
        os << " " <<  method;
    }
    os <<" ]";
    return os;
}

struct ClassDiagram {
    map<string,shared_ptr<ClassNode>> dictionary;
     weak_ptr<ClassNode> find(const ClassQuery &query) {
        auto found_parent = dictionary.find(query.name);
        if (found_parent == end(dictionary) || !(*found_parent).second) { return {}; }
        queue<weak_ptr<ClassNode>> look_for;
        look_for.push((*found_parent).second);
        while (look_for.size() > 0) {
            weak_ptr<ClassNode> current = look_for.back();
            look_for.pop();
            if (current.lock()->hasInterface(query.methods) && current.lock()->hasProperties(query.properties)) {
                return current;
            } else {
                for (auto child:current.lock()->children) {
                    look_for.push(child);
                }
            }
        }
        return {};
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
        ClassNode class_query = fullId(lexer,graph);        
        Symbol symbol;
        if ( (symbol.id = Symbol::ARE) && accept(lexer, symbol) ) { 
            weak_ptr<ClassNode> parent_node = inherit(lexer,graph);
            if (parent_node.expired()) {
                return false;
            }
            weak_ptr<ClassNode> class_node = graph.find(class_query);
            if (class_node.expired()) { return false; }
            class_node.lock()->addParent(parent_node);
            parent_node.lock()->addChild(class_node);
        } else if ( (symbol.id = Symbol::HAVE) && accept(lexer, symbol) ) {
            weak_ptr<ClassNode> class_node = graph.findOrCreate(class_query);
            weak_ptr<ClassNode> property_node = property(lexer,graph);
            class_node.lock()->addProperty(property_node);
        } else if ( (symbol.id = Symbol::CAN) && accept(lexer, symbol) ) {
            weak_ptr<ClassNode> class_node = graph.findOrCreate(class_query);
            class_node.lock()->addMethod(method(lexer));
        }
        return true;
    } 
    
    weak_ptr<ClassNode> inherit(Lexer &lexer, ClassDiagram &graph) { 
        //cerr << "inherit" << endl;
        ClassNode class_query = fullId(lexer,graph);
        weak_ptr<ClassNode> class_node = graph.findOrCreate(class_query);
        return class_node;
    } 

    weak_ptr<ClassNode> property(Lexer &lexer, ClassDiagram &graph) {
        //cerr << "property" << endl; 
        string name = id(lexer);
        ClassNode query(name);
        return  graph.findOrCreate(query);
    }
    string method(Lexer &lexer) {
        //cerr << "method" << endl;
        return id(lexer);
    }
    ClassQuery fullId(Lexer &lexer, ClassDiagram &graph) {
        //cerr << "fullid" << endl;
        //conjunction.clauses.insert(begin(conjunction.clauses), base.getRegisteredClause({Clause::ID,id(lexer)}));
        ClassQuery query;
        query.name = id(lexer);
        lexer.nextSymbol();
        Symbol symbol;
        if ( (symbol.id = Symbol::WITH) && accept(lexer,symbol) ) {
            vector<string> property_names = properties(lexer,graph);
            query.properties =  property_names;
        }
        if ( (symbol.id = Symbol::THAT_CAN) && accept(lexer, symbol) ) {
            vector<string> method_names = interface(lexer);
            query.methods = method_names;
        }
        return query;
    }

    vector<string> properties(Lexer &lexer, ClassDiagram &graph) {
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
    auto test = tests[0];
    sort(test.begin(),test.end()); 
    for (auto item : test ) {
        text_queue.push(item);
    }
    while (text_queue.size() > 0) {
        string text = text_queue.front();
        text_queue.pop();
        if (!parser.parse(lexer,graph,text)) {
            text_queue.push(text);
        }
    }
    for (auto item:graph.dictionary) {
        cerr << item.first << "=>" << (*item.second) << endl;        
    }

    ClassNode query("PIGS");
    query.addMethod("FLY");
    weak_ptr<ClassNode> result = graph.find(query);
    if (result.expired()) {
        cout << "No pigs can fly" << endl;
    } else if (result.lock()->name != "PIGS") {
        cout << "Some pigs can fly" << endl;
    } else {
        cout << "All pigs can fly" << endl;
    }
}