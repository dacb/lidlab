#### Import packages ####
library(reshape)  # so I can use cast
library(reshape2)  # cast was depreciated.  Replaced with dcast and acast
library(ggplot2)
library(grofit)
library(RCurl)
library(plyr)

# path of the function I want:
#   /Dropbox/R_functions/2014_plate_reader_paste_massager.R
# The path of the CSV I want to massage:
#   ./raw_data/2014_05_14 Test existing ACS library in 96-well plate - purify with high NaCl - raw_A280.csv

getwd()
path.massager <- "../../../../../../../R_functions/2014_plate_reader_paste_massager.R"
path.csv <- "./raw_data/2014_05_14.csv"
d <- read.csv(path.csv)
d
source(path.massager, local=FALSE, echo=FALSE,chdir=FALSE, print.eval=FALSE)
plate_reader_paste_massager(d)
