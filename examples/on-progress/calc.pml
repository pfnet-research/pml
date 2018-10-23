letfun main n:int -> {x:int | Prob(x >= 10*n) <= 1/100} =
    if n <= 0 then 1
    else rand(1, 10) + main (n - 1)
in
main 3

