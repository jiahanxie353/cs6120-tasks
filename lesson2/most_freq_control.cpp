#include <jsoncpp/json/json.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

const std::set<std::string> CONTROL_OPS = {"jmp", "br", "call", "ret"};

bool isControlOp(const Json::Value);
std::vector<std::string> findMostFreqControl(std::map<std::string, int>);

int main(int argc, char *argv[]) {
    // this simple task will count the most frequent control op, which includes
    // "jmp", "br", "call", and "ret"
    if (argc < 2) {
        std::cout << "Missing bril test file" << std::endl;
		return EXIT_FAILURE;
    }
	std::ifstream jsonFile(argv[1]);
	Json::Value brilProg;
	jsonFile >> brilProg;

	std::map<std::string, int> controlOpCnt;
	for (const std::string op: CONTROL_OPS){
		controlOpCnt[op] = 0;
	}
	
	const Json::Value brilFunctions = brilProg["functions"];
	for (int fcnIdx = 0; fcnIdx < brilFunctions.size(); ++fcnIdx) {
		const Json::Value brilInstrucions = brilFunctions[fcnIdx]["instrs"];
		for (int instrIdx = 0; instrIdx < brilInstrucions.size(); ++instrIdx) {
			const Json::Value instruction = brilInstrucions[instrIdx];
			if (isControlOp(instruction)){
				controlOpCnt[instruction["op"].asString()] += 1;
			}
		}
	}

	for (auto const& op: findMostFreqControl(controlOpCnt)) {
		std::cout << op << std::endl;
	}

    return EXIT_SUCCESS;
}

bool isControlOp(const Json::Value brilInstruction) {
	if (brilInstruction.isMember("op") && CONTROL_OPS.find(brilInstruction["op"].asString()) != CONTROL_OPS.end()) {
		return true;
	} 
	return false;
}

std::vector<std::string> findMostFreqControl(std::map<std::string, int> controlOpCount) {
	std::vector<std::string> candidateOp;
	int currMax = 1;
	
	for (auto const& [opName, opCount]: controlOpCount) {
		if (opCount > currMax) {
			currMax = opCount;
			candidateOp = {opName};
		}
		else if (opCount == currMax)
		{
			candidateOp.push_back(opName);
		}
	}

	return candidateOp;
}
