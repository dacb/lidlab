# HPLC data processing
#Goal:Take superold file format from our ancient Shimadzu HPLC and do signal processing

#Notes
#Forrest Coleman wrote the read_dat.py file
#Peak detection from https://gist.github.com/1178136 is incorporated --> download that file separately

**Features**
* Read .dat files into numpy arrays for all .dat files in a folder

**Inputs**
* .dat files from the HPLC with the file name of the order /8digitdate_injectionvol_samplename.dat

**Packages Used**
- olefile
- numpy
- pandas
- os
- matplotlib.pyplot
- scipy
- pylab

**Current Issues**
* Performance is slow because in order to make the file input interactive, it reloads everything for small adjustments
