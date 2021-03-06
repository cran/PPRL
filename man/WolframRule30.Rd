\encoding{utf8}

\name{WolframRule30}

\alias{WolframRule30}
\alias{Rule 30}

\title{Apply Wolframs rule 30 on bit vectors}

\description{
Apply Wolframs Cellular Automaton rule 30 on the input bit vectors.
}

\usage{
WolframRule30(ID, data, lenBloom, t)
}

\arguments{
  \item{ID}{IDs as character vector.}
  \item{data}{character vector containing bit vectors.}
  \item{lenBloom}{length of Bloom filters.}
  \item{t}{indicates how often rule 30 is to be used.}
}

\value{
Returns a character vector with new bit vectors after rule 30 has been applied t times.
}

\references{
https://en.wikipedia.org/wiki/Rule_30


Schnell, R. (2017): Recent Developments in Bloom Filter-based Methods for Privacy-preserving Record Linkage. Curtin Institute for Computation, Curtin University, Perth, 12.9.2017.


Wolfram, S. (1983): Statistical mechanics of cellular automata. Rev. Mod. Phys. 55 (3): 601–644.

}


\seealso{
  
  \code{\link{WolframRule90}}
}

\examples{
# Load test data
testFile <- file.path(path.package("PPRL"), "extdata/testdata.csv")
testData <- read.csv(testFile, head = FALSE, sep = "\t",
  colClasses = "character")

# Create bit vector e.g. by CreateCLK or CreateBF
CLK <- CreateCLK(ID = testData$V1,
  data = testData[, c(2, 3, 7, 8)],
  k = 20, padding = c(0, 0, 1, 1),
  q = c(1, 1, 2, 2), l = 1000,
  password = c("HUh4q", "lkjg", "klh", "Klk5"))

# Apply rule 30 once
res <- WolframRule30(CLK$ID, CLK$CLK, lenBloom = 1000, t = 1)

}
