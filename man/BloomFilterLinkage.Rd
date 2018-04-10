\encoding{utf8}


\name{BloomFilterLinkage}
\alias{BloomFilterLinkage}
\alias{Multibit Tree}
\alias{bloom}

\title{Bloom Filter-based linkage using Multibit Trees}
\usage{
BloomFilterLinkage(IDA, dataA, IDB, dataB, blocking = NULL, similarity)
}
\description{
Linking Bloom Filters using Multibit Trees or Union-bit Trees.
}

\arguments{

\item{IDA}{A character vector or integer vector containing the IDs of the first data.frame.}
\item{dataA}{A data.frame containing Bloom Filters in the column specified in SelectSimilarityFunction.}
\item{IDB}{A character vector or integer vector containing the IDs of the second data.frame.}
\item{dataB}{A data.frame containing Bloom Filters in the column specified in SelectSimilarityFunction.}
%\item{threshold}{A numeric value giving the lower bound of the chosen similarity-coefficient.}
\item{similarity}{see \code{\link{SelectSimilarityFunctionBF}}}
\item{blocking}{see \code{\link{SelectBlockingFunction}}}
}

\details{
Two character vectors of Bloom Filters/CLKs including their IDs are compared using tree-based methods. An index tree is built from the first input (input A). The second input (B) is queried sequentially; all record pairs over the set threshold will be considered as links by the algorithm.

To call BloomFilterLinkage it is necessary to select the similarity function in {\code{\link{SelectSimilarityFunctionBF}}}. To use external blocking, calling \code{\link{SelectBlockingFunction}} is required.

}

\value{
A data.frame containing ID-pairs considered as links and their respective similarity values.
}



\references{

Bachteler, T., Reiher, J., Schnell, R. (2013): Similarity Filtering with Multibit Trees for Record Linkage. German Record Linkage Center Working Paper WP-GRLC-2013-01.

Kristensen, T. G., Nielsen, J., Pedersen, C. N. (2010): A Tree-based Method for the Rapid Screening of Chemical Fingerprints. Algorithms for Molecular Biology 5(9).

Schnell, R. (2014): An efficient Privacy-Preserving Record Linkage Technique for Administrative Data and Censuses. Journal of the International Association for Official Statistics (IAOS) 30: 263-270.

}

\examples{
# load test data
testFile <- file.path(path.package("PPRL"), "extdata/testdata.csv")
testData <- read.csv(testFile, head = FALSE, sep = "\t", colClasses = "character")

# create Bloom Filters
testDataBF <- CreateBF(ID = testData$V1, testData$V7,
  k = 20, padding = 1, q = 2, l = 1000, password = "(H]$6Uh*-Z204q")

# define bloom filter column in data and select similarity function and threshold
lBF <- SelectSimilarityFunctionBF("CLKs","CLKs", method = "mtan",
  threshold = 0.85)

# calculate result (in this example data is linked to itself)
linked <- BloomFilterLinkage(testDataBF$ID, testDataBF, testDataBF$ID, testDataBF,
  blocking = NULL, similarity = lBF)
}

\seealso{
  \code{\link{PPRL}},
  \code{\link{SelectBlockingFunction}},
  \code{\link{SelectSimilarityFunctionBF}},
  \code{\link{StandardizeString}}
}
