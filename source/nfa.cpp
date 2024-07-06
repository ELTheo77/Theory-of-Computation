#include "nfa.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <queue>

NFA::NFA(const std::string& nfa_str) {
    std::istringstream iss(nfa_str);
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

bool NFA::validate() const {
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
        const auto& symbol_next_states = state_transitions.second;

        // Check if the state is a valid state
        if (std::find(states.begin(), states.end(), state) == states.end()) {
            return false;
        }

        // Check if all transition symbols are valid alphabet symbols or epsilon
        for (const auto& symbol_states : symbol_next_states) {
            const std::string& symbol = symbol_states.first;

            if (symbol != "" && alphabet.find(symbol) == alphabet.end()) {
                return false;
            }

            // Check if all next states are valid states
            for (const std::string& next_state : symbol_states.second) {
                if (std::find(states.begin(), states.end(), next_state) == states.end()) {
                    return false;
                }
            }
        }
    }

    return true;
}

bool NFA::accepts(const std::string& input_str) const {
    std::unordered_set<std::string> current_states = epsilonClosure(start_state);

    for (char symbol : input_str) {
        std::string symbol_str(1, symbol);

        // Check if the symbol is a valid alphabet symbol
        if (alphabet.find(symbol_str) == alphabet.end()) {
            return false;
        }

        // Get the next states based on the current states and symbol
        current_states = epsilonClosure(getNextStates(current_states, symbol_str));
    }

    // Check if any of the final states are accept states
    for (const std::string& state : current_states) {
        if (accept_states.find(state) != accept_states.end()) {
            return true;
        }
    }

    return false;
}

std::string NFA::toString() const {
    std::ostringstream oss;

    // Convert states to string
    for (size_t i = 0; i < states.size(); ++i) {
        oss << states[i];
        if (i < states.size() - 1) {
            oss << ",";
        }
    }
    oss << "\n";

    // Convert alphabet to string
    for (const std::string& symbol : alphabet) {
        oss << symbol << ",";
    }
    oss.seekp(-1, std::ios_base::end);
    oss << "\n";

    // Convert start state to string
    oss << start_state << "\n";

    // Convert accept states to string
    for (const std::string& accept_state : accept_states) {
        oss << accept_state << ",";
    }
    oss.seekp(-1, std::ios_base::end);
    oss << "\n";

    // Convert transitions to string
    for (const auto& state_transitions : transitions) {
        const std::string& state = state_transitions.first;
        const auto& symbol_next_states = state_transitions.second;

        for (const auto& symbol_states : symbol_next_states) {
            const std::string& symbol = symbol_states.first;
            const auto& next_states = symbol_states.second;

            for (const std::string& next_state : next_states) {
                oss << state << "," << (symbol.empty() ? "e" : symbol) << "," << next_state << "\n";
            }
        }
    }

    return oss.str();
}

std::string NFA::toDFA() const {
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> dfa_transitions;
    std::unordered_set<std::string> dfa_states;
    std::unordered_set<std::string> dfa_accept_states;

    std::queue<std::unordered_set<std::string>> state_queue;
    std::unordered_map<std::string, std::unordered_set<std::string>> state_map;

    std::unordered_set<std::string> initial_states = epsilonClosure(start_state);
    std::string initial_state = setToString(initial_states);
    state_queue.push(initial_states);
    state_map[initial_state] = initial_states;
    dfa_states.insert(initial_state);

    while (!state_queue.empty()) {
        std::unordered_set<std::string> current_states = state_queue.front();
        state_queue.pop();
        std::string current_state = setToString(current_states);

        // Check if the current state is an accept state
        for (const std::string& state : current_states) {
            if (accept_states.find(state) != accept_states.end()) {
                dfa_accept_states.insert(current_state);
                break;
            }
        }

        // Process transitions for each alphabet symbol
        for (const std::string& symbol : alphabet) {
            std::unordered_set<std::string> next_states = epsilonClosure(getNextStates(current_states, symbol));
            std::string next_state = setToString(next_states);

            if (!next_states.empty()) {
                dfa_transitions[current_state][symbol] = next_state;

                if (state_map.find(next_state) == state_map.end()) {
                    state_queue.push(next_states);
                    state_map[next_state] = next_states;
                    dfa_states.insert(next_state);
                }
            }
        }
    }

    std::ostringstream oss;

    // Convert DFA states to string
    for (const std::string& state : dfa_states) {
        oss << state << ",";
    }
    oss.seekp(-1, std::ios_base::end);
    oss << "\\n";

    // Convert DFA alphabet to string
    for (const std::string& symbol : alphabet) {
        oss << symbol << ",";
    }
    oss.seekp(-1, std::ios_base::end);
    oss << "\\n";

    // Convert DFA start state to string
    oss << initial_state << "\\n";

    // Convert DFA accept states to string
    for (const std::string& accept_state : dfa_accept_states) {
        oss << accept_state << ",";
    }
    oss.seekp(-1, std::ios_base::end);
    oss << "\\n";

    // Convert DFA transitions to string
    for (const auto& state_transitions : dfa_transitions) {
        const std::string& state = state_transitions.first;
        const auto& symbol_next_state = state_transitions.second;

        for (const auto& symbol_state : symbol_next_state) {
            const std::string& symbol = symbol_state.first;
            const std::string& next_state = symbol_state.second;

            oss << state << "," << symbol << "," << next_state << "\\n";
        }
    }

    return oss.str();
}

