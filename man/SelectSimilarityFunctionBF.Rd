\encoding{utf8}


\name{SelectSimilarityFunctionBF}
\alias{SelectSimilarityFunctionBF}
\alias{Linking}
\alias{Trees}
\alias{Multibit Trees}

\title{
Select Similarity Function for Bloom Filter-based methods
}

\description{
To call \code{\link{BloomFilterLinkage}} it is mandatory to select a similarity function for each variable. Each element of the setup contains the two variable names and the method. For some methods further informations can be entered.
}
\usage{
SelectSimilarityFunctionBF(variable1, variable2, method = 'mtan',
  threshold = 0.85, windowSize = 5, looseThreshold = 0.8, tightThreshold = 0.7,
  symdex = TRUE, leaflimit = 3, cores = 1)
}

\arguments{
  \item{variable1}{name of linking variable 1 in the data.frame. The column must be of type character, numeric or integer, containing the data to be merged. The data vector must have the same length as the ID vector.}
  \item{variable2}{name of linking variable 2 in the data.frame. The column must be of type character, numeric or integer, containing the data to be merged. The data vector must have the same length as the ID vector.}
\item{method}{linking method. Possible values are:
\itemize{
  \item 'mtan' = Tanimoto Similarity/Jaccard Similarity for Bloom Filters/CLKs using Multibit trees

 \item  'mham' = Hamming distance for Bloom Filters/CLKs using Multibit trees

  \item 'utan' =  Tanimoto Similarity/Jaccard Similarity for Bloom Filters/CLKs using union bit trees

 \item  'uham' = Hamming distance for Bloom Filters/CLKs using union bit trees

 \item  'CCtan' = Tanimoto Similarity/Jaccard Similarity for Bloom Filters/CLKs using canopy clustering

 \item  'CCtanXOR' = Tanimoto Similarity/Jaccard Similarity for Bloom Filters/CLKs using canopy clustering  with XOR-filtering

 \item  'SNtan' = Tanimoto Similarity/Jaccard Similarity for Bloom Filters/CLKs using sorted nearest neighbourhood

 \item  'SNtanXOR' = Tanimoto Similarity/Jaccard Similarity for Bloom Filters/CLKs using sorted nearest neighbourhood with XOR-filtering
 }
  }
  \item{threshold}{Numeric value giving the lower bound of the Tanimoto-coefficient or the normalised Hamming distance to search for. Must be in the range of 0.0 <= threshold <=1. }
\item{symdex}{To deactivate symdex pre-processing, set \code{symdex = FALSE} for Multibit trees.}
\item{leaflimit}{Optional parameter for Multibit trees specifying the maximum number of Bloom Filters/CLKs in a leaf.}
\item{cores}{Optional parameter for Multibit trees specifying the number of parallel threads that shall be used to construct the search tree and perform the search within it.}
\item{looseThreshold}{Numeric value giving the loose threshold for canopy clustering}
\item{tightThreshold}{Numeric value giving the tight threshold for canopy clustering}
\item{windowSize}{Integer value giving the window size for sorted neighbourhood searching}
}
\value{
Calling the function will return a short confirmation message only.
}

\references{

Bachteler, T., Reiher, J., Schnell, R. (2013): Similarity Filtering with Multibit Trees for Record Linkage. German Record Linkage Center Working Paper WP-GRLC-2013-01.

Kristensen, T. G., Nielsen, J., Pedersen, C. N. (2010): A Tree-based Method for the Rapid Screening of Chemical Fingerprints. Algorithms for Molecular Biology 5(9).

Schnell, R. (2014): An efficient Privacy-Preserving Record Linkage Technique for Administrative Data and Censuses. Journal of the International Association for Official Statistics (IAOS) 30: 263-270.

Tai, D., Fang, J. (2012): SymDex: Increasing the Efficiency of Chemical Fingerprint Similarity Searches for Comparing Large Chemical Libraries by Using Query Set Indexing. Journal of Chemical Information and Modeling 52: 1926-1935.

}

\seealso{
  \code{\link{PPRL}}
  \code{\link{BloomFilterLinkage}},
  \code{\link{DeterministicLinkage}},
  \code{\link{ProbabilisticLinkage}},
  \code{\link{SelectBlockingFunction}},
  \code{\link{SelectSimilarityFunctionBF}},
  \code{\link{StandardizeString}}
}

\examples{
# load test data
testFile <- file.path(path.package("PPRL"), "extdata/testdata.csv")
testData <- read.csv(testFile, head = FALSE, sep = "\t", colClasses = "character")

# create Bloom Filters
testDataBF <- CreateBF(ID = testData$V1, testData$V7,
  k = 20, padding = 1, q = 2, l = 1000, password = "(H]$6Uh*-Z204q")

# define bloom filter column in data and select similarity function and threshold using
# multibit trees
lBF <- SelectSimilarityFunctionBF("CLKs","CLKs", method = "mtan", threshold = 0.85, symdex = TRUE,
leaflimit = 3, cores = 1)

# or

# define bloom filter column in data and select similarity function and threshold using
# canopy clustering
lBF <- SelectSimilarityFunctionBF("CLKs","CLKs", method = "CCtan", threshold = 0.85,
looseThreshold = 0.7, tightThreshold = 0.8)

# or

# define bloom filter column in data and select similarity function and threshold using
# sorted neighbourhood
lBF <- SelectSimilarityFunctionBF("CLKs","CLKs", method = "SNtan", threshold = 0.85, windowSize = 5)

# calculate result (in this example data is linked to itself)
linked <- BloomFilterLinkage(testDataBF$ID, testDataBF, testDataBF$ID, testDataBF,
  blocking = NULL, similarity = lBF)
}
