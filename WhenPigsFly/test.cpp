#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <sstream>
#include <set>
#include <map>

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

class Lexer {
private:
    string _text;
    Symbol _current;
public:
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

struct Conjunction {
    set<int> clauses;
    void insert(const Conjunction &fusion) { clauses.insert(fusion.clauses.begin(), fusion.clauses.end());}
    void insert(unsigned char clause) {if (!contains(clause)) clauses.insert(clause); }
    void replace(const set<int> &from_clauses, const set<int> &to_clauses) {
        for (auto clause:from_clauses) {
            clauses.erase(clause);
        }
        for (auto clause:to_clauses) {
            clauses.insert(clause);
        }        
    }
    bool containsAll(const set<int> &all_clauses) const {
        for (auto clause:all_clauses) {
            if (!contains(clause)) return false;
        }
        return true;
    }
    bool contains(unsigned int clause) const  {
        return clauses.find(clause) != end(clauses);
    }
    string toString(const vector<string> &dictionary) const {
        stringstream builder;
        bool first = true;
        for (auto clause:clauses) {
            if (!first) builder << " ^ ";
            builder << dictionary[clause];
            first = false; 
        }
        return builder.str();        
    }
    friend bool operator < (const Conjunction &c1, const Conjunction &c2);
    friend bool operator==(const Conjunction &c1, const Conjunction &c2);
    friend ostream& operator<<(ostream& os, const Conjunction& conjunction);
};

bool operator <(const Conjunction &c1, const Conjunction &c2) {
    return c1.clauses < c2.clauses;
}

bool operator==(const Conjunction &c1, const Conjunction &c2) {
    return !(c1 < c2 || c2 < c1);
}

ostream &operator<<(ostream& os, const Conjunction& conjunction) {
    bool first = true;
    for (auto clause:conjunction.clauses) {
        if (!first) os << " ^ ";
        os << clause;
        first = false; 
    }
    return os;
}

struct Rule {
    Conjunction premice;
    Conjunction conclusion; 
    
    string toString(const vector<string> &dictionary) const {
        stringstream builder;
        builder << premice.toString(dictionary) << "=>"<< conclusion.toString(dictionary);
        return builder.str();
    }    
    friend bool operator<(const Rule& r1, const Rule &r2);
    friend bool operator==(const Rule& r1, const Rule &r2);
    friend ostream& operator<<(ostream& os, const Rule& rule);
};
bool operator<(const Rule& r1, const Rule &r2) {
    return r1.premice < r2.premice || ( r1.premice == r2.premice && r1.conclusion < r2.conclusion); 
}
bool operator==(const Rule& r1, const Rule &r2) {
    return r1.premice == r2.premice && r1.conclusion == r2.conclusion;
}
ostream &operator<<(ostream& os, const Rule& rule) {
    os <<rule.premice<< "=>"<<rule.conclusion;
    return os;
}

class Inference {
public:
    bool  createInclusion(const Rule &r1, const Rule &r2, Rule &r) {
        r = { };
        if (r1.conclusion.containsAll(r2.premice.clauses)) {
            r = r1;
            r.conclusion.replace(r2.premice.clauses, r2.conclusion.clauses);
            return true;
        } else { 
            return false;
        }
    }
    bool createIntersection(const Rule &r1, const Rule &r2, Rule &r) {
        r = { };
        if (r1.premice.containsAll(r2.premice.clauses))  {
            r = r1;
            r.conclusion.insert(r2.conclusion);
            return true;
        } else {
            return false;
        }
        
    }
};


class KnowledgeBase {
public:
    set<Rule> _base;
    set<Rule> _infered_rules;
    vector<string> _clauses;

