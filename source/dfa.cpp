#include "dfa.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

DFA::DFA(const std::string& dfa_str) {
    std::istringstream iss(dfa_str);
    std::string line;

    std::getline(iss, line);
    parseStates(line);
    std::getline(iss, line);
    parseAlphabet(line);
    std::getline(iss, line);
    parseStartState(line);
    std::getline(iss, line);
    parseAcceptStates(line);
    std::getline(iss, line);
    parseTransitions(line);
}

bool DFA::validate() const {

    // Check if the start state is a valid state
    if (std::find(states.begin(), states.end(), start_state) == states.end()) {
        return false;
    }

    // Check if all accept states are valid states
    for (const std::string& accept_state : accept_states) {
        if (std::find(states.begin(), states.end(), accept_state) == states.end()) {
            return false;
        }
    }

    // Check if all transitions are valid
    for (const auto& state_transitions : transitions) {
        const std::string& state = state_transitions.first;
        const auto& symbol_next_state = state_transitions.second;

        // Check if the state is a valid state
        if (std::find(states.begin(), states.end(), state) == states.end()) {
            return false;
        }

        // Check if all transition symbols are valid alphabet symbols
        for (const auto& symbol_state : symbol_next_state) {
            const std::string& symbol = symbol_state.first;
            const std::string& next_state = symbol_state.second;

            if (alphabet.find(symbol) == alphabet.end()) {
                return false;
            }

            // Check if the next state is a valid state
            if (std::find(states.begin(), states.end(), next_state) == states.end()) {
                return false;
            }
        }
    }

    return true;
}

bool DFA::accepts(const std::string& input_str) const {
    std::string current_state = start_state;

    for (char symbol : input_str) {
        std::string symbol_str(1, symbol);

        // Check if the symbol is a valid alphabet symbol
        if (alphabet.find(symbol_str) == alphabet.end()) {
            return false;
        }

        // Get the next state based on the current state and symbol
        auto state_iter = transitions.find(current_state);
        if (state_iter != transitions.end()) {
            auto symbol_iter = state_iter->second.find(symbol_str);
            if (symbol_iter != state_iter->second.end()) {
                current_state = symbol_iter->second;
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }
    }

    // Check if the final state is an accept state
    return accept_states.find(current_state) != accept_states.end();
}

std::string DFA::toString() const {
    std::ostringstream oss;

    // Convert states to string
    for (size_t i = 0; i < states.size(); ++i) {
        oss << states[i];
        if (i < states.size() - 1) {
            oss << ",";
        }
    }
    oss << "n";

    // Convert alphabet to string
    for (const std::string& symbol : alphabet) {
        oss << symbol << ",";
    }
    oss.seekp(-1, std::ios_base::end);
    oss << "n";

    // Convert start state to string
    oss << start_state << "n";

    // Convert accept states to string
    for (const std::string& accept_state : accept_states) {
        oss << accept_state << ",";
    }
    oss.seekp(-1, std::ios_base::end);
    oss << "n";

    // Convert transitions to string
    for (const auto& state_transitions : transitions) {
        const std::string& state = state_transitions.first;
        const auto& symbol_next_state = state_transitions.second;

        for (const auto& symbol_state : symbol_next_state) {
            const std::string& symbol = symbol_state.first;
            const std::string& next_state = symbol_state.second;

            oss << state << "," << symbol << "," << next_state << "n";
        }
    }

    return oss.str();
}

void DFA::parseTransitions(const std::string& transitions_str) {
    std::istringstream iss(transitions_str);
    std::string state, symbol, next_state, blank;
    std::string prev_state, prev_symbol, prev_next_state;

    for (int i = 0; i < 4; i++)
        while (std::getline(iss, blank, ',')) {
            if (blank == "\\n") break;
        }

    for (int i = 0; i < 100; i++) {
        std::getline(iss, state, ',');
        std::getline(iss, symbol, ',');
        std::getline(iss, next_state, ',');
        std::getline(iss, blank, ',');

        if (state == prev_state && symbol == prev_symbol && next_state == prev_next_state) break;

        prev_state = state;
        prev_symbol = symbol;
        prev_next_state = next_state;

        transitions[state][symbol] = next_state;
    }
}