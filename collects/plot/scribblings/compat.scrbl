#lang scribble/manual
@(require (for-label racket
                     racket/gui/base
                     plot/compat)
          plot/compat
          (only-in unstable/latent-contract/defthing
                   doc-apply))

@title[#:tag "compat"]{Compatibility Module}

@author["Alexander Friedman" "Jamie Raymond" "Neil Toronto"]

@defmodule[plot/compat]

This module provides an interface compatible with PLoT 5.1.3 and earlier.

@bold{Do not use both @racketmodname[plot] and @racketmodname[plot/compat] in the same module.}
It is tempting to try it, to get both the new features and comprehensive backward compatibility.
But to enable the new features, the objects plotted in @racketmodname[plot] have to be a different data type than the objects plotted in @racketmodname[plot/compat].
They do not coexist easily, and trying to make them do so will result in contract violations.

@; ----------------------------------------

@section[#:tag "plot"]{Plotting}

@doc-apply[plot]{
Plots @racket[data] in 2D, where @racket[data] is generated by
functions like @racket[points] or @racket[line].

A @racket[data] value is represented as a procedure that takes a
@racket[2d-plot-area%] instance and adds plot information to it.

The result is a @racket[image-snip%] for the plot. If an @racket[#:out-file]
path or port is provided, the plot is also written as a PNG image to
the given path or port.

The @racket[#:lncolor] keyword argument is accepted for backward compatibility, but does nothing.
}

@doc-apply[plot3d]{
Plots @racket[data] in 3D, where @racket[data] is generated by a
function like @racket[surface]. The arguments @racket[alt] and
@racket[az] set the viewing altitude (in degrees) and the azimuth
(also in degrees), respectively.

A 3D @racket[data] value is represented as a procedure that takes a
@racket[3d-plot-area%] instance and adds plot information to it.

The @racket[#:lncolor] keyword argument is accepted for backward compatibility, but does nothing.
}

@doc-apply[points]{
Creates 2D plot data (to be provided to @racket[plot]) given a list
of points specifying locations. The @racket[sym] argument determines
the appearance of the points.  It can be a symbol, an ASCII character,
or a small integer (between -1 and 127).  The following symbols are
known: @racket['pixel], @racket['dot], @racket['plus],
@racket['asterisk], @racket['circle], @racket['times],
@racket['square], @racket['triangle], @racket['oplus], @racket['odot],
@racket['diamond], @racket['5star], @racket['6star],
@racket['fullsquare], @racket['bullet], @racket['full5star],
@racket['circle1], @racket['circle2], @racket['circle3],
@racket['circle4], @racket['circle5], @racket['circle6],
@racket['circle7], @racket['circle8], @racket['leftarrow],
@racket['rightarrow], @racket['uparrow], @racket['downarrow].
}


@doc-apply[line]{
Creates 2D plot data to draw a line.

The line is specified in either functional, i.e. @math{y = f(x)}, or
parametric, i.e. @math{x,y = f(t)}, mode.  If the function is
parametric, the @racket[mode] argument must be set to
@racket['parametric].  The @racket[t-min] and @racket[t-max] arguments
set the parameter when in parametric mode.
}


@doc-apply[error-bars]{
Creates 2D plot data for error bars given a list of vectors.  Each
vector specifies the center of the error bar @math{(x,y)} as the first
two elements and its magnitude as the third.
}


@doc-apply[vector-field]{
Creates 2D plot data to draw a vector-field from a vector-valued
function.
}


@doc-apply[contour]{
Creates 2D plot data to draw contour lines, rendering a 3D function
a 2D graph cotours (respectively) to represent the value of the
function at that position.
}

@doc-apply[shade]{
Creates 2D plot data to draw like @racket[contour], except using
shading instead of contour lines.
}


@doc-apply[surface]{
Creates 3D plot data to draw a 3D surface in a 2D box, showing only
the @italic{top} of the surface.
}


@defproc[(mix [data (any/c . -> . void?)] ...)
         (any/c . -> . void?)]{
Creates a procedure that calls each @racket[data] on its argument in
order. Thus, this function can composes multiple plot @racket[data]s
into a single data.
}

@doc-apply[plot-color?]{
Returns @racket[#t] if @racket[v] is one of the following symbols,
@racket[#f] otherwise:

@racketblock[
'white 'black 'yellow 'green 'aqua 'pink
'wheat 'grey 'blown 'blue 'violet 'cyan
'turquoise 'magenta 'salmon 'red
]}

@; ----------------------------------------

@section[#:tag "curve-fit"]{Curve Fitting}

@define[fit-warning]{
@para{
@bold{Do not use the @(racket fit) function. It is going to be removed in Racket 5.2.1.}
It relies on old C code that nobody understands or is willing to maintain, and that is also slightly crashy.
}}

@fit-warning

Quite independent of plotting, and for reasons lost in the sands of time,
the @racketmodname[plot] library provides a non-linear, least-squares
fit algorithm to fit parameterized functions to given data.
The code that implements the algorithm is public
domain, and is used by the @tt{gnuplot} package.

To fit a particular function to a curve:

@itemize[

  @item{Set up the independent and dependent variable data.  The first
  item in each vector is the independent variable, the second is the
  result.  The last item is the weight of the error; we can leave it
  as @racket[1] since all the items weigh the same.

    @racketblock[
      (define data '(#(0 3 1)
                     #(1 5 1)
                     #(2 7 1)
                     #(3 9 1)
                     #(4 11 1)))
      ]
    }

  @item{Set up the function to be fitted using fit.  This particular
  function looks like a line.  The independent variables must come
  before the parameters.

    @racketblock[
      (define fit-fun
        (lambda (x m b) (+ b (* m x))))
      ]
    }

  @item{If possible, come up with some guesses for the values of the
  parameters.  The guesses can be left as one, but each parameter must
  be named.}

  @item{Do the fit.

    @racketblock[
      (define fitted
        (fit fit-fun
             '((m 1) (b 1))
             data))
      ]
    }

  @item{View the resulting parameters; for example,

    @racketblock[
      (fit-result-final-params fitted)
      ]

    will produce @racketresultfont{(2.0 3.0)}.
    }

  @item{For some visual feedback of the fit result, plot the function
    with the new parameters. For convenience, the structure that is
    returned by the fit command has already the function.

    @racketblock[
      (plot (mix (points data)
                 (line (fit-result-function fitted)))
            #:y-max 15)
      ]}]

A more realistic example can be found in
@filepath{compat/tests/fit-demo-2.rkt} in the @filepath{plot} collection.

@defproc[(fit [f (real? ... . -> . real?)]
              [guess-list (list/c (list symbol? real?))]
              [data (or/c (list-of (vector/c real? real? real?))
                          (list-of (vector/c real? real? real? real?)))])
         fit-result?]{

@fit-warning
                       
Attempts to fit a @defterm{fittable function} to the data that is
given. The @racket[guess-list] should be a set of arguments and
values. The more accurate your initial guesses are, the more likely
the fit is to succeed; if there are no good values for the guesses,
leave them as @racket[1].}

@defstruct[fit-result ([rms real?]
                       [variance real?]
                       [names (listof symbol?)]
                       [final-params (listof real?)]
                       [std-error (listof real?)]
                       [std-error-percent (listof real?)]
                       [function (real? ... . -> . real?)])]{

The @racket[params] field contains an associative list of the
parameters specified in @racket[fit] and their values.  Note that the
values may not be correct if the fit failed to converge.  For a visual
test, use the @racket[function] field to get the function with the
parameters in place and plot it along with the original data.}

@; ----------------------------------------

@section{Miscellaneous Functions}

@defproc[(derivative [f (real? . -> . real?)] [h real? .000001])
         (real? . -> . real?)]{

Creates a function that evaluates the numeric derivative of
@racket[f].  The given @racket[h] is the divisor used in the
calculation.}

@defproc[(gradient [f (real? real? . -> . real?)] [h real? .000001])
         ((vector/c real? real?) . -> . (vector/c real? real?))]{

Creates a vector-valued function that the numeric gradient of
@racket[f].}

@defproc[(make-vec [fx (real? real? . -> . real?)] [fy (real? real? . -> . real?)])
         ((vector/c real? real?) . -> . (vector/c real? real?))]{

Creates a vector-values function from two parts.}
