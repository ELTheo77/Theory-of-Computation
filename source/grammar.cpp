#include "grammar.hpp"
#include <sstream>

Grammar::Grammar(const std::string& grammar_str) {
    std::istringstream iss(grammar_str);
    std::string line;

    std::getline(iss, line);
    parseVariables(line);
    std::getline(iss, line);
    parseTerminals(line);
    std::getline(iss, line);
    parseStartVariable(line);

    while (std::getline(iss, line)) {
        parseProductions(line);
    }
}

void Grammar::parseVariables(const std::string& variables_str) {
    std::istringstream iss(variables_str);
    std::string variable;

    while (std::getline(iss, variable, ',')) {
        variables.insert(variable);
    }
}

void Grammar::parseTerminals(const std::string& terminals_str) {
    std::istringstream iss(terminals_str);
    std::string terminal;

    while (std::getline(iss, terminal, ',')) {
        terminals.insert(terminal);
    }
}

void Grammar::parseStartVariable(const std::string& start_variable_str) {
    start_variable = start_variable_str;
}

void Grammar::parseProductions(const std::string& productions_str) {
    std::istringstream iss(productions_str);
    std::string variable, production;

    std::getline(iss, variable, ' ');
    std::getline(iss, production);

    productions[variable].push_back(production);
}