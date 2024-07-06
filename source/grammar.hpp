#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <string>
#include <unordered_set>
#include <unordered_map>

class Grammar {
public:
    Grammar(const std::string& grammar_str);
    virtual ~Grammar() = default;

    virtual bool validate() const = 0;
    virtual bool generates(const std::string& str) const = 0;
    virtual std::string toString() const = 0;

protected:
    std::unordered_set<std::string> variables;
    std::unordered_set<std::string> terminals;
    std::string start_variable;
    std::unordered_map<std::string, std::vector<std::string>> productions;

    void parseVariables(const std::string& variables_str);
    void parseTerminals(const std::string& terminals_str);
    void parseStartVariable(const std::string& start_variable_str);
    void parseProductions(const std::string& productions_str);
};

#endif