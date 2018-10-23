letfun move_arm (angle:double, n:int) -> double =
  if n == 0 then angle
  else
    let v = rand(0, 3) in
       if v == 0 then
          move_arm (angle-1.0) (n-1)
       if v == 3 then
          move_arm (angle+1.0) (n-1)
       else
          move_arm angle (n-1)
in
move_arm 0 10 : {x:int | p(x <= -10) < 1/1000000}

