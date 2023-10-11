# Writing code
@tableofcontents

The ECSS-Services repository is based on C++17 and is compiled by gcc. The nature of the project means that there are a number of limitations and guidelines that need to be followed before code is accepted to the default branch.

## Restrictions {#restrictions}

This repository contains mission-critical software that is destined for use in space. As such, a number of restrictions are in place to ensure high performance, bounded memory and determinism.

The following C++ features are **forbidden** from use in flight-ready software:
1. **Dynamic memory allocation**.

   This prohibits use of `malloc`/`free`, `new`/`delete` and almost most `std::` containers.
2. **Run-Time Type Inference** (RTTI).

   This prohibits use of `dynamic_cast`, `typeid` and `std::type_info`.
3. **Exceptions**
4. **Multiple inheritance**
5. Some features only available in [hosted implementations](https://en.cppreference.com/w/cpp/freestanding).

   If a C++ or compiler feature is not available in a bare-metal microcontroller, then it cannot be used in this repository.

   This prohibits use of libraries such as `<ctime>`, `<pthreads>`, `<iostream>` and others.

@see [DDJF_OBSW](https://gitlab.com/acubesat/documentation/cdr-public/-/blob/master/DDJF/DDJF_OBSW.pdf)

There are no strict requirements about compiler portability: ecss-services is built around modern versions of `gcc` and
`clang`. While no unexpected behaviour mush be invoked, and the code must remain portable across different
architectures, we occasionally use compiler-specific features to aid performance or memory management.

## Code standards {#seandards}

The ECSS-Services repository uses a number of tools to perform static code analysis, which are part of the automated CI/CD pipeline.

Due to the lack of available free static analysis tools for embedded standards, only the checks mentioned above are executed. However, performed static analysis checks and rules loosely follow the following guidelines:
- [The Power of 10: Rules for Developing Safety-Critical Code](https://spinroot.com/gerard/pdf/P10.pdf)
- [MISRA C++](https://www.misra.org.uk/misra-c-plus-plus/)
- [Joint Strike Fighter C++ Coding Standards](https://www.stroustrup.com/JSF-AV-rules.pdf)
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)

## Code style {#code-style}

This repository typically follows the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) with the following notable modifications:
1. The code is indented using **tabs**
2. A soft column limit of `120` characters is followed
3. Variable names are in `camelCase`
4. Constant names are in `PascalCase`

Code style can be enforced automatically:
- By using the CLion _Reformat Code_ function
- By using [`clang-format`](https://clang.llvm.org/docs/ClangFormat.html) and the accompanying `.clang-format` file.  \
  You can use the `ci/clang-format.sh` script for automated code reformatting across the repository.

Note that code style is not enforced authoritatively; sometimes deviations may exist between the two tools and the current code. Occasional deviations are accepted if needed for better readability and maintainability.

## Documentation {#documentation}

We use [Doxygen](https://www.doxygen.nl/index.html) to document our code and its interfaces. All classes, functions and most variables should be documented using doxygen.

We use javadoc-style comments, for example:
```cpp
/**
 * This class represents an object that exists.
 *
 * Represents a [physical object](https://en.wikipedia.org/wiki/Thing) and its location in our universe.
 * Coordinates are given according to XYZ. This is similar to the @ref Item class, but implemented dynamically.
 *
 * @warning Functions in this class are not re-entrant and should be used carefully when combined with an RTOS.
 */
class Something
```

## Resources {#resources}

Contributing to ecss-services requires using modern C++ that is not often seen in the wild. There are various resources
available online, covering beginner to advanced topics, such as:
1. Introductory
   - [A Tour of C++](https://isocpp.org/tour)
   - [learncpp.com](https://www.learncpp.com/)
   - [Introduction to C++, MIT 6.096](https://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-096-introduction-to-c-january-iap-2011/index.htm)
   - [Wikiversity Introduction to C++](https://en.wikiversity.org/wiki/C%2B%2B/Introduction)
3. Advanced
   - [C++ Notes for Professionals](https://goalkicker.com/CPlusPlusBook/)
4. Specific to modern C++
   - [Modern C++ Tutorial: C++11/14/17/20 On the Fly](https://github.com/changkun/modern-cpp-tutorial)
   - [C++ Super-FAQ](https://isocpp.org/faq)
   - [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
5. References
   - [cppreference.com](https://en.cppreference.com/w/), contains advanced descriptions of pretty much every feature of C++
6. Tools
   - [Compiler Explorer](https://godbolt.org/)
