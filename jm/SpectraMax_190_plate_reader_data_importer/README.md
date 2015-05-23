The function massages data exported from the computer program SoftMax Pro into a more convenient format for plotting in R or other programs.  It was originally developed by Janet Matsen (JanetMatsen@gmail.com) for use with a machine/software pair of the specifications noted at the bottom of this file. 

The text file "fun_import_raw_spectra.R" contains a function called "fun_import_raw_spectra".
This function accepts scans done in all three modes: end-point, kinetic, and spectrum.    
After reading in raw .csv files, the function returns one data frame with one well scan per row.  Each measurement gets its own row.  One of the columns in the data frame corresponds to the name the user entered for each scan while using SoftMax Pro.

Requirements:
* Exported CSV from SoftMax Pro software: see example of CSV format [here](https://docs.google.com/spreadsheets/d/1ILyE9JzosQp_oiixHFNr2D6E2HL2Io8N0c4Hq1uk4_Y/edit#gid=0)
  * Note: the software does have two settings for export; make sure it looks somewhat like the link above, and see description of difference [here](http://openwetware.org/wiki/Lidstrom:_Molecular_Devices_Plate_Reader#Exporting_Data) 
* The melt function of the reshape2 package.
  * To install: install.packages("reshape2")
  * To load for each R session: library(reshape2)

Argument  | Requirements/Description
------------- | -------------
filename | The name of the CSV file that contains the raw output
path  |  The path to the folder in which the CSV lives

For example, if you have a file named data.csv, and it lives in the folder ./raw_data you would pass filename = "data.csv",  path = "./raw_data"
Don't enter path with the trailing slash like "./raw_data/" or an error will result. 

Limitations:
* In kinetic mode, the time intervals of reads must be consistent.  With our software, this is the only possibility.  

See the demo package for more detail.  

Required packages for demo:
library(reshape2)  # for the melt function used in the function
library(RCurl) # necesary to run getURL in the demo package

About our SoftMax Pro software & Molecular Devices SpectraMax190 software:
Software serial number: SMP500-03176XXXX

Instrument type: PLUS190PC
Instrument serial number: NNR0249
Instrument ROM version: 1.21 Aug 18 2005

Copyright 1999-2008 MDS Analytical Technologies (US) Inc. 
