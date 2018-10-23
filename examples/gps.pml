let a = rand(-10, 10) in
let b = a + rand(0, 10) in
 not (b-a >= 10) /\ (b + rand(0, 5) - (a + rand(0, 5)) >= 10)
    : {x:bool | Prob(x) <= 1/10}
