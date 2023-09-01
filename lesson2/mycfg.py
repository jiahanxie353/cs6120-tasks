import json
import sys
from typing import List

Instruction = dict
Block = List[Instruction]

TERMINATORS_OP = ("jmp", "br", "ret")


def build_basic_blocks(instrs: List[Instruction]) -> List[Block]:
    """
    Given a list of instructions, group them into a list of basic blocks
    """
    basic_blocks, cur_block = [], []
    for instr in instrs:
        # if `instr` is a terminator, we want to complete the current basic block and append to the result basic blocks
        if is_terminator(instr):
            cur_block.append(instr)
            basic_blocks.append(cur_block)
            cur_block = []
        # if `instr` contains "label", we should make it the start of a basic block
        elif "label" in instr.keys():
            if len(cur_block) > 0:
                basic_blocks.append(cur_block)
            cur_block = [instr]
        else:
            cur_block.append(instr)
    basic_blocks.append(cur_block)
    return basic_blocks


def is_terminator(instr: Instruction):
    return "labels" in instr.keys() and instr["op"] in TERMINATORS_OP


def block_map(blocks: List[Block]) -> dict:
    mapping = {}
    for block in blocks:
        if "label" in block[0]:
            name = block[0]["label"]
        else:
            name = "b{}".format(len(mapping))
        mapping[name] = block
    return mapping


def build_cfg(name_block_map: dict) -> dict:
    cfg_graph = {}
    for i, name_block in enumerate(name_block_map.items()):
        name, block = name_block[0], name_block[-1]
        last_instr = block[-1]
        if is_terminator(last_instr):
            if last_instr["op"] in ("jmp", "br"):
                succ = last_instr["labels"]
            else:
                succ = []
        else:
            if i < len(name_block_map) - 1:
                succ = [list(name_block_map.keys())[i+1]]
            else:
                succ = []
        cfg_graph[name] = succ
    return cfg_graph


def mycfg():
    prog = json.load(sys.stdin)
    for function in prog["functions"]:
        instrs, name = function["instrs"], function["name"]
        basic_blocks = build_basic_blocks(instrs)
        name_block_map = block_map(basic_blocks)
        cfg = build_cfg(name_block_map)
        for name, succs in cfg.items():
            for succ in succs:
                print("{} -> {}".format(name, succ))


if __name__ == "__main__":
    mycfg()
