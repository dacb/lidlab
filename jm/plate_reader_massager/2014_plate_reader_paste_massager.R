# This function takes a data frame and returns a data frame that is massaged into a nice format.
# It is for a csv that results from pasting a the results of SoftMaxPro reading a plate on endpoint mode with any number of shakings before reading it. 
# See the demo folder for a sample input and output file, as well as instructions for running it.  

plate_reader_paste_massager <- function (raw) {
  d <- raw
  d <- subset(d, select=c("X1","X2","X3","X4","X5","X6","X7","X8","X9","X10","X11","X12"))
  head(d)
  d <- d[rowSums(is.na(d)) != ncol(d),]  # remove rows that are all NA.
  # When you paste the data into a spreadsheet from SoftMaxPro,
    # the last set of numbers is the calculate number such as average or Vmax.  
  # Delete this last set, which comes with column labels.
  d <-  d[1:(nrow(d)-9),]  
       # trimmed off the last 9 rows, corresponding to the summary numbers at the bottom. 
  # rename the columns so they match the 1-12 numbering of 96-well plates. 
  colnames(d) <- c(1:12)
  head(d, 20)  # shows the first 20 rows of d in its current state
  # How many sets of data are there?
  n_measurements <- nrow(d)/8  # divide by 8 because there are 8 rows in a 96-well plate.
  #rep(c("A", "B", "C", "D", "E", "F", "G", "H"), 3)  # see how it works
  # add a column that labels each row. 
  d$row <- rep(c("A", "B", "C", "D", "E", "F", "G", "H"), n_measurements)
  d
  # now the rows and columns are labeled correctly.  
  # Next, add a column for the replicate number of the measurement.
  d$rep <- 1:nrow(d) 
  d$rep <- ceiling(d$rep/8)  
  # now I have a replicate number in the column rep
  d_melt <- melt(d, id.vars=c("row", "rep"), variable_name=c("col"))
  d_melt$well <- paste(d_melt$row, d_melt$col, sep="")
  head(d_melt, 15)
  d_melt <- subset(d_melt, select=-c(row, col))  # removes the columns named "row" and "col"
  #head(d_melt)
  return(d_melt)
}
