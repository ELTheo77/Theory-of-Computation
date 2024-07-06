#include "pda.hpp"
#include <sstream>
#include <algorithm>

PDA::PDA(const std::string& pda_str) : Automaton("", "", "", "") {
    std::istringstream iss(pda_str);
    std::string line;

    std::getline(iss, line);
    parseStates(line);
    std::getline(iss, line);
    parseAlphabet(line);
    std::getline(iss, line);
    parseStackStartSymbol(line);
    std::getline(iss, line);
    parseStartState(line);
    std::getline(iss, line);
    parseAcceptStates(line);

    while (std::getline(iss, line)) {
        parseTransitions(line);
    }
}

bool PDA::validate() const {
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

    return true;
}

bool PDA::accepts(const std::string& input_str) const {
    // TODO: simulate PDA on the input string
    return false;
}

std::string PDA::toString() const {
    std::ostringstream oss;

    // Convert states to string
    for (const std::string& state : states) {
        oss << state << ",";
    }
    oss.seekp(-1, std::ios_base::end);
    oss << "\n";

    // Convert alphabet to string
    for (const std::string& symbol : alphabet) {
        oss << symbol << ",";
    }
    oss.seekp(-1, std::ios_base::end);
    oss << "\n";

    // Convert stack start symbol to string
    oss << stack_start_symbol << "\n";

    // Convert start state to string
    oss << start_state << "\n";

    // Convert accept states to string
    for (const std::string& accept_state : accept_states) {
        oss << accept_state << ",";
    }
    oss.seekp(-1, std::ios_base::end);
    oss << "\n";

    // Convert transitions to string
    for (const auto& state_entry : transitions) {
        const std::string& state = state_entry.first;
        const auto& input_map = state_entry.second;

        for (const auto& input_entry : input_map) {
            const std::string& input_symbol = input_entry.first;
            const auto& stack_map = input_entry.second;

            for (const auto& stack_entry : stack_map) {
                const std::string& stack_symbol = stack_entry.first;
                const auto& next_state_stack_ops = stack_entry.second;

                for (const auto& next_state_stack_op : next_state_stack_ops) {
                    const std::string& next_state = next_state_stack_op.first;
                    const std::string& stack_op = next_state_stack_op.second;

                    oss << state << " " << input_symbol << " " << stack_symbol << " " << next_state << " " << stack_op << "\n";
                }
            }
        }
    }

    return oss.str();
}

CFG PDA::toCFG() const {
    // TODO: convert PDA to  CFG
    return CFG("");
}

void PDA::parseStackStartSymbol(const std::string& stack_start_symbol_str) {
    stack_start_symbol = stack_start_symbol_str;
}

void PDA::parseTransitions(const std::string& transitions_str) {
    std::istringstream iss(transitions_str);
    std::string state, input_symbol, stack_symbol, next_state, stack_op;

    iss >> state >> input_symbol >> stack_symbol >> next_state >> stack_op;
    transitions[state][input_symbol][stack_symbol].push_back(std::make_pair(next_state, stack_op));
}