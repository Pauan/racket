(module let-float typed/scheme 
  (require racket/unsafe/ops)
  (let ((x (+ 3.0 2.0)))
    (* 9.0 x)))