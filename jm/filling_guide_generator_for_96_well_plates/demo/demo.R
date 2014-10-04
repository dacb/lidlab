#### Import packages ####
# most of these packages aren't used but are included because they are Janet's favorites. 
#library(reshape)  # for melt and cast
library(reshape2)  # cast was depreciated.  Replaced with dcast and acast
#library(ggplot2)
library(RCurl) # necessary for getURL below
library(plyr)
library(stringr)

#### run function ####
#getwd()
# specify where the csv is saved
path.csv <- "./raw_data/demo_data.csv"
d <- read.csv(path.csv)
head(d)  # shows top of d


# There are two ways to source an R function in a file.  You can secify an absolute or relative path (see below) *or* get it from github via the command getURL in the package RCurl or by specifying the path to the .R file that contains the function (see below)

# USING THE INTERNET (and getURL from RCurl):  #This is how Janet will use it.  Note, requires internet access!
  # advantage: uses the most current R file, and can be accessed from any computer.   

#specify the web page for the R file.  NOTE:  want the "raw." part at the beginning!
source.path <- "https://raw.githubusercontent.com/dacb/lidlab/master/jm/filling_guide_generator_for_96_well_plates/filling_guide_generator_for_96_well_plates.R" 

# use getURL to get the R script
eval(expr = parse( text = getURL(source.path, ssl.verifypeer=FALSE) )) 
# to run 
cols_for_demo <- c("purification.well","source.flask", "mL.culture", "uL.resin",
                   "mM.NaCl.for.lysis.and.wash", "uL.0mM.NaCl.lysis.buffer", 
                   "uL.500mM.NaCl.lysis.buffer", "mM.NaCl.in.lysis.and.first.two.washes", 
                   "mM.NaCl.in.last.2.washes", "uL.slurry.added", "uL.slurry.to.add")
result <- filling_guide_generator_for_96_well_plates(d, cols_for_demo, "purification.well")   # or just print it to the screen with this command. 
head(result)
# write.csv works, but I wanted to omit the row and column names stored in result for saving to a csv, so write.table worked better. 
write.table(result, "./output/demo_result.csv", sep=",", row.names=FALSE, col.names=FALSE)

# or if you had a file without plate numbers:
cols_for_demo <- c("well.position","source.flask", "mL.culture", "uL.resin",
                   "mM.NaCl.for.lysis.and.wash", "uL.0mM.NaCl.lysis.buffer", 
                   "uL.500mM.NaCl.lysis.buffer", "mM.NaCl.in.lysis.and.first.two.washes", 
                   "mM.NaCl.in.last.2.washes", "uL.slurry.added", "uL.slurry.to.add")
result2 <- filling_guide_generator_for_96_well_plates(d, cols_for_demo, "well.position")   # or just print it to the screen with this command. 

head(result2)

# save to a csv
#write.csv(d_massaged, "./output/demo_output.csv")

# USING PATHS TO ACCESS A COPY OF SoftMax_pro_endpoiint_paste_massager

# To run the function by loading a file from a relative path: 
# (1) set the path below and uncomment the line
path.function <- "../filling_guide_generator_for_96_well_plates.R"
# (2) 
# set the path to the CSV you want to run the function on.
  # In this case it is the path below.  Uncomment the line.  
path.csv <- "./raw_data/2014_05_30.csv"
# (3) read in the data in the csv  (uncomment line below)
d <- read.csv(path.csv)
head(d)
# d  # optional: print the raw CSV that hasn't been manipulated before. 
# (4) use the source command to load the function (uncomment again)
source(path.function, local=FALSE, echo=FALSE,chdir=FALSE, print.eval=FALSE)
# (5) run the function on the data frame.  You could save it by d2 <- plate_reader_paste_massager(d)
colnames(d)
cols_for_demo <- c("purification.well","source.flask", "mL.culture", "uL.resin",
                   "mM.NaCl.for.lysis.and.wash", "uL.0mM.NaCl.lysis.buffer", 
                   "uL.500mM.NaCl.lysis.buffer", "mM.NaCl.in.lysis.and.first.two.washes", 
                   "mM.NaCl.in.last.2.washes", "uL.slurry.added", "uL.slurry.to.add")
result <- filling_guide_generator_for_96_well_plates(d, cols_for_demo, "purification.well")   # or just print it to the screen with this command. 
# write.csv works, but I wanted to omit the row and column names stored in result for saving to a csv, so write.table worked better. 
head(result)
write.table(result, "./output/demo_result.csv", sep=",", row.names=FALSE, col.names=FALSE)

