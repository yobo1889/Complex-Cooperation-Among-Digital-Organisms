#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "OrgState.h"
#include "sgpl/library/OpLibraryCoupler.hpp"
#include "sgpl/library/prefab/ArithmeticOpLibrary.hpp"
#include "sgpl/operations/flow_global/Anchor.hpp"
#include "sgpl/program/Instruction.hpp"
#include "sgpl/program/Program.hpp"
#include "sgpl/spec/Spec.hpp"

struct IOInstruction {
  template <typename Spec>
  static void run(sgpl::Core<Spec> &core, const sgpl::Instruction<Spec> &inst,
                  const sgpl::Program<Spec> &,
                  typename Spec::peripheral_t &state) noexcept {
    uint32_t output = core.registers[inst.args[0]];
    state.world->CheckOutput(output, state);

    uint32_t input = sgpl::tlrand.Get().GetUInt();
    core.registers[inst.args[0]] = input;
    state.add_input(input);
  }

  static std::string name() { return "IO"; }
  static size_t prevalence() { return 1; }
};

struct ReproduceInstruction {
  template <typename Spec>
  static void run(sgpl::Core<Spec> &core, const sgpl::Instruction<Spec> &inst,
                  const sgpl::Program<Spec> &,
                  typename Spec::peripheral_t &state) noexcept {
    state.world->ReproduceOrg(state.current_location);
  }

  static std::string name() { return "Reproduce"; }
  static size_t prevalence() { return 1; }
};

using Library =
    sgpl::OpLibraryCoupler<sgpl::ArithmeticOpLibrary, sgpl::global::Anchor,
                           sgpl::global::JumpIf, IOInstruction, 
                           ReproduceInstruction>;

using Spec = sgpl::Spec<Library, OrgState>;

#endif