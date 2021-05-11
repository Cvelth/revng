#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <array>
#include <deque>
#include <iterator>
#include <numeric>
#include <queue>
#include <random>
#include <set>

#include "llvm/ADT/iterator_range.h"

#include "revng/ADT/RecursiveCoroutine.h"
#include "revng/Support/Assert.h"

class Node {
private:
  size_t Index;
  const char *Name;
  std::vector<Node *> Children;
  std::vector<Node *> Parents;

public:
  Node(size_t Index, const char *Name) : Index(Index), Name(Name) {}
  size_t index() const { return Index; }
  const char *name() const { return Name; }
  const std::vector<Node *> &children() const { return Children; }
  void addChild(Node *NewChild) {
    Children.push_back(NewChild);
    NewChild->Parents.push_back(this);
  }

  Node *getRandomParent() const {
    return Parents[static_cast<unsigned long int>(random()) % Parents.size()];
  }
};

class Graph {
private:
  std::deque<Node> Nodes;
  Node *Root;

public:
  Graph() : Nodes(), Root(newNode()) {}

  size_t size() const { return Nodes.size(); }

  Node *root() { return Root; }
  Node *newNode(const char *Name = nullptr) {
    Nodes.emplace_back(Nodes.size(), Name);
    return &Nodes.back();
  }
  Node *getRandom() {
    return &Nodes[static_cast<unsigned long>(random()) % Nodes.size()];
  }
};

inline Graph createSimpleGraph() {
  Graph SimpleGraph;
  Node *Root = SimpleGraph.root();
  auto *Left = SimpleGraph.newNode("Left");
  auto *Right = SimpleGraph.newNode("Right");
  auto *Post = SimpleGraph.newNode("Post");
  auto *PostPost = SimpleGraph.newNode("PostPost");
  Root->addChild(Left);
  Root->addChild(Right);
  Right->addChild(Post);
  Left->addChild(Post);
  Post->addChild(PostPost);
  return SimpleGraph;
}

#include <iostream>
template<typename OddsType,
         typename ActionType = decltype(OddsType::value_type::first),
         typename IntType = decltype(OddsType::value_type::second)>
struct ActionDistribution {
  ActionDistribution(OddsType const &Odds) :
    Odds(Odds), Underlying(0, totalOdds() - 1) {}

  template<typename GeneratorType>
  ActionType operator()(GeneratorType &Generator) {
    std::cout << "total:" << totalOdds() << '\n';
    const IntType Random = Underlying(Generator);
    std::cout << "random:" << Random << '\n';
    IntType Accumulator = 0;
    for (auto const &[Action, Value] : Odds) {
      std::cout << "Accumulator:" << Accumulator << '\n';
      if ((Accumulator += Value) >= Random)
        return Action;
    }
    revng_check(false);
    return ActionType{};
  }

private:
  constexpr IntType totalOdds() {
    return std::accumulate(Odds.begin(),
                           Odds.end(),
                           IntType{},
                           [](auto const &lhs, auto const &rhs) {
                             return lhs + rhs.second;
                           });
  }

private:
  OddsType const &Odds;
  std::uniform_int_distribution<IntType> Underlying;
};

inline Graph createRandomGraph() {
  Graph Result;
  srandom(1);

  enum class Action { NewChild, LinkToRandom, LinkToUncle, Stop };
  static constexpr std::array Odds = { std::pair{ Action::NewChild, 20u },
                                       std::pair{ Action::LinkToRandom, 5u },
                                       std::pair{ Action::LinkToUncle, 5u },
                                       std::pair{ Action::Stop, 5u } };

  std::mt19937 Generator(std::random_device{}());
  ActionDistribution Distribution{ Odds };

  std::queue<Node *> ToProcess;
  ToProcess.push(Result.root());

  while (!ToProcess.empty() and Result.size() < 300) {
    Node *Current = ToProcess.front();
    ToProcess.pop();

    Action CurrentAction = Distribution(Generator);
    while (CurrentAction != Action::Stop) {
      switch (CurrentAction) {
      case Action::NewChild: {
        Node *NewNode = Result.newNode();
        ToProcess.push(NewNode);
        Current->addChild(NewNode);
        break;
      }
      case Action::LinkToRandom:
        Current->addChild(Result.getRandom());
        break;
      case Action::LinkToUncle:
        Current->getRandomParent()->addChild(Current);
        break;
      case Action::Stop:
        revng_abort();
      }

      CurrentAction = Distribution(Generator);
    }
  }

  return Result;
}

extern size_t MaxDepth;
extern size_t Iterations;

struct Entry {
  using iterator = std::vector<Node *>::const_iterator;
  Node *CurNode = nullptr;
  bool FirstVisit = true;
  iterator ChildNext;
  iterator ChildEnd;
};

inline void iterativeFindMaxDepth(std::vector<Entry> &Stack) {
  while (not Stack.empty()) {
    revng_assert(nullptr != Stack.back().CurNode);

    // The first time we visit a node, we increment the Iterations, we detect if
    // we're closing a loop with something else already on the stack, and we
    // increment MaxDepth.

    Entry &CurrentEntry = Stack.back();

    if (CurrentEntry.FirstVisit) {
      CurrentEntry.FirstVisit = false;

      ++Iterations;

      bool Skip = false;
      for (const Entry &E : llvm::make_range(Stack.begin(), --Stack.end())) {
        if (E.CurNode == CurrentEntry.CurNode) {
          Skip = true;
          break;
        }
      }

      if (Skip) {
        Stack.pop_back();
        continue;
      }

      MaxDepth = std::max(MaxDepth, Stack.size());

      continue;
    }

    if (CurrentEntry.ChildNext != CurrentEntry.ChildEnd) {
      // Visit the next children
      Node *Child = *CurrentEntry.ChildNext;
      auto &Children = Child->children();
      ++CurrentEntry.ChildNext;
      Stack.push_back(Entry{ Child, true, Children.begin(), Children.end() });
    } else {
      // We're done visiting all the children of the node, pop it
      Stack.pop_back();
    }
  }
}

inline RecursiveCoroutine<> findMaxDepth(std::vector<Node *> &RCS) {
  revng_check(not RCS.empty() and nullptr != RCS.back());
  Node *Current = RCS.back();
  ++Iterations;

  revng_assert(RCS.size() != 0);
  for (const Node *N : llvm::make_range(RCS.begin(), --RCS.end())) {
    if (N == Current) {
      RCS.pop_back();
      rc_return;
    }
  }

  MaxDepth = std::max(MaxDepth, RCS.size());

  size_t ChildIndex = 0;
  for (Node *Child : Current->children()) {
    RCS.emplace_back(Child);
    rc_recur findMaxDepth(RCS);
    ++ChildIndex;
  }

  RCS.pop_back();
  rc_return;
}

inline RecursiveCoroutine<size_t>
findMaxDepthRet(Node *Current, std::set<Node *> &Stack) {
  size_t MaxChildDepth = 0ULL;
  for (Node *Child : Current->children()) {
    auto [_, New] = Stack.insert(Child);
    if (New)
      MaxChildDepth = std::max(MaxChildDepth,
                               rc_recur findMaxDepthRet(Child, Stack));
  }

  size_t CurrDepth = MaxChildDepth + 1;
  Stack.erase(Current);
  rc_return CurrDepth;
}

inline size_t iterativeFindMaxRet(Node *Root, std::set<Node *> &Stack) {

  using iterator = std::vector<Node *>::const_iterator;
  std::vector<std::tuple<Node *, iterator, size_t>> IterativeStack;
  size_t Max = 0ULL;

  IterativeStack.push_back({ Root, Root->children().begin(), 0ULL });

  while (not IterativeStack.empty()) {
    auto &[Current, ChildIt, MaxChildDepth] = IterativeStack.back();

    if (ChildIt != Current->children().end()) {

      Node *Child = *ChildIt;
      ++ChildIt;

      if (Stack.insert(Child).second)
        IterativeStack.push_back({ Child, Child->children().begin(), 0ULL });

    } else {

      size_t CurDepth = MaxChildDepth + 1;
      Stack.erase(Current);
      Max = std::max(Max, CurDepth);
      IterativeStack.pop_back();

      if (not IterativeStack.empty()) {
        auto &OldMax = std::get<2>(IterativeStack.back());
        OldMax = std::max(OldMax, CurDepth);
      }
    }
  }

  return Max;
}
