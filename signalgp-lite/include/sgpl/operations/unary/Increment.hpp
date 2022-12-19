#pragma once
#ifndef SGPL_OPERATIONS_UNARY_INCREMENT_HPP_INCLUDE
#define SGPL_OPERATIONS_UNARY_INCREMENT_HPP_INCLUDE

#include <map>
#include <set>
#include <string>

#include "../../../../third-party/conduit/include/uit_emp/tools/string_utils.hpp"

#include "../../hardware/Core.hpp"
#include "../../program/Instruction.hpp"
#include "../../program/Program.hpp"

namespace sgpl {

/**
 * Takes `reg[arg_0]`, increments it by one, and stores the result in
 * `reg[arg_0]`.
 */
struct Increment {

  template<typename Spec>
  static void run(
    sgpl::Core<Spec>& core,
    const sgpl::Instruction<Spec>& inst,
    const sgpl::Program<Spec>&,
    typename Spec::peripheral_t&
  ) noexcept {
    ++core.registers[ inst.args[0] ];
  }

  static std::string name() { return "Increment"; }

  static size_t prevalence() { return 1; }

  template<typename Spec>
  static auto descriptors( const sgpl::Instruction<Spec>& inst ) {

    return std::map<std::string, std::string>{
      { "argument a", uit_emp::to_string( static_cast<int>( inst.args[0] ) ) },
      { "summary", "a = a + 1" },
    };
  }

  template<typename Spec>
  static std::set<std::string> categories(const sgpl::Instruction<Spec>&) {
    return {
      "calculation",
      "intrinsic",
      "op",
    };
  }

};

} // namespace sgpl

#endif // #ifndef SGPL_OPERATIONS_UNARY_INCREMENT_HPP_INCLUDE
