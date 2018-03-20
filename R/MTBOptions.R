# similarity options for MTB
# uses options_manager from package 'settings'
SelectSimilarityFunction <- function(variable1, variable2, method = 'jw', ind_c0 = FALSE, ind_c1 = FALSE ,
                                     m = 0.9, u = 0.1, p = 0.05, epsilon = 0.0004,
                                     lower = 0.0, upper = 0.0, threshold = 0.85, jaroWeightFactor = 1.0, lenNgram = 2)
  {

  methodlist <- c('exact', 'exactCL', 'LCS', 'lv', 'dl', 'jaro', 'jw', 'jw2', 'ngram', 'Gcp', 'Reth', 'Soundex', 'Metaphone', 'DoubleMetaphone', 'tanimoto')
  if (!is.element(method, methodlist)) {
    cat('Allowed methods are:\n')
    cat(methodlist, sep = ", ")
    stop(paste0('error: Option value method = \'',method,'\' out of range.'))
    method <- NULL
    return(NULL)
  }
  if (m>1.0 ||m<0.0 ||u>1.0 || u < 0.0) {
    stop(paste0('error: Option values \'m\' and \'u\' must be between 0.0 and 1.1.'))
    method <- NULL
    return(NULL)
  }

  MergeOptionsRes <- settings::options_manager(variable1 = variable1, variable2 = variable2, method = method,  ind_c0 = ind_c0, ind_c1 = ind_c1 ,
                                     m = m, u = u, p = p, epsilon = epsilon,  upper = upper, lower = lower, threshold = threshold, jaroWeightFactor = jaroWeightFactor, lenNgram = lenNgram)
  return(MergeOptionsRes)
  }

SelectBlockingFunction <- function(variable1, variable2, method )
{
  # check methods
  methodlist <- c('exact', 'exactCL', '0'  )
  if (!is.element(method, methodlist)) {
    cat(' Allowed methods are:\n')
    cat(methodlist, sep = ", ")
    stop('error: Option value method out of range.')
    method <- NULL
  }
  BlockingOptionsRes <- settings::options_manager(variable1 = variable1, variable2 = variable2, method = method)
  return(BlockingOptionsRes)
}


# similarity options for MTB
# uses options_manager from package 'settings'
SelectSimilarityFunctionBF <- function(variable1, variable2, method = 'mtan', threshold = 0.85, windowSize = 5,
                                       looseThreshold = 0.8, tightThreshold = 0.7, symdex = TRUE, leaflimit = 3,
                                       cores = 1)
{
  # check methods
  methodlist <- c('mtan', 'utan', 'mham', 'uham', 'CCtan', 'CCtanXOR', 'SNtan', 'SNtanXOR')

  if (!is.element(method, methodlist)) {
    cat('Allowed methods are:\n')
    cat(methodlist, sep = ", ")
    stop(paste0('error: Option value method = \'',method,'\' out of range.'))
    method <- NULL
    return(NULL)
  }

  MergeOptionsRes <- settings::options_manager(variable1 = variable1, variable2 = variable2, method = method,
                                     symdex = symdex, leaflimit = leaflimit, cores = cores,
                                     threshold = threshold, windowSize = windowSize, looseThreshold = looseThreshold, tightThreshold = tightThreshold)
  return(MergeOptionsRes)
}

