#include "cfg.hpp"
#include <sstream>
#include <algorithm>

CFG::CFG(const std::string& cfg_str) : Grammar(cfg_str) {}

bool CFG::validate() const {
    // Check if the start variable is a valid variable
    if (variables.find(start_variable) == variables.end()) {
        return false;
    }

    // Check if all productions are valid
    for (const auto& entry : productions) {
        const std::string& variable = entry.first;
        const std::vector<std::string>& production_rules = entry.second;

        // Check if the variable is a valid variable
        if (!isVariable(variable)) {
            return false;
        }

        // Check if all production rules are valid
        for (const std::string& rule : production_rules) {
            if (!isValidProduction(rule)) {
                return false;
            }
        }
    }

    return true;
}

bool CFG::generates(const std::string& str) const {
    // TODO: CFG generates given string
    return false;
}

std::string CFG::toString() const {
    std::ostringstream oss;

    // Convert variables to string
    for (const std::string& variable : variables) {
        oss << variable << ",";
    }
    oss.seekp(-1, std::ios_base::end);
    oss << "\n";

    // Convert terminals to string
    for (const std::string& terminal : terminals) {
        oss << terminal << ",";
    }
    oss.seekp(-1, std::ios_base::end);
    oss << "\n";

    // Convert start variable to string
    oss << start_variable << "\n";

    // Convert productions to string
    for (const auto& entry : productions) {
        const std::string& variable = entry.first;
        const std::vector<std::string>& production_rules = entry.second;

        for (const std::string& rule : production_rules) {
            oss << variable << " " << rule << "\n";
        }
    }

    return oss.str();
}

bool CFG::isValidProduction(const std::string& production) const {
    std::istringstream iss(production);
    std::string symbol;

    while (iss >> symbol) {
        if (!isVariable(symbol) && !isTerminal(symbol)) {
            return false;
        }
    }

    return true;
}

bool CFG::isVariable(const std::string& symbol) const {
    return variables.find(symbol) != variables.end();
}

bool CFG::isTerminal(const std::string& symbol) const {
    return terminals.find(symbol) != terminals.end();
}