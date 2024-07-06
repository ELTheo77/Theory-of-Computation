#ifndef NFA_HPP
#define NFA_HPP

#include "automaton.hpp"
#include "dfa.hpp"
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Forward declaration of the setToString function
std::string setToString(const std::unordered_set<std::string>& set);

class NFA : public Automaton {
public:
    NFA(const std::string& nfa_str);
    bool validate() const override;
    bool accepts(const std::string& input_str) const override;
    std::string toString() const override;
    std::string toDFA() const;

private:
    std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_set<std::string>>> transitions;

    void parseTransitions(const std::string& transitions_str);
    std::unordered_set<std::string> epsilonClosure(const std::string& state) const;
    std::unordered_set<std::string> epsilonClosure(const std::unordered_set<std::string>& states) const;
    std::unordered_set<std::string> getNextStates(const std::unordered_set<std::string>& states, const std::string& symbol) const;
};

#endif