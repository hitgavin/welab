#pragma once

/**
 * @brief Disable copy constructor and assignment operator.
 */
#define DISABLE_COPY(ClassType)          \
  ClassType(const ClassType &) = delete; \
  ClassType &operator=(const ClassType &) = delete;

#define DECLARE_SINGLETON(ClassType)                           \
  static std::shared_ptr<ClassType> instance() {               \
    static std::once_flag flag;                                \
    static std::shared_ptr<ClassType> inst;                    \
    std::call_once(flag, [] { inst.reset(new ClassType()); }); \
    return inst;                                               \
  }

/**
 * @brief Class forward declaration.
 */
#define FORWARD_DECLARE_TYPE(ClassType) class ClassType;

/**
 * @brief Declare smart pointer inside a class
 */
#define DECLARE_POINTER_TYPE(ClassType)         \
  using SharedPtr = std::shared_ptr<ClassType>; \
  using UniquePtr = std::unique_ptr<ClassType>;
