#include "automaton.hpp"
#include <iostream>
#include <sstream>

Automaton::Automaton(const std::string& states_str, const std::string& alphabet_str,
    const std::string& start_state_str, const std::string& accept_states_str) {
    parseStates(states_str);
    parseAlphabet(alphabet_str);
    parseStartState(start_state_str);
    parseAcceptStates(accept_states_str);
}

void Automaton::parseStates(const std::string& states_str) {
    std::istringstream iss(states_str);
    std::string state;

    while (std::getline(iss, state, ',')) {
        if (state == "\\n") break;
        states.push_back(state);
    }
}

void Automaton::parseAlphabet(const std::string& alphabet_str) {
    std::istringstream iss(alphabet_str);
    std::string symbol;

    while (std::getline(iss, symbol, ',')) {
        if (symbol == "\\n") break;
    }

    while (std::getline(iss, symbol, ',')) {
        if (symbol == "\\n") break;
        alphabet.insert(symbol);
    }
}

void Automaton::parseStartState(const std::string& start_state_str) {
    start_state = start_state_str;
    start_state.erase(0, start_state.find("n") + 1);
    start_state.erase(0, start_state.find("n") + 2);
    start_state.erase(start_state.find(","));
}

void Automaton::parseAcceptStates(const std::string& accept_states_str) {
    std::istringstream iss(accept_states_str);
    std::string accept_state;

    for (int i = 0; i < 3; i++)
        while (std::getline(iss, accept_state, ',')) {
            if (accept_state == "\\n") break;
        }

    while (std::getline(iss, accept_state, ',')) {
        if (accept_state == "\\n") break;
        accept_states.insert(accept_state);
    }
}