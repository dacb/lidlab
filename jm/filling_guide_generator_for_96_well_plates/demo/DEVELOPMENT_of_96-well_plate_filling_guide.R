#### Import packages ####
# most of these packages aren't used but are included because they are Janet's favorites. 
# library(reshape)  # for melt and cast
library(reshape2)  # cast was depreciated.  Replaced with dcast and acast
#library(ggplot2)
library(RCurl) # necessary for getURL below
library(plyr)
library(stringr)

#getwd()

#specify the web page for the R file.  NOTE:  want the "raw." part at the beginning!
#source.path <- "https://raw.githubusercontent.com/dacb/lidlab/master/jm/plate_reader_massager/SoftMaxPro_endpoint_paste_massager.R" 

# use getURL to get the R script
# eval(expr = parse( text = getURL(source.path, ssl.verifypeer=FALSE) )) 

# specify the path to the data you want to use
path.csv <- "./raw_data/2014_05_30.csv"

# read the csv in
d <- read.csv(path.csv)
head(d)

# trim_columns <- function(dataframe, cols) {
#   dataframe.name <- as.character(substitute(dataframe))
#   print(paste("data frame being manipulated: ", dataframe.name, sep=""))
#   print("columns to format as 96-well plate:")
#   #print(colnames(dataframe))
#   print(cols)
#   d <- subset(dataframe, select=cols)
#   head(d)
#   return(d)
# }
# 
# d <- trim_columns(d, c("purification.well", "source.flask", "mL.culture", "uL.resin",
#                        "mM.NaCl.for.lysis.and.wash", "uL.0mM.NaCl.lysis.buffer",
#                        "uL.500mM.NaCl.lysis.buffer", "mM.NaCl.in.lysis.and.first.two.washes", "uL.slurry.to.add"))
# head(d)

row_col_prep <- function(dataframe,well.identifier) {
  #d <- subset(dataframe, select=c(well.identifier, col))
  d.name <- as.character(substitute(dataframe))
  print(head(d))
  print("well used as ID for arranging into 96-well block format:")
  print(d[,well.identifier]) # this is how you pass a data frame column name into a function.
  #print(d$purification.well)
  d$row <- str_extract(d[,"purification.well"],"[[:upper:]]")
  d$col <- str_extract(d[,"purification.well"],"[[:digit:]]+")
  #d$col <- as.numeric(d$col)  # I don't think I need this. 
  d$col <- factor(d$col, c(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12)) # setting order of factors to 1, 2, 3,...12 not 1 10 11 12, 2, 3, 4, ...
  print(d)
  return(d)
}
d2 <- row_col_prep(d, well.identifier = "purification.well")
d2$col


#(d, well.identifier="purification.well", "mL.culture")
head(d2,12)
d2$col
#d3$col <- factor(d2$col, c(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12))

### figure out how to do it better  ###

filling_guide_96_well_arranger_1_col <- function(dataframe, col) {
  loading_guide_label <- c(col, "", "", "", "", "", "", "", "", "", "", "", "", "")
  loading_guide <- dcast(dataframe, row~col, value.var = col)
  #melt(dataframe, id.vars=c(""))  
  return(rbind(loading_guide_label, loading_guide))
}
filling_guide_96_well_arranger_1_col(d2, "mL.culture")

Erick <- function(df, col) {
  #print(head(df))
  print(head(df[,col])) #this is how you pass a column name into a function
}

Erick(df=d2, col="purification.well")
lapply(c("purification.well"), function(colname) {Erick(d2, colname)})
x=lapply(c("purification.well"), function(colname) {Erick(d2, colname)})
x  # works on one column
y=lapply(c("purification.well", "source.flask"), function(colname) {Erick(d2, colname)})
y  # works on two columns
z=lapply(c("purification.well", "source.flask"), function(colname) {filling_guide_96_well_arranger_1_col(d2, colname)})
z
# use do.call and rbind to make one dataframe out of the list of dataframes. 
do.call(rbind.data.frame, z)  # from http://stackoverflow.com/questions/4227223/r-list-to-data-frame
product <- do.call(rbind, z)  # also works 
product

# Now I want one function that takes a raw data_frame, d, and gives me something like product. 
guide_creator <- function(df, cols, well.identifier) {
  # first prepare the dataframe
  d <- row_col_prep(df, well.identifier)
  d2 <- lapply(cols, function(colname) {filling_guide_96_well_arranger_1_col(d2, colname)})
  d3 <- do.call(rbind, d2)
  return(d3)
}
guide_creator(d, c("source.flask", "mL.culture", "uL.resin"), well.identifier="purification.col")

# Now I want one function that doesn't depend on other functions. 
guide_creator_2 <- function(df, cols, well.identifier) {
  # first prepare the dataframe
  d <- df
  d$row <- str_extract(d[,"purification.well"],"[[:upper:]]")
  d$col <- str_extract(d[,"purification.well"],"[[:digit:]]+")
  d$col <- factor(d$col, c(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12)) # setting order of factors to 1, 2, 3,...12 not 1 10 11 12, 2, 3, 4, ...
  # do the equivalent of lapply(cols, function(colname) {filling_guide_96_well_arranger_1_col(d2, colname)})
  d2 <- lapply(cols, function(colname) {
    loading_guide_label <- c(colname, "", "", "", "", "", "", "", "", "", "", "", "", "")
    loading_guide <- dcast(d, row~col, value.var = colname)
    return(rbind(loading_guide_label, loading_guide))
  })
  # do the equibalent of:  do.call(rbind, d2)
  d3 <- do.call(rbind, d2)
  return(d3)
}
guide_creator_2(d, c("source.flask", "mL.culture", "uL.resin"), well.identifier="purification.col")

