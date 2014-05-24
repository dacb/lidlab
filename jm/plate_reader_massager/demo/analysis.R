#### Import packages ####
# most of these packages aren't used but are included because they are Janet's favorites. 
library(reshape)  # for melt and cast
#library(reshape2)  # cast was depreciated.  Replaced with dcast and acast
#library(ggplot2)
library(RCurl) # necessary for getURL below
library(plyr)

#getwd()

# There are two ways to source an R function in a file.  You can secify an absolute or relative path (see below) *or* get it from github via the command getURL in the package RCurl or by specifying the path to the .R file that contains the function (see below)

# USING THE INTERNET (and getURL from RCurl):  #This is how Janet will use it.  Note, requires internet access!
  # advantage: uses the most current R file, and can be accessed from any computer.   

#specify the web page for the R file.  NOTE:  want the "raw." part at the beginning!
source.path <- "https://raw.githubusercontent.com/dacb/lidlab/master/jm/plate_reader_massager/SoftMaxPro_endpoint_paste_massager.R" 

# use getURL to get the R script
eval(expr = parse( text = getURL(source.path, ssl.verifypeer=FALSE) )) 

# specify the path to the data you want to use
path.csv <- "./raw_data/2014_05_14.csv"

# read the csv in
d <- read.csv(path.csv)

# print the CSV to the screen (for RStudio)
d
#source(path.massager, local=FALSE, echo=FALSE,chdir=FALSE, print.eval=FALSE)
plate_reader_paste_massager(d)
d_massaged <- plate_reader_paste_massager(d)

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




