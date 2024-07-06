#ifndef AUTOMATON_HPP
#define AUTOMATON_HPP

#include <string>
#include <vector>
#include <unordered_set>

class Automaton {
public:
    Automaton() {}
    Automaton(const std::string& states_str, const std::string& alphabet_str,
        const std::string& start_state_str, const std::string& accept_states_str);
    virtual ~Automaton() = default;

    virtual bool validate() const = 0;
    virtual bool accepts(const std::string& input_str) const = 0;
    virtual std::string toString() const = 0;

protected:
    std::vector<std::string> states;
    std::unordered_set<std::string> alphabet;
    std::string start_state;
    std::unordered_set<std::string> accept_states;

    void parseStates(const std::string& states_str);
    void parseAlphabet(const std::string& alphabet_str);
    void parseStartState(const std::string& start_state_str);
    void parseAcceptStates(const std::string& accept_states_str);
};

#endif