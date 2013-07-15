\name{coneA}
\alias{coneA}
\title{Cone Projection -- Polar Cone}
\description{
This routine implements the hinge algorithm for cone projection to minimize 
\eqn{||y - \theta||^2} over the cone \eqn{C} of the form \eqn{\{\theta: A\theta \ge 0\}}. }
\usage{coneA(y, amat, w = NULL, lst = TRUE)  }
\arguments{
  \item{y}{A vector of length \eqn{n}.}
  \item{amat}{A constraint matrix. The rows of amat must be irreducible. The column number of amat must equal the length of \eqn{y}.}
  \item{w}{An optional nonnegative vector of weights of length \eqn{n}. If w is not given, all weights are taken to equal 1. Otherwise, the minimization of \eqn{(y - \theta)'w(y - \theta)} over \eqn{C} is returned. The default is w = NULL.}
  \item{lst}{A logical scalar. If lst == TRUE, a list of df, thetahat, steps, message and convergence is returned; Otherwise, message and convergence are omitted. The default is lst = TRUE.}  }

\details{
The routine coneA dynamically loads a C++ subroutine "coneACpp". The rows
of \eqn{- A} are the edges of the polar cone \eqn{\Omega^o}. This routine first projects \eqn{y} onto \eqn{\Omega^o} to get the residual of the projection onto the constraint cone \eqn{C}, and then uses the fact that \eqn{y} is equal to the sum of the projection of \eqn{y} onto \eqn{C} and the projection of \eqn{y} onto \eqn{\Omega^o} to get the estimation of \eqn{\theta}. See references cited in this section for more details about the relationship between polar cone and constraint cone. }

\value{
\item{df}{The dimension of the face of the constraint cone on which the projection lands.}
\item{thetahat}{The projection of  \eqn{y} on the constraint cone.}
\item{steps}{The number of iterations before the algorithm converges.}
\item{message}{A warning message showing the convergence of the algorithm. If steps > \eqn{n^2}, \eqn{n} is the length of \eqn{y}, a warning message is given.}
\item{convergence}{A number showing the convergence of the algorithm. If convergence is 0,  the algorithm converges; If convergence is 1, the algorithm fails to converge.}  }

\references{
Meyer, M. C. (1999) An extension of the mixed primal-dual bases algorithm to the case of more constraints than 
dimensions. \emph{Journal of Statistical Planning and Inference \bold{81}}, 13--31.

Meyer, M. C. (2013b) A simple new algorithm for quadratic programming with applications in statistics. \emph{Communications
in Statistics \bold{42(5)}}, 1126--1139.  }

\author{
Mary C. Meyer and Xiyue Liao  }

\seealso{
\code{\link{coneB}}, \code{\link{constreg}}, \code{\link{qprog}}  }

\examples{
#generate y
    n <- 50
    y <- rnorm(n) + sort(rnorm(n))

#create the constraint matrix to make y monotonically increasing
    n <- 50
    amat <- matrix(0, n - 1, n)
    for(i in 1:(n - 1)){
        amat[i, i] <- -1; amat[i, i + 1] <- 1
    }

#call coneA
    ans1 <- coneA(y, amat)
    ans2 <- coneA(y, amat, w = (1:n)/n)

#make a plot to compare the unweighted fit and the weighted fit
    par(mar = c(4, 4, 1, 1))
    plot(y, cex = .7, ylab = "y")
    lines(ans1$thetahat, col = 2, lty = 2)
    lines(ans2$thetahat, col = 4, lty = 2)
    legend("topleft", c("unweighted fit", "weighted fit"), col = c(2, 4), lty = c(2, 2))
    title("ConeA Example Plot")  }
