# package dependencies: reshape2, plyr, stringr

filling_guide_generator_for_96_well_plates <- function(df, cols, well.identifier) {
  # first prepare the dataframe
  d <- df  #df is short for data.frame, d is short for data or df
  # make a column called row that lists the row in the 96-well plate.  
  d$row <- str_extract(d[,well.identifier],"[[:upper:]]")  # uses the user-specified well-identifier to grab as in 11 from B11.
  d$col <- str_extract(d[,well.identifier],"[[:digit:]]+") # uses the user-specified well-identifier to grab as in B from B11.
  # fix the factors for merging. 
  d$col <- factor(d$col, c(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12)) # setting order of factors to 1, 2, 3,...12 not 1 10 11 12, 2, 3, 4, ...
  # do the equivalent of lapply(cols, function(colname) {filling_guide_96_well_arranger_1_col(d2, colname)})
  # make a new dataframe label by rbinding together the column name, and the dcast output that converts columns into 96-well 8*12 format. 
  d2 <- lapply(cols, function(colname) {
    loading_guide_label <- c(colname, "", "", "", "", "", "", "", "", "", "", "", "", "") # prepare a row that describes the numbers that will appear after it. 
    loading_guide <- dcast(d, row~col, value.var = colname)  # converts a column of 96 wells into a 12-wide, 8 long dataframe
    return(rbind(loading_guide_label, loading_guide))  # this is what the function gives back.  It is ready to be saved as a CSV and colored in gDocs/excel. 
  })
  # do the equibalent of:  do.call(rbind, d2)
  d3 <- do.call(rbind, d2) # convert a list of data.frames into one dataframe. 
  return(d3)
}
# SAMPLE USE: 
# function_96_well_plate_filling_guide_creator(d, c("source.flask", "mL.culture", "uL.resin"), well.identifier="purification.col")
  ## columns it will format = "source.flask", "mL.culture", "uL.resin"
  ## purification.col is the column that describes the well (A1, B5, D12, etc.)
  ## The function returns a data.frame you can save with write.csv
