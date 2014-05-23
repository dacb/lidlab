# This function takes a data frame and returns a data frame that is just a CSV that has been loaded
  # and massages it into a nice format.
# Save this script into a folder.
# To operate the function on a csv of your choice, do something like:
#  source(../../)  (incomplete)
plate_reader_paste_massager <- function (raw) {
  d <- raw
  d <- subset(d, select=c("X1","X2","X3","X4","X5","X6","X7","X8","X9","X10","X11","X12"))
  head(d)
  d <- d[rowSums(is.na(d)) != ncol(d),]  # remove rows that are all NA.
  # When you paste the data into a spreadsheet from SoftMaxPro, the last set of numbers is the calculate number such as average or Vmax.  
  # Delete this last set, which comes with column labels.
  nrow(d[1:(nrow(d)-9),])
  d <-  d[1:(nrow(d)-9),]  # trimmed off the last 9 rows, corresponding to the summary numbers at the bottom. 
  # rename the columns so they match the 1-12 numbering of 96-well plates. 
  colnames(d) <- c(1:12)
  d
  # How many sets of data are there?
  n_measurements <- nrow(d)/8
  #rep(c("A", "B", "C", "D", "E", "F", "G", "H"), 3)  # see how it works
  d$row <- rep(c("A", "B", "C", "D", "E", "F", "G", "H"), n_measurements)
  d
  # now the rows and columns are labeled correctly.  Next, add a column for the replicate number of the measurement.
  d$rep <- 1:nrow(d) 
  d$rep <- ceiling(d$rep/8)  
  # now I have a replicate number in the column rep
  d_melt <- melt(d, id.vars=c("row", "rep"), variable_name=c("col"))
  d_melt$well <- paste(d_melt$row, d_melt$col, sep="")
  d_melt <- subset(d_melt, select=-c(row, col))
  #head(d_melt)
  return(d_melt)
}
