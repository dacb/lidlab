# #### load packages ####
library(reshape2)  # cast was depreciated.  Replaced with dcast and acast
library(RCurl) # for downloading function from github/web
# library(ggplot2)
# library(plyr)
# library(stringr)

##### Imporrt function #######
# A file can be sourced two ways: online and offline/local:
## (1) You can secify an absolute or relative path (see below)
### Save a copy of the file locally on your computer to access it.
## (2) Source from github via the command getURL in the package RCurl
### See instructions below.

# USING THE INTERNET (and getURL from RCurl):  #This is how Janet will use it.  Note, requires internet access (can't work on plane)!
# advantage: uses the most current R file, and can be accessed from any computer.

#specify the web page for the R file.  NOTE:  want the "raw." part at the beginning!
source.path <- "https://raw.githubusercontent.com/dacb/lidlab/master/jm/SpectraMax_190_plate_reader_data_importer/fun_import_raw_spectra.R"
# use getURL to get the R script
eval(expr = parse( text = getURL(source.path, ssl.verifypeer=FALSE) ))


##### Run the function #######
# grab the filename
filename_CSV <- grep(pattern = "more than 1 hr", x =  list.files("./raw_data"), value = T)
filename_CSV
# run the function
d <- fun_import_raw_spectra(filename_CSV, "./raw_data")

# write output to CSV:
dir.create("./demo_output")
write.table(d, "./demo_output/demo_result.csv", sep=",", row.names=FALSE)




