#ifndef PDA_HPP
#define PDA_HPP

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "automaton.hpp"
#include "cfg.hpp"

class PDA : public Automaton {
public:
    PDA(const std::string& pda_str);
    bool validate() const override;
    bool accepts(const std::string& input_str) const override;
    std::string toString() const override;
    CFG toCFG() const;

private:
    std::string stack_start_symbol;
    std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>>>>> transitions;

    void parseStackStartSymbol(const std::string& stack_start_symbol_str);
    void parseTransitions(const std::string& transitions_str);
};

#endif