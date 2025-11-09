
# DSA — Assignment Suite (C++): Neural Networks, Data Structures & Algorithms

This repository contains a student project implemented in modern C++ (C++17). It bundles implementations and demos for classic data structures & algorithms and a compact neural-network training/evaluation pipeline used across three course assignments.

The repo is self-contained: a small ANN library (layers, models, loss, optimizers), dataset loaders, demo programs and a shell-based compilation script. The original assignment briefs are in `Spec/`.

---

## Table of Contents

1. Summary
2. Quick Start
3. Results & Performance
4. File-level mapping to assignment requirements
5. How to build and run (detailed)
6. Troubleshooting
7. Project layout (important files)
8. License

---

## 1 — Summary

### Technical & build
- **Language:** C++ (C++17 required)
- **Key libraries:** xtensor (`.npy` I/O, tensor operations), fmt (string formatting), C++ standard library
- **Build:** `Code/compilation-command.sh` → `Code/program`

This repository bundles a compact ANN library, dataset loaders, demos and a shell build script; see Section 5 for full build/run details.

### Assignments (Assignment-1 / Assignment-2 / Assignment-3)

**Assignment-1 (Foundations):**  
Core data structure implementations (lists, stacks/queues, heaps, hash maps, sorting, trees) and dataset creation for MLP inference.

**Assignment-2 (ANN Training Pipeline):**  
HashMap and Heap implementations (TASK-1), complete MLP training with layers, loss functions, metrics, three optimizers (SGD/Adagrad/Adam with param groups), and checkpoint I/O (TASK-2).

**Assignment-3 (Graphs & Computational Graphs):**  
Graph data structures (directed/undirected via adjacency lists) and topological sorting algorithms (DFS/BFS) for computational graph traversal during backpropagation.

---

## 2 — Quick Start

Build and run the default demo (example tested on macOS/Linux). Two options are provided:

Option A — quick (use the included script):

```bash
cd Code
chmod +x compilation-command.sh
bash compilation-command.sh
./program
```

Option B — developer (uses Makefile, incremental build):

```bash
cd Code
make run
```

`make run` builds the binary (incrementally) and runs it from the `Code/` directory.

The default demo trains the 2-class classifier under `Code/datasets/2c-classification/` and writes checkpoints to `models/`.

---

## 3 — Results & Performance

This project was submitted for official university grading and achieved the following results:

-   **Official Grade:** Near-perfect score. The only deduction was a minor penalty for an unused, accidentally included external library that had no impact on the final logic or program correctness.
-   **Third-Party Unit Tests:** Passed 100% of unit tests provided by a third party, validating the correctness and robustness of the data structures and algorithms.

---

## 4 — File-level mapping to assignment requirements

Below is a concrete, file-level mapping showing the headers and key source files that implement the required student work. Use these as the authoritative list when preparing deliverables or grading.

### Assignment-1 (Foundations — data structures & algorithms)
- Lists / ArrayList / Iterators
  - Code/include/list/DLinkedList.h
  - Code/include/list/IList.h
  - Code/include/list/XArrayList.h
  - Code/include/list/listheader.h

- Stacks / Queues / Deque
  - Code/include/stacknqueue/Stack.h
  - Code/include/stacknqueue/Queue.h
  - Code/include/stacknqueue/IDeck.h

- Heap
  - Code/include/heap/IHeap.h
  - Code/include/heap/Heap.h
  - Code/demo/heap/HeapDemo.h (examples)

- Hash map
  - Code/include/hash/IMap.h
  - Code/include/hash/xMap.h
  - Code/demo/hash/xMapDemo.h (examples)

