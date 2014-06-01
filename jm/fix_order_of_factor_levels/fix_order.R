# package dependencies: reshape2, plyr, stringr

# function for ordering the factors so they facet and appear in the legend in the order they appear in your spreadsheet
# Use example: well_identities$substrate <- fix_order(df = well_identities, col = "substrate")
fix_order <- function(df, col) {
  df_col <- df[,colnames(df) == col]
  print(paste("column being fixed: ",col))
  pr <- do.call(paste, as.list(dQuote(levels(df_col))))  # formats the factors as one string with quotes around each factor
  print(paste("levels before factoring: ", pr, sep="")) 
  df_col <- factor(df_col, unique(df_col))
  pr <- do.call(paste, as.list(dQuote(levels(df_col))))
  print(paste("levels *after* factoring: ", pr, sep="")) 
  #print(head(df_col))
  return(df_col)  
}


# SAMPLE USE: 
# dataframe$colname <- fix_order(dataframe, "colname")
