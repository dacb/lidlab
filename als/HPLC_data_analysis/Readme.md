# HPLC data processing
*Goal:Take superold file format from our ancient Shimadzu HPLC and do signal processing

#Notes
- Forrest Collman wrote the read_dat.py file
- Peak detection from https://gist.github.com/1178136 is incorporated --> download that file separately

**Features**
* Read .dat files into numpy arrays for all .dat files in a folder

**Inputs**
* .dat files from the HPLC with the file name of the order /8digitdate_injectionvol_samplename.dat

**Outputs**
- pdf files with all plots
- .csv files with time and intensity data for all samples

**Packages Used**
- olefile
- numpy
- pandas
- os
- matplotlib.pyplot
- scipy
- pylab
- ggplot


**Current Issues**
- Peak detection does not pick up all peaks
- ggplot does not give group plots
