# This function takes a data frame and returns a data frame that is massaged into a nice format.
# It is for a csv that results from pasting a the results of SoftMaxPro reading a plate on endpoint mode with any number of shakings before reading it. 
# See the demo folder for a sample input and output file, as well as instructions for running it. 
# use package reshap2 for melt, not reshape.  

plate_reader_paste_massager <- function (raw) {
  d <- raw
  d <- subset(d, select=c("Time.hh.mm.ss.", "X1","X2","X3","X4","X5","X6","X7","X8","X9","X10","X11","X12"))
  head(d)
   # When you paste the data into a spreadsheet from SoftMaxPro,
    # it sometimes pastes an extra set of numbers corresponding to the summary number calculated by the software
    # This set of numbers shows up when you have the SoftMaxPro software set to display the summary number for each well.
    # This set of numbers needs to be deleted from the data when present 
  # Delete the summary numbers if present
    # the summary numbers are always accompanied by "Time(hh:mm:ss)" n the first column.  
    # We want to delete the rows at or below that string.  
  # if the string "Time(hh:mm:ss)" is present, return d with the bottom rows trimmed off.  Otherwise return d. 
  if(length(which(d$Time.hh.mm.ss.=="Time(hh:mm:ss)"))==1) print('Time(hh:mm:ss) is present') else print('Time(hh:mm:ss) is absent')
  exra.rows.beginning <- which(d[,1]== "Time(hh:mm:ss)") 
  print(paste("extra summary rows begin at row: ", exra.rows.beginning, sep=""))
  d <- if(length(which(d$Time.hh.mm.ss.=="Time(hh:mm:ss)"))==1) 
    d[1:exra.rows.beginning-1,] else d
  # remove rows where all entries are either "" or NA
    # First change all the "" values to NA
  d[d==""] <- NA   #df[df == 0] <- NA  http://stackoverflow.com/questions/11036989/replace-all-0-values-to-na
     # then 
  d <- d[rowSums(is.na(d)) != ncol(d),]  # remove rows that are all NA.  #  d <- na.omit(d) didn't work for me. 
  # remove the first column, which was the time values.
  print(colnames(d))
  d <- subset(d, select=-c(Time.hh.mm.ss.))
  colnames(d) <- c(1:12)
  # rename the columns so they match the 1-12 numbering of 96-well plates. 
  # How many sets of data are there?
  n_measurements <- nrow(d)/8  # divide by 8 because there are 8 rows in a 96-well plate.
  #rep(c("A", "B", "C", "D", "E", "F", "G", "H"), 3)  # see how it works
  # add a column that labels each row. 
  d$row <- rep(c("A", "B", "C", "D", "E", "F", "G", "H"), n_measurements)
  # now the rows and columns are labeled correctly.  
  # Next, add a column for the replicate number of the measurement.
  d$rep <- 1:nrow(d) 
  d$rep <- ceiling(d$rep/8)  
  # now I have a replicate number in the column rep
  d_melt <- melt(d, id.vars=c("row", "rep"), variable_name="col") 
      # in the old melt function in the reshape package, it was variable.name not variable_name
  d_melt$well <- paste(d_melt$row, d_melt$col, sep="")
  head(d_melt, 15)
  #d_melt <- subset(d_melt, select=-c(row, col))  # removes the columns named "row" and "col"
  #head(d_melt)
  return(d_melt)
#return(d)
}
