#lang racket

(require racket/unsafe/ops)

(letrec ((unboxed-real-1 1.0)
         (unboxed-imag-2 2.0)
         (unboxed-real-3 2.0)
         (unboxed-imag-4 4.0)
         (unboxed-real-5 3.0)
         (unboxed-imag-6 6.0)
         (unboxed-real-7 (unsafe-fl+ unboxed-real-3 unboxed-real-5))
         (unboxed-imag-8 (unsafe-fl+ unboxed-imag-4 unboxed-imag-6))
         (f (lambda (x) (f x))))
  (let* ((unboxed-real-9 (unsafe-fl+ unboxed-real-1 unboxed-real-7))
         (unboxed-imag-10 (unsafe-fl+ unboxed-imag-2 unboxed-imag-8)))
      (unsafe-make-flrectangular unboxed-real-9 unboxed-imag-10)))
(void)