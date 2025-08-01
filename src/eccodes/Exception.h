#pragma once

#include <stdexcept>

class FatalRuntimeError : public std::runtime_error {
  public:
    explicit FatalRuntimeError(const std::string& message) : std::runtime_error(message) {}
};


class RecoverableRuntimeError : public std::runtime_error {
  public:
    explicit RecoverableRuntimeError(const std::string& message): std::runtime_error(message){}
};