- Sorting & auxiliary
  - Code/include/sorting/ISort.h
  - Code/include/sorting/DLinkedListSE.h
  - Code/demo/sorting/* (various sort demos)

- Trees
  - Code/include/tree/IBST.h
  - Code/include/tree/ITreeWalker.h
  - Code/demo/tree/* (BST/AVL demos)

### Assignment-2 (Heap/Hash + dataset interfaces)
- Loader & dataset interfaces
  - Code/include/loader/dataset.h
  - Code/include/loader/dataloader.h

- Dataset factory used by ANN (TASK-2 depends on this)
  - Code/include/ann/dataset/DSFactory.h
  - Code/src/ann/dataset/DSFactory.cpp

- ANN Training components (TASK-2: Multi-Layer Perceptron)
  - **Activation Layers (required by spec)**
    - Code/include/ann/layer/ILayer.h (abstract base)
    - Code/include/ann/layer/FCLayer.h (fully connected)
    - Code/include/ann/layer/ReLU.h
    - Code/include/ann/layer/Sigmoid.h
    - Code/include/ann/layer/Tanh.h
    - Code/include/ann/layer/Softmax.h

  - **Loss function**
    - Code/include/ann/loss/ILossLayer.h
    - Code/include/ann/loss/CrossEntropy.h

  - **Metrics**
    - Code/include/ann/metrics/IMetrics.h
    - Code/include/ann/metrics/ClassMetrics.h

  - **Optimizers with parameter groups (required by spec)**
    - Code/include/ann/optim/IOptimizer.h (abstract base)
    - Code/include/ann/optim/IParamGroup.h (abstract base)
    - Code/include/ann/optim/SGD.h + Code/include/ann/optim/SGDParamGroup.h
    - Code/include/ann/optim/Adagrad.h + Code/include/ann/optim/AdaParamGroup.h
    - Code/include/ann/optim/Adam.h + Code/include/ann/optim/AdamParamGroup.h

  - **Model**
    - Code/include/ann/model/IModel.h (abstract base)
    - Code/include/ann/model/MLPClassifier.h (multi-layer classifier)

- Config & utilities
  - Code/include/ann/config/Config.h (hyperparameter management)
  - Code/include/ann/annheader.h (convenience header including all ANN components)
  - Code/src/ann/config/Config.cpp (implementation)
  - Code/src/tensor/xtensor_lib.cpp (tensor utility implementations)

### Assignment-3 (Graphs & topological sorting — TASK-1 / TASK-2)
- Graph data structures (TASK-1)
  - Code/include/graph/IGraph.h
  - Code/include/graph/AbstractGraph.h
  - Code/include/graph/DGraphModel.h (directed graphs)
  - Code/include/graph/UGraphModel.h (undirected graphs)
  - Code/demo/graph/DGraphDemo.h
  - Code/demo/graph/UGraphDemo.h

- Topological sorting for computational graphs (TASK-2)
  - Code/include/graph/TopoSorter.h (BFS/DFS sorting for backpropagation)
  - Implements vertex in-degree tracking and topological ordering

- Demos & examples
  - Code/src/program.cpp (demo launcher)
  - Code/demo/graph/DGraphDemo.h (directed graph examples)
  - Code/demo/graph/UGraphDemo.h (undirected graph examples)

### Implementation Notes

**Scope & Specification Compliance:**
All implementations strictly follow the assignment specifications. No additional components beyond the spec requirements have been implemented:

- **Assignment-1:** Lists, stacks, queues, heaps, hash maps, sorting, basic trees, and MLP inference components (ReLU + Softmax activation layers).
- **Assignment-2:** Heap & HashMap (TASK-1) and complete MLP training pipeline (TASK-2), including all three required optimizers (SGD, Adagrad, Adam), four additional activation layers (Sigmoid, Tanh, plus ReLU/Softmax from A1), loss functions (CrossEntropy), and metrics (ClassMetrics). Dataset factory and config management included.
- **Assignment-3:** Graph data structures (directed/undirected via adjacency lists) and topological sorting (DFS/BFS-based) for computational graph traversal during backpropagation.

**Note on file organization:**  
The files listed above are the concrete student-facing headers and key source files students were expected to implement or adapt to meet the specifications. The `Code/include/` headers are the primary artifacts for grading — students implement the interfaces and fill the "YOUR CODE HERE/TODO" areas in those headers and corresponding `Code/src/` implementations.

---

## 5 — How to build and run (detailed)

### Prerequisites

- **Compiler:** C++17-capable compiler (clang++ or g++)
- **xtensor:** A vendored subset exists under `Code/include/tensor/xtensor`, or install via package manager/conan/vcpkg
- **fmt:** Included under `Code/include/sformat`, or install system-wide

### Build options

**Option 1: Using the shell script (recommended)**

```bash
cd Code
chmod +x compilation-command.sh
bash compilation-command.sh
./program
```

**Option 2: Using Makefile (incremental, developer-friendly)**

```bash
cd Code
make run
```

**Option 3: Manual compilation with custom flags**

```
-std=c++17 -O2 -Iinclude -Wall -Wextra
```

### Expected output

The default demo trains a 2-class classifier using the dataset in `Code/datasets/2c-classification/`. Training progress is logged, and model checkpoints are saved to `models/`.

---

## 6 — Troubleshooting

**Common issues and solutions:**

- **`std::filesystem` IntelliSense warnings on macOS:** This is commonly an editor/indexer issue; building from terminal with a C++17 compiler usually succeeds. Ensure Xcode Command Line Tools are installed.

- **`xt::load_npy` exceptions:** Check that `.npy` files exist and are readable under `Code/datasets/*`. Ensure you're running the program from the `Code/` directory where datasets are located.

- **Config file errors:** 
  - Verify `Code/config.txt` exists and is readable
  - Check that numeric values (e.g., `learning_rate`) are valid numbers, not strings
  - Run the binary from the `Code/` directory (not the repo root)

---

## 7 — Project layout (important files & directories)

| Directory | Contents |
|-----------|----------|
| `Code/` | Build script, config, compiled binary, demo datasets |
| `Code/src/` | C++ source implementations (ANN, optimizers, main) |
| `Code/include/` | Header files (data structures, ANN API, utilities) |
| `Code/demo/` | Demo programs for various components |
| `models/` | Checkpoint outputs and trained model weights |
| `Spec/` | Original assignment specifications (PDF & TXT) |

---

## 8 — License

This project is released under the MIT License. See the included `LICENSE` file in the repository root for the full text.

SPDX-License-Identifier: MIT

---

## 👤 Author

**Truong Trung Bao**  
Student, Ho Chi Minh City University of Technology (HCMUT — VNU)

- GitHub: [BrianTr9](https://github.com/BrianTr9)
- University: [Ho Chi Minh City University of Technology (HCMUT)](https://www.hcmut.edu.vn)
- Data Structures & Algorithms (Semester 2, 2024)

