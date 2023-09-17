#include "data_flow.hpp"

#include <algorithm>
#include <queue>
#include <type_traits>

string Phi = "\u03A6";

int main(int argc, char* argv[]) {
    const string analysis = reachDef;
    json brilProg = readJson(argv[1]);
    for (auto& brilFcn : brilProg.at("functions")) {
        std::cout << "function: " << brilFcn.at("name") << std ::endl;
        set<string> initValues;
        if (brilFcn.contains("args")) {
            for (const auto& arg : brilFcn.at("args"))
                initValues.insert(arg["name"].get<string>());
        }
        CFG cfg(brilFcn);
        DataFlow<string> dataFlow(analysis, cfg, initValues);
        auto [in, out] = workList<string>(dataFlow);

        auto itIn = in.begin();
        auto itOut = out.begin();

        while (itIn != in.end()) {
            std::cout << "block: " << itIn->first->getLabel() << std::endl;

            std::cout << "in: ";
            if (itIn->second.size() == 0)
                std::cout << Phi;
            else {
                for (auto setIt = itIn->second.begin();
                     setIt != itIn->second.end(); ++setIt) {
                    std::cout << *setIt;
                    if (std::next(setIt) != itIn->second.end())
                        std::cout << ", ";
                }
            }
            std::cout << std::endl;

            std::cout << "out : ";
            if (itOut->second.size() == 0)
                std::cout << Phi;
            else {
                for (auto setIt = itOut->second.begin();
                     setIt != itOut->second.end(); ++setIt) {
                    std::cout << *setIt;
                    if (std::next(setIt) != itOut->second.end())
                        std::cout << ", ";
                }
            }
            std::cout << std::endl;
            ++itIn;
            ++itOut;
        }
    }

    return EXIT_SUCCESS;
}

template <class T>
DataFlow<T>::DataFlow(const string& a, CFG& c, const set<T>& iv)
    : analysis(a), cfg(c), initValue(iv) {
    tuple<dataflow::mergeType<T>, dataflow::transferType<T>> mergeTransfer =
        this->mergeTransferFactory(a);
    this->merge = std::get<0>(mergeTransfer);
    this->transfer = std::get<1>(mergeTransfer);
}

template <class T>
tuple<dataflow::mergeType<T>, dataflow::transferType<T>>
DataFlow<T>::mergeTransferFactory(const string analysis) {
    if (analysis.compare(reachDef) == 0) {
        static_assert(std::is_same<T, Var>::value, "T must be string");

        dataflow::mergeType<T> merge =
            [this](const vector<set<T>>& outPreds) -> set<T> {
            // union sets of strings
            set<T> unionedRes;
            for (const auto& s : outPreds)
                unionedRes.insert(s.begin(), s.end());
            return unionedRes;
        };

        dataflow::transferType<T> transfer =
            [this](const shared_ptr<Block> currBlock,
                   const set<T>& inputSet) -> set<T> {
            set<T> defined = currBlock->getDefined<T>();
            set<T> killed = currBlock->computeKilled<T>(inputSet);
            set<T> difference;
            std::set_difference(inputSet.begin(), inputSet.end(),
                                killed.begin(), killed.end(),
                                std::inserter(difference, difference.begin()));
            set<T> unionedRes;
            std::set_union(defined.begin(), defined.end(), difference.begin(),
                           difference.end(),
                           std::inserter(unionedRes, unionedRes.begin()));
            return unionedRes;
        };

        return std::make_tuple(merge, transfer);
    }
    throw std::runtime_error("Data flow analysis not supported: " + analysis);
}

template <class T>
tuple<map<shared_ptr<Block>, set<T>>, map<shared_ptr<Block>, set<T>>> workList(
    DataFlow<T>& df) {
    CFG cfg = df.getCFG();
    shared_ptr<Block> entry = cfg.getEntry();

    map<shared_ptr<Block>, set<T>> in, out;
    in.insert({entry, df.getInitValue()});
    for (const auto block : cfg.getBasicBlocks()) {
        out.insert({block, df.getInitValue()});
    }

    std::queue<shared_ptr<Block>> worklist;
    for (const auto b : cfg.getBasicBlocks()) worklist.push(b);
    while (!worklist.empty()) {
        shared_ptr<Block> block = worklist.front();
        worklist.pop();

        if (block->getPredecessors().size() > 0) {
            vector<set<T>> outPreds;
            for (const auto pred : block->getPredecessors())
                outPreds.push_back(out[pred]);

            set<T> predsOut = df.merge(outPreds);
            in.insert({block, predsOut});
        }

        auto currOut = out[block];
        set<T> transferRes = df.transfer(block, in[block]);
        out[block] = df.transfer(block, in[block]);

        if (currOut != out[block]) {
            for (const auto succ : block->getSuccessors())
                worklist.emplace(succ);
        }
    }
    return std::make_tuple(in, out);
}

template <class T>
CFG& DataFlow<T>::getCFG() const {
    return this->cfg;
}

template <class T>
set<T> DataFlow<T>::getInitValue() const {
    return this->initValue;
}