#ifndef CFG_HPP
#define CFG_HPP

#include "grammar.hpp"

class CFG : public Grammar {
public:
    CFG(const std::string& cfg_str);
    bool validate() const override;
    bool generates(const std::string& str) const override;
    std::string toString() const override;

private:
    bool isValidProduction(const std::string& production) const;
    bool isVariable(const std::string& symbol) const;
    bool isTerminal(const std::string& symbol) const;
};

#endif