    bool alreadyKnown(const Rule &rule1) {
        bool in_base =   _base.find(rule1) != _base.end();
        bool in_infered = _infered_rules.find(rule1) != _infered_rules.end();
        return in_base || in_infered;
    }    
    void add(const vector<Rule> &rules) {
        for (auto rule:rules) {
            add(rule);
        }
    }
    void add(const Rule &rule) {
        Inference inference;
        //cerr << "add: " << rule.toString() <<  endl;
        if (!alreadyKnown(rule)) {
            _infered_rules.insert(rule);        
        }
        while(_infered_rules.size() > 0)   {
            Rule current_rule = (*_infered_rules.begin());          
            ///cerr <<"infer: " << current_rule.toString() << endl;            
            for( auto base_rule:_base) {
                Rule new_rule;
                if (inference.createInclusion(base_rule,current_rule, new_rule) && !alreadyKnown(new_rule)) {
                    //cerr << current_rule.toString() << " & " << base_rule.toString() <<  " -> " << new_rule.toString() << endl;
                    _infered_rules.insert(begin(_infered_rules),new_rule);
                }
                if (inference.createInclusion(current_rule,base_rule, new_rule) && !alreadyKnown(new_rule)) {
                    //cerr << current_rule.toString() << " & " << base_rule.toString() <<  " -> " << new_rule.toString() << endl;
                    _infered_rules.insert(begin(_infered_rules),new_rule);
                }
                if (inference.createIntersection(base_rule,current_rule, new_rule) && !alreadyKnown(new_rule)) {
                    //cerr << current_rule.toString() << " & " << base_rule.toString() <<  " -> " << new_rule.toString() << endl;
                    _infered_rules.insert(begin(_infered_rules),new_rule);
                }
                if (inference.createIntersection(current_rule,base_rule, new_rule) && !alreadyKnown(new_rule)) {
                    //cerr << current_rule.toString() << " & " << base_rule.toString() <<  " -> " << new_rule.toString() << endl;
                    _infered_rules.insert(begin(_infered_rules),new_rule);
                }
            }
            _infered_rules.erase(current_rule);
            _base.insert(current_rule);
        } ;
    }
    bool queryAll(const Rule &query_rule) {
        for (auto rule:_base) {
            if ( rule == query_rule ) {
                return true;
            }
        }
        return false;
    }
    bool querySome(const Rule &query_rule) {
        for (auto rule:_base) {            
            if (rule.conclusion.containsAll(query_rule.premice.clauses)&&rule.conclusion.containsAll(query_rule.conclusion.clauses)) return true;
        }
        return false;
    }

};

class Parser
{
public:
    bool accept(Lexer &lexer,  Symbol &symbol) {
        if (lexer.getSymbol().id == symbol.id) {
            symbol.content = lexer.getSymbol().content;
            return true;
        }
        else return false;
    }

    Rule parse(Lexer &lexer, vector<string>& dictionany, const string &text) {
        lexer.setText(text);
        return assertion(lexer,dictionany);
    }

    Rule assertion(Lexer &lexer, vector<string>& dictionany) {
        //cerr << "assertion" << endl;
        Rule rule_node;
        rule_node.premice = fullId(lexer,dictionany);
        Symbol symbol;
        if ( (symbol.id = Symbol::ARE) && accept(lexer, symbol) ) { 
            rule_node.conclusion = inherit(lexer,dictionany);
        } else if ( (symbol.id = Symbol::HAVE) && accept(lexer, symbol) ) {
            rule_node.conclusion = property(lexer,dictionany);
        } else if ( (symbol.id = Symbol::CAN) && accept(lexer, symbol) ) {
            rule_node.conclusion = method(lexer,dictionany);
        }
        return rule_node;
    } 
    
    Conjunction inherit(Lexer &lexer, vector<string>& dictionany) { 
        //cerr << "inherit" << endl;
        return fullId(lexer,dictionany);
    } 

