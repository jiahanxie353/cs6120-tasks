#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

class AbstractCondition {
public:
  virtual ~AbstractCondition() = default;
  virtual void flip() = 0;
};

class IntCondition : public AbstractCondition {
public:
  IntCondition(ICmpInst *I) : IC(I) {}

  void flip() override {
    ICmpInst::Predicate InvPred = IC->getInversePredicate();
    IC->setPredicate(InvPred);
    errs() << "I flipped a integer compare instr" << *IC << "\n";
  }

private:
  ICmpInst *IC;
};

class FloatCondition : public AbstractCondition {
public:
  FloatCondition(FCmpInst *F) : FC(F) {}

  void flip() override {
    FCmpInst::Predicate InvPred = FC->getInversePredicate();
    FC->setPredicate(InvPred);
    errs() << "I flipped a float compare instr" << *FC << "\n";
  }

private:
  FCmpInst *FC;
};

namespace {

struct SkeletonPass : public PassInfoMixin<SkeletonPass> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
    bool Changed = false;
    for (auto &F : M) {
      for (auto &BB : F) {
        for (auto &I : BB) {
          if (auto *Branch = dyn_cast<BranchInst>(&I)) {
            if (Branch->isConditional()) {
              Value *Cond = Branch->getCondition();
              AbstractCondition *C = nullptr;

              if (ICmpInst *ICmp = dyn_cast<ICmpInst>(Cond))
                C = new IntCondition(ICmp);

              if (FCmpInst *FCmp = dyn_cast<FCmpInst>(Cond))
                C = new FloatCondition(FCmp);

              if (C) {
                C->flip();
                Changed = true;
              }
            }
          }
        }
      }
    }
    if (!Changed)
      return PreservedAnalyses::all();
    else
      return PreservedAnalyses::none();
  };
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {.APIVersion = LLVM_PLUGIN_API_VERSION,
          .PluginName = "Skeleton pass",
          .PluginVersion = "v0.1",
          .RegisterPassBuilderCallbacks = [](PassBuilder &PB) {
            PB.registerPipelineStartEPCallback(
                [](ModulePassManager &MPM, OptimizationLevel Level) {
                  MPM.addPass(SkeletonPass());
                });
          }};
}
