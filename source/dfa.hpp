#ifndef DFA_HPP
#define DFA_HPP

#include "automaton.hpp"
#include <unordered_map>

class DFA : public Automaton {
public:
    DFA(const std::string& dfa_str);
    bool validate() const override;
    bool accepts(const std::string& input_str) const override;
    std::string toString() const override;

private:
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> transitions;

    void parseTransitions(const std::string& transitions_str);
};

#endif