###### OLD #####



save.purification.well <- dcast(d2, row~col, value.var = "purification.well")
save.purification.well
save.source.flask <- dcast(d2, row~col, value.var = "source.flask")
save.source.flask
save.mL.culture <- dcast(d2, row~col, value.var = "mL.culture")
save.mL.culture
save.uL.resin <- dcast(d2, row~col, value.var = "uL.resin")
save.uL.resin
save.mM.NaCl.for.lysis.and.wash <- dcast(d2, row~col, value.var = "mM.NaCl.for.lysis.and.wash")
save.mM.NaCl.for.lysis.and.wash 
save.uL.0mM.NaCl.lysis.buffer <- dcast(d2, row~col, value.var = "uL.0mM.NaCl.lysis.buffer")
save.uL.0mM.NaCl.lysis.buffer
save.uL.500mM.NaCl.lysis.buffer <- dcast(d2, row~col, value.var = "uL.500mM.NaCl.lysis.buffer")
save.uL.500mM.NaCl.lysis.buffer
save.mM.NaCl.in.lysis.and.first.two.washes <- dcast(d2, row~col, value.var = "mM.NaCl.in.lysis.and.first.two.washes")
save.mM.NaCl.in.lysis.and.first.two.washes
save.uL.slurry.to.add <- dcast(d2, row~col, value.var = "uL.slurry.to.add")
save.uL.slurry.to.add

output_140530 <- rbind(c("purification.well", "", "","","","","","","","","","", ""),
                       save.purification.well, 
                       c("source.flask", "", "","","","","","","","","","", ""), 
                       save.source.flask,
                       c("mL.culture", "", "","","","","","","","","","", ""), 
                       save.mL.culture,
                       c("uL.resin", "", "","","","","","","","","","", ""), 
                       save.uL.resin,
                       c("mM.NaCl.for.lysis.and.wash", "", "","","","","","","","","","", ""), 
                       save.mM.NaCl.for.lysis.and.wash,
                       c("uL.0mM.NaCl.lysis.buffer", "", "","","","","","","","","","", ""), 
                       save.uL.0mM.NaCl.lysis.buffer,
                       c("uL.500mM.NaCl.lysis.buffer", "", "","","","","","","","","","", ""), 
                       save.uL.500mM.NaCl.lysis.buffer,
                       c("mM.NaCl.in.lysis.and.first.two.washes", "", "","","","","","","","","","", ""), 
                       save.mM.NaCl.in.lysis.and.first.two.washes,
                       c("uL.slurry.to.add", "", "","","","","","","","","","", ""), 
                       save.uL.slurry.to.add
)
output_140530
write.csv(output_140530, file= "./output/output_140530.csv")

filling_guide_96_well_arranger_multi_column(d2,c("source.flask", "mL.culture","uL.resin"))
lst <- c("source.flask", "mL.culture")  #c("source.flask", "mL.culture","uL.resin")
lapply(lst, filling_guide_96_well_arranger_1_col)
filling_guide_96_well_arranger_1_col(d2, "source.flask")
filling_guide_96_well_arranger_1_col(d2, "mL.culture")
mapply(filling_guide_96_well_arranger_multi_column, dataframe=d2, col=lst)
mapply(filling_guide_96_well_arranger_1_col, )
do.call(filling_guide_96_well_arranger_1_col, d, as.list(lst))

d2$purification.well
lapply(c("purification.well", "source.flask"), function(colname) { Erick(d2, colname)})

f <- data.frame(A=1:10, B=2:11, C=3:12)
fun1 <- function(x, column){
  max(x[,column])
}
fun1(df, "B")
fun1(df, c("B","A"))


apply(lst, filling_guide_96_well_arranger_multi_column)


gamma <- 1:3 
beta <- 1:4
expand.grid(gamma=gamma, beta=beta)
mouse <- expand.grid("d2", lst)
mapply(filling_guide_96_well_arranger_1_col, dataframe=mouse$Var1, col=mouse$Var2)

mapply(filling_guide_96_well_arranger_1_col, col=mouse, MoreArgs=list(dataframe="d2"))



# save to a csv
write.csv(d_massaged, "./output/demo_output.csv")

# USING PATHS TO ACCESS A COPY OF SoftMax_pro_endpoiint_paste_massager

# To run the function by loading a file from a relative path: 
# (1) set the path below and uncomment the line
#path.massager <- "../../../../../../../R_functions/SoftMaxPro_endpoint_paste_massager.R"
# (2) 
# set the path to the CSV you want to run the function on.
  # In this case it is the path below.  Uncomment the line.  
# path.csv <- "./raw_data/2014_05_14.csv"i
# (3) read in the data in the csv  (uncomment line below)
# d <- read.csv(path.csv)
# d  # optional: print the raw CSV that hasn't been manipulated before. 
# (4) use the source command to load the function plate_reader_plate_massager (uncomment again)
#source(path.massager, local=FALSE, echo=FALSE,chdir=FALSE, print.eval=FALSE)
# (5) run the function on the data frame.  You could save it by d2 <- plate_reader_paste_massager(d)
# plate_reader_paste_massager(d)   # or just print it to the screen with this command. 




