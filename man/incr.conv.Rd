\name{incr.conv}

\alias{incr.conv}

\title{Specify an Increasing and Convex Shape-Restriction in a SHAPEREG Formula}

\description{A symbolic routine to define that the mean vector is increasing and convex in a predictor in a formula argument to coneproj.}

\usage{incr.conv(x)}

\arguments{
  \item{x}{A numeric predictor which has the same length as the response vector.}
}

\value{The vector x with two attributes, i.e., name: the name of x and shape: 5 ("increasing and convex").}

\details{
  "incr.conv" returns the vector "x" and imposes on it two attributes: name and shape. 
  
   The shape attribute is 5 ("increasing and convex"), and according to the value of the vector itself and this attribute, the cone edges of the cone generated by the constraint matrix, which constrains the relationship between the mean vector and "x" to be increasing and convex,  will be made. The cone edges are a set of basis employed in the hinge algorithm. 

  Note that "incr.conv" does not make the corresponding cone edges itself. It sets things up to a subroutine called makedelta in coneproj.

  See references cited in this section for more details.
}

\references{
   Meyer, M. C. (2013b) A simple new algorithm for quadratic programming with applications in statistics. \emph{Communications in Statistics \bold{42(5)}}, 1126--1139. 
}

\author{
  Mary C. Meyer and Xiyue Liao
}

\examples{
  data(cubic)

  # extract x
  x <- cubic$x

  # extract y
  y <- cubic$y

  # regress y on x with the shape restriction: "increasing" and "convex"
  ans <- shapereg(y ~ incr.conv(x))

  # make a plot
  par(mar = c(4, 4, 1, 1))
  plot(x, y, cex = .7, xlab = "x", ylab = "y")
  lines(x, fitted(ans), col = 2)
  legend("topleft", bty = "n", "shapereg: increasing and convex fit", col = 2, lty = 1)


}

\seealso{
  \code{\link{incr.conc}}, \code{\link{incr}}
}

\keyword{shape routine}
