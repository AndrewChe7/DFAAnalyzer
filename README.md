# DFAAnalyzer
Simple imgui deterministic finite automaton analyzer

# Installation
```bash
mkdir build && cd build
conan install ..
cmake ..
make
```
Run executable. Make graph of your DFA, write sequence using 0, 1 and *. Then press "Start". 
"Next step" will change current state and sequence depending on your graph. By default there 
is two states: q0 and qf. 