void NFA::parseTransitions(const std::string& transitions_str) {
    std::istringstream iss(transitions_str);
    std::string state, symbol, next_state, blank;
    std::string prev_state, prev_symbol, prev_next_state;
    int flag = 0;

    for (int i = 0; i < 4; i++)
        while (std::getline(iss, blank, ',')) {
            if (blank == "\\n") break;
        }

    for (int i = 0; i < 100; i++) {
        std::getline(iss, state, ',');
        std::getline(iss, symbol, ',');
        std::getline(iss, next_state, ',');
        std::getline(iss, blank, ',');

        if (symbol == "e") {
            symbol = "";
        }

        if (state == prev_state && symbol == prev_symbol && next_state == prev_next_state) break;

        prev_state = state;
        prev_symbol = symbol;
        prev_next_state = next_state;

        if (next_state[next_state.length() - 1] == '-') {
            next_state.erase(next_state.length() - 1);
            transitions[state][symbol].insert(next_state);
            break;
        }

        transitions[state][symbol].insert(next_state);
    }


}

std::unordered_set<std::string> NFA::epsilonClosure(const std::string& state) const {
    std::unordered_set<std::string> closure;
    std::queue<std::string> state_queue;

    closure.insert(state);
    state_queue.push(state);

    while (!state_queue.empty()) {
        std::string current_state = state_queue.front();
        state_queue.pop();

        // Find epsilon transitions from the current state
        auto it = transitions.find(current_state);
        if (it != transitions.end()) {
            auto epsilon_it = it->second.find("");
            if (epsilon_it != it->second.end()) {
                for (const std::string& next_state : epsilon_it->second) {
                    if (closure.find(next_state) == closure.end()) {
                        closure.insert(next_state);
                        state_queue.push(next_state);
                    }
                }
            }
        }
    }

    return closure;
}

std::unordered_set<std::string> NFA::epsilonClosure(const std::unordered_set<std::string>& states) const {
    std::unordered_set<std::string> closure;

    for (const std::string& state : states) {
        std::unordered_set<std::string> state_closure = epsilonClosure(state);
        closure.insert(state_closure.begin(), state_closure.end());
    }

    return closure;
}

std::unordered_set<std::string> NFA::getNextStates(const std::unordered_set<std::string>& states, const std::string& symbol) const {
    std::unordered_set<std::string> next_states;

    for (const std::string& state : states) {
        auto it = transitions.find(state);
        if (it != transitions.end()) {
            auto symbol_it = it->second.find(symbol);
            if (symbol_it != it->second.end()) {
                next_states.insert(symbol_it->second.begin(), symbol_it->second.end());
            }
        }
    }

    return next_states;
}

std::string setToString(const std::unordered_set<std::string>& set) {
    std::ostringstream oss;
    bool first = true;

    for (const std::string& item : set) {
        if (!first) {
            oss << "";
        }
        oss << item;
        first = false;
    }

    return oss.str();
}