    Conjunction property(Lexer &lexer, vector<string>& dictionany) {
        //cerr << "property" << endl; 
        Conjunction conjunction;
        //Clause property_clause = { Clause::PROPERTY, id(lexer)  };
        conjunction.insert(id(lexer,dictionany));
        return conjunction;
    }
    Conjunction method(Lexer &lexer, vector<string>& dictionany) {
        //cerr << "method" << endl;
        Conjunction conjunction;
        //Clause method_clause = { Clause::METHOD, id(lexer) };
        conjunction.insert(id(lexer,dictionany));
        return conjunction; 
    }
    Conjunction fullId(Lexer &lexer, vector<string>& dictionany) {
        //cerr << "fullid" << endl;
        Conjunction conjunction;
        //conjunction.clauses.insert(begin(conjunction.clauses), base.getRegisteredClause({Clause::ID,id(lexer)}));
        conjunction.insert( id(lexer,dictionany) );
        lexer.nextSymbol();
        Symbol symbol;
        if ( (symbol.id = Symbol::WITH) && accept(lexer,symbol) ) {
            conjunction.insert(properties(lexer,dictionany));
        }
        if ( (symbol.id = Symbol::THAT_CAN) && accept(lexer, symbol) ) {
            conjunction.insert(interface(lexer,dictionany));
        }
        return conjunction;
    }
    Conjunction properties(Lexer &lexer, vector<string>& dictionany) {
        //cerr << "properties" << endl;
        Conjunction conjunction; 
        //conjunction.clauses.insert(begin(conjunction.clauses), base.getRegisteredClause({Clause::PROPERTY, id(lexer)}));
        conjunction.insert(id(lexer,dictionany));
        lexer.nextSymbol();
        Symbol symbol;
        while ( (symbol.id = Symbol::AND) && accept(lexer,symbol) ) {
            //conjunction.clauses.insert(begin(conjunction.clauses), base.getRegisteredClause({Clause::PROPERTY, id(lexer)}));
            conjunction.insert(id(lexer,dictionany));
            lexer.nextSymbol();
        }
        return conjunction;
    }
    Conjunction interface(Lexer &lexer, vector<string>& dictionany) {
        //cerr << "interface" << endl;
        Conjunction conjunction; 
        //conjunction.clauses.insert(begin(conjunction.clauses), base.getRegisteredClause({Clause::METHOD, id(lexer)}));
        conjunction.insert(id(lexer,dictionany));
        lexer.nextSymbol();
        Symbol symbol;
        while ( (symbol.id = Symbol::AND) && accept(lexer,symbol) ) {
            //conjunction.clauses.insert(begin(conjunction.clauses), base.getRegisteredClause({Clause::METHOD, id(lexer)}));
            conjunction.insert(id(lexer,dictionany));
            lexer.nextSymbol();
        }
        return conjunction;
    }
    unsigned char id(Lexer &lexer, vector<string>& dictionary) {
        lexer.nextSymbol();
        Symbol symbol;
        symbol.id = Symbol::ID;
        accept(lexer,symbol);
        //cerr << "id" << " "<< symbol.content << endl;
        vector<string>::iterator it = find(dictionary.begin(), dictionary.end(), symbol.content);
        if (it == dictionary.end()) {
            dictionary.push_back(symbol.content);
            return dictionary.size() - 1;
        } else {
            return distance(dictionary.begin(), it);
        }         
    }
};


int main()
{
    Lexer lexer;
    Parser parser;
    KnowledgeBase kb;

    vector<string> text;
    vector<Rule> rules;
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
    vector<vector<string>> test =
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

    for (auto item:test[0]) {
        cerr << item << endl;
        Rule r = parser.parse(lexer,kb._clauses, item);  
        rules.push_back(r); 
    }
    for (auto r:rules) {
        kb.add(r);
    }
    cerr << "Facts:-----------------------" << endl;
    for (auto rule:kb._base) {
        cerr << rule.toString(kb._clauses) << endl;
    }
    cerr << "-----------------------" << endl;

    Rule query;
    query.premice.insert(distance(kb._clauses.begin(), find(kb._clauses.begin(), kb._clauses.end(),"PIGS")));
    query.conclusion.insert(distance(kb._clauses.begin(), find(kb._clauses.begin(), kb._clauses.end(),"FLY")));

    if (kb.queryAll(query)) {
        cout << "All pigs can fly" << endl;
    } else if(kb.querySome(query)) {
        cout << "Some pigs can fly" << endl;
    } else {
        cout << "No pigs can fly" << endl;
    }
}