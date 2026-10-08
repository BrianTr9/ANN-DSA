# ANN-DSA — A Neural-Network Library and Its Data Structures, in C++17

A small deep-learning library written **from scratch in C++17** — and, underneath it, the
data structures it runs on (linked/array lists, hash map, heap, graphs) implemented by hand
instead of using `std::vector`/`std::unordered_map`/`std::priority_queue`.

Layers, loss, metrics, optimizers (SGD / Adagrad / Adam), mini-batch data loading, model
checkpointing and a topological sorter are all implemented here. The only third-party code is
[xtensor](https://github.com/xtensor-stack/xtensor) (n-d arrays, `.npy` I/O) and
[fmt](https://github.com/fmtlib/fmt) (formatting), both vendored in `Code/include/`.

> Built for the HCMUT *Data Structures & Algorithms* (CO2003) course. The three assignment
> briefs are in [`Spec/`](Spec/). Author-reported result: near-full marks on the official
> grading, with the instructor-side unit tests passing.

---

## Highlights

- **Backpropagation by hand.** Every layer implements `forward`/`backward`; gradients are
  verified against finite differences (see [Verification](#verification)).
- **Optimizers with parameter groups.** `IOptimizer` owns one `IParamGroup` per layer; groups hold
  *non-owning* pointers to the layer's weights/gradients, so SGD, Adagrad (RMS-style) and Adam
  (with bias correction) update parameters in place with no copies.
- **Ownership-aware containers.** `DLinkedList`, `XArrayList`, `xMap` and `Heap` are class templates
  that take optional *deleter* callbacks (`freeKey`, `freeValue`, `Heap<T>::free`, …), so the same
  container works for values and for owning raw pointers, and frees them exactly once.
- **Hash map = separate chaining over `DLinkedList`**, load factor 0.75, automatic rehash.
- **Heap with a pluggable comparator** (min-heap by default, max-heap via comparator).
- **Graphs.** Directed and undirected graphs on an adjacency list, plus a `TopoSorter`
  (DFS- and BFS-based; Kahn's algorithm for BFS).
- **Mini-batch loading.** `DataLoader` supports shuffling (optionally seeded), `drop_last`, and range-based `for` over batches.
- **Checkpoints.** `MLPClassifier::save/load` writes an `arch.txt` plus one `.npy` per weight, bias.

## Results

Trained with the code in this repository (macOS, Apple clang, `-std=c++17`, no optimisation flags),
evaluated on the held-out **test split** shipped in `Code/datasets/`:

| Task | Model | Optimizer | Epochs | Test accuracy |
|------|-------|-----------|--------|---------------|
| 2-class (2-D points) | FC(2→50) – ReLU – FC(50→20) – ReLU – FC(20→2) – Softmax | SGD, lr 2e-3 | 1000 | **99.5 %** |
| 3-class (2-D points) | same, last layer 20→3 | Adam, lr 1e-3, β = (0.9, 0.99) | 1000 | **99.7 %** |

A model that is saved and loaded back produces identical predictions. Training takes about
12 s (2-class) to 22 s (3-class) on an Apple-silicon laptop.

## Quick start

```bash
git clone https://github.com/BrianTr9/ANN-DSA.git
cd ANN-DSA/Code
make run          # builds incrementally, then trains the 2-class demo
```

Alternatives: `bash compilation-command.sh && ./program`, or `make OPT=-O2` for an optimised build.
`make clean` removes the binary and the object files.

> **Run from `Code/`.** The program reads `config.txt` and `datasets/` with relative paths.
> The 2-class demo **overwrites** the pretrained weights in `Code/models/2c-classification-1/`
> (and, from the 3-class demo, `3c-classification-1/`). Use `git checkout -- Code/models` to restore them.

Requirements: a C++17 compiler (g++ or clang++) and `make`. Nothing else has to be installed —
xtensor, xtensor-blas and fmt are vendored. Tested on macOS; it uses only standard C++17.

### Use the library

```cpp
DSFactory factory("./config.txt");
auto* datasets = factory.get_datasets_2cc();            // normalised train / valid / test
DataLoader<double,double> train(datasets->get("train_ds"), 50, /*shuffle=*/true, /*drop_last=*/false);
DataLoader<double,double> valid(datasets->get("valid_ds"), 50, false, false);

ILayer* layers[] = { new FCLayer(2, 50, true), new ReLU(),
                     new FCLayer(50, 20, true), new ReLU(),
                     new FCLayer(20, 2, true),  new Softmax() };
MLPClassifier model("./config.txt", "2c-classification", layers, 6);   // model owns the layers

SGD optim(2e-3);  CrossEntropy loss;  ClassMetrics metrics(2);
model.compile(&optim, &loss, &metrics);
model.fit(&train, &valid, /*epochs=*/1000);
model.save("./models/2c-classification-1");

xt::xarray<double> probs  = model.predict(X, /*make_decision=*/false);  // class probabilities
xt::xarray<double> labels = model.predict(X, /*make_decision=*/true);   // argmax class ids
delete datasets;                                                        // the map owns the datasets
```

Full versions: `Code/include/ann/modelzoo/twoclasses.h` and `threeclasses.h`.

## Architecture

```
Code/include/
├── list/        IList, XArrayList, DLinkedList (+ forward and backward iterators)
├── hash/        IMap, xMap   (separate chaining, rehash, deleter callbacks)
├── heap/        IHeap, Heap  (comparator-driven min/max heap)
├── stacknqueue/ Stack, Queue (on top of DLinkedList)
├── graph/       IGraph → AbstractGraph → DGraphModel / UGraphModel, TopoSorter
├── loader/      Dataset, TensorDataset, DataLoader (range-for over batches)
└── ann/
    ├── layer/   ILayer → FCLayer, ReLU, Sigmoid, Tanh, Softmax
    ├── loss/    ILossLayer → CrossEntropy
    ├── metrics/ IMetrics → ClassMetrics (accuracy, macro/weighted precision, recall, F1)
    ├── optim/   IOptimizer → SGD, Adagrad, Adam;  IParamGroup → SGD/Ada/AdamParamGroup
    ├── model/   IModel → MLPClassifier (compile / fit / evaluate / predict / save / load)
    ├── dataset/ DSFactory        config/ Config        modelzoo/ ready-made examples
Code/src/        .cpp files of the ANN library, program.cpp (demo entry point)
Code/demo/       small usage examples for the data structures
Code/datasets/   2-class and 3-class point datasets (.npy)
Code/models/     pretrained checkpoints (arch.txt + .npy weights)
```

Training loop (`IModel::fit`): `zero_grad → forward → loss → backward → optimizer.step`
per mini-batch, then a validation pass per epoch. `backward` walks the layer list with the
`DLinkedList` **backward iterator**.

`Config` reads simple `key: value` lines (keys are case-insensitive, `#` starts a comment).
Recognised keys: `model_root`, `ckpt_name`, `arch_file`, `dataset_root`.

## Verification

- Every layer's gradient (FC with and without bias, 1-D and batched input, ReLU, Sigmoid, Tanh,
  Softmax, and a full FC–Tanh–FC–Softmax–CrossEntropy network) was checked against central
  finite differences.
- Adam and Adagrad were compared against a scalar reference implementation of their update rules.
- Lists, `xMap`, `Heap`, `Stack`, `Queue` were compared against `std::vector`, `std::map`,
  `std::multiset`, `std::stack` and `std::queue` on thousands of random operations, and
  ownership was checked with live-object counters (no leaks, no double frees).
- The whole suite ran clean under AddressSanitizer and UndefinedBehaviorSanitizer.

These checks were run as an ad-hoc local suite; **it is not committed to this repository**, so
there is no `make test` target yet.

## Scope and limitations

- Fully connected MLP classifiers only (no convolution, no GPU). Dense math goes through
  xtensor / xtensor-blas.
- Backpropagation is the usual layer-by-layer chain rule; it is **not** a general
  autodiff graph. `TopoSorter` is an independent, tested graph algorithm that provides the
  ordering step a computational-graph engine would need — it is not wired into
  `MLPClassifier`, whose layers form a simple chain.
- `Code/include/sorting/ISort.h` and `Code/include/tree/*` are interface headers only
  (provided by the course); sorting algorithms, BST/AVL, a singly linked list and the
  `DGraphAlgorithm`/`UGraphAlgorithm` demos are **not implemented**, so the corresponding
  files in `Code/demo/` do not compile on their own.

## Troubleshooting

- **`xt::load_npy` exception or "can not open" for the dataset** — run the binary from `Code/`.
- **Config values ignored** — only the keys listed above are read; unknown keys are harmless.
- **IntelliSense complains about `std::filesystem`** — editor issue only; the terminal build works.

## Author & license

**Truong Trung Bao** — HCMUT (VNU-HCM), Computer Science & Engineering ·
[GitHub @BrianTr9](https://github.com/BrianTr9)

Released under the [MIT License](LICENSE).
