letfun main
  ({turn:int|turn>=0}
  {b1:double|b1>=0.0} {b2:double|b2>=0.0} {b3:double|b3>=0.0})
  -> {x:bool| Prob(x) >= 0.1}
=
  if b1 > 10.0 \/ b2 > 10.0 \/ b3 > 10.0 then
    false (* overflow ... *)
  else if b1 == 0 /\ b2 == 0 /\ b3 == 0 then
    true (* complete!! *)
  else if turn == 0 then (* task consumer *)
    let move = rand(1, 3) in
    if move == 1 then      main 2 0.0 b2 b3
    else if move == 2 then main 2 b1 0.0 b3
    else                   main 2 b1 b2 0.0
  else (* task distributer *)
    let move = rand(1, 3) in
    if move == 1 then      main (turn-1) (b1+1.0) b2 b3
    else if move == 2 then main (turn-1) b1 (b2+1.0) b3
    else                   main (turn-1) b1 b2 (b3+1.0)
in main 2 1.0 1.0 1.0

