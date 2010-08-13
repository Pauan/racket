#lang racket

(require racket/unsafe/ops)

;; invalid: f "escapes", according to our analysis
(letrec ((f (lambda (x)
              (let ((y f))
                x))))
  (f (let*-values (((unboxed-real-1) 1.0)
                   ((unboxed-imag-2) 2.0)
                   ((unboxed-real-3) 2.0)
                   ((unboxed-imag-4) 4.0)
                   ((unboxed-real-5) (unsafe-fl+ unboxed-real-1 unboxed-real-3))
                   ((unboxed-imag-6) (unsafe-fl+ unboxed-imag-2 unboxed-imag-4)))
       (unsafe-make-flrectangular unboxed-real-5 unboxed-imag-6))))
(void)