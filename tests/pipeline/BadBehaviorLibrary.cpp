/// \file BadBehaviorLibrary.cpp
/// \brief This file, when compiled into a library, crashes in a manner
/// configured in the environment variables.

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

int *SomeGlobalPointerNobodyWillInitialize;

#ifdef __clang__
#define OPTNONE [[clang::optnone]]
#else
#define OPTNONE __attribute__((optimize("O0")))
#endif

static void printAndFlush(std::string_view String) {
  std::cout << String << std::endl;
  std::cout.flush();
}

OPTNONE static void doCrash() {
  const char *CEnv = std::getenv("REVNG_CRASH_SIGNAL");
  if (CEnv == nullptr)
    abort(); // But would this be a bug or a feature?
  std::string Env(CEnv);
  int Signal = std::stoi(Env);

  switch (Signal) {
  case SIGILL:
    printAndFlush("SIGILL via illegal instruction");
    asm(".byte 0x0f, 0x0b");
    __builtin_unreachable();
  case SIGABRT:
    printAndFlush("SIGABRT via abort()");
    abort();
    __builtin_unreachable();
  case SIGSEGV:
    printAndFlush("SIGSEGV via write to uninitialized pointer");
    *SomeGlobalPointerNobodyWillInitialize = 69;
    __builtin_unreachable();
  default:
    std::cout << "Signal " << Signal << " via raise()" << std::endl;
    std::cout.flush();
    std::raise(Signal);
    __builtin_unreachable();
  }
}

class WillCrash {
public:
  WillCrash() { doCrash(); }
};

static WillCrash CrashyBoi;
