This function massages data exported from the computer program ???? that talks to the Molecular Devices (brand) SpectraMax 190 (model) plate reader machine.  It can import raw files including end-point, kinetic, and spectrum data types.

It exports all of these scans to one data frame with one well scan per row.  Each wavelength or time a well is measured at gets its own row. The name entered for the scan on the plate reader is returned as a new column. 

Requirements:
* Exported CSV from the ???? software: see example of CSV format [here](https://docs.google.com/spreadsheets/d/1ILyE9JzosQp_oiixHFNr2D6E2HL2Io8N0c4Hq1uk4_Y/edit#gid=0)
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
