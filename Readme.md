# Probabilistic Meta Language

  This is a ML-like programming language with probabilistic behavior (e.g. sampling from probability distribution).
This language is a statically typed, and its types can represent probabilistic specification.

**Disclaimer: PFN provides no warranty or support for this software. Use it at your own risk.**

This software is developed as part of [PFN summer internship 2018](https://www.preferred-networks.jp/en/news/internship2018summer) and the main developer is [Yoji Nanjo](https://github.com/akitsu-sanae).

## Installation

1. install PRISM (https://www.prismmodelchecker.org/)
2. download this source code by `git clone https://github.com/pfnet-research/pml.git` or somehow, and then `$ make` in the directory
3. `./build/pml`, the interpreter binary file, will be generated

## Dependencies

  You need following software install these before building this software.

* `PRISM v4.4` : a probabilistic model checker
* `Clang v6.0 or later` : a C++ compiler
* `Boost v1.54 or later` : a widely used C++ library

## Example

### Coin Flip ([examples/coin.pml](examples/coin.pml)):
```
(let a = rand(0, 1) in
let b = rand(0, 1) in
a+b == 0) : {x:bool | Prob(x) <= 1/4}
```

This says that the probability that "toss a coin two times, and both are heads" is less than 1/4

### Speed Violation Detection based on two GPS locations ([examples/gps.pml](examples/gps.pml)):
```
let a = rand(-10, 10) in
let b = a + rand(0, 10) in
 not (b-a >= 10) /\ (b + rand(0, 5) - (a + rand(0, 5)) >= 10)
    : {x:bool | Prob(x) <= 1/10}
```

Suppose we want to detect speed violation from two GPS locations (This example is taken from [Uncertain\<T\>: Abstractions for Uncertain Hardware and Software](https://ieeexplore.ieee.org/document/7106409) and simplified).
Here `a` and `b` are real locations in consecutive timesteps while `a + rand(0, 5)` and `b + rand(0, 5)` are observed noisy locations using GPS system (we assume GPS locations are one dimension and noises distributes uniformly for simplicity).
Then this program says that the probability that "the system detected as a speed violation (`(b + rand(0, 5)) - (a + rand(0, 5)) >= 10`) even though it is not actually violating speed (`not (b-a >= 10)`)" is less than 1/10.

## Language Manual

### Expressions

Following expressions `e` are available:

* primitive
  - integer literals (`42`, `-12`, or so on)
  - boolean literals (`true`, `false`)
  - variable (`hoge`, `fuga1`, `piyo_`, or so on)
  - random sampling (`rand(0, 3)`, or so on)
* arithmetic
  - addition, subtraction, multiplication, and division (`e1 + e2`, `e1 - e2`, `e1 * e2`, `e1 / e2`)
  - comparison (`e1 == e2`, `e1 != e2`, `e1 <= e2`, `e1 >= e2`)
* logical
  - negation (`not e`)
  - conjunction, disjunction (`e1 /\ e2`, `e1 \/ e2`)
* others
  - let binding (`let a = e1 in e2`)
  - function binding (`letfun foo (arg1:int, arg2:int) -> int = e1 in e2`, or so on)
  - conditional (`if e1 then e2 else e3`)
  - function application (`f arg1 arg2`, or so on)
  - explicit typed (`e : type`)

### Types

Following types are available:

* types for expressions `τ`
  - `{x:int | φ}` : any integer `x` that satisfy the logical formula `φ`
  - `{x:bool| φ}` : any boolean `x` that satisfy the logical formula `φ`
  - `x:int` : abbreviation of `{x:int | true}`
  - `x:bool` : abbreviation of `{x:bool | true}`
  - `int` : abbreviation of `{_:int | true}`
  - `bool` : abbreviation of `{_:bool | true}`
* types for functions `σ`
  - `(τ1, τ2) -> τ3` : any function that receives a value of `τ1` and a value of `τ2` as arguments and return a value of `τ3`
  - `τ1 -> τ2` : abbreviation of `(τ1) -> τ2`

### Logical Formula `φ`

Following logical formulas are available:

* formula variable (`x`, or so on)
* top, bottom (`true`, `false`)
* negation (`not φ`)
* conjunction, disjunction, implication (`φ1 /\ φ2`, `φ1 \/ φ2`, `φ1 => φ2`)
* comparison (`t1 = t2`, `t1 < t2`, `t1 <= t2`, `t1 >= t2`, `t1 > t2`)

### Logical Term `t`

Following logical terms are available:

* term variable (`x`, or so on)
* integer (`42`, `-12`, or so on)
* addition, subtraction, multiplication, division (`t1 + t2`, `t1 - t2`, `t1 * t2`, `t1 / t2`)
* probability (`Prob(φ)`)

## License

MIT License.  

We provide no warranty or support for this implementation. Each model performance is not guaranteed, and may not achieve the score reported in each paper. Use it at your own risk.

Please see the [LICENSE](./LICENSE) file for details.


