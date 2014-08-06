This function rearranges a csv created from pasting endpoint data from SoftMaxPro (for the Molecular Devices plate reader) into a format suitable for R analysis

This function takes a data frame (easily read from a csv) and returns a data frame that is massaged into a nice dataframe format.  
* If summary numbers are present at the bottom of the dataframe (indicated by the presence of "Time(hh:mm:ss)"), it trims them off. 
* Empty rows are deleted
* Well labels are prepared
* The dataframe is melted to be 1 column with repeat measurements as rows. 
* Replicate measurements are indicated by the column rep in case there are multiple shake & read events.

The input should be a dataframe created from a csv that results from pasting a the results of SoftMaxPro reading a plate on endpoint mode with any number of shakings before reading it.
The pasted data can include or omit the summary numbers SoftMax pro caluclates and displays; they will be cut off if present. 

See the demo folder for a sample input and output file, as well as instructions for running it.

It is for:
* endpoint mode data, not kinetic data (a setting in plate >settings)
* data pasted into excel by using the mouse to select all the wells, and copy paste with c & v into escel or google spreadsheets, not exported from the file menu (which exports more data and is generally a clunky approach because it is hard to predict what your selections mean for the output format)
** Must be in the right export mode on the plate reader software. 
* SoftMaxPro, not the Tecan machine.  A similar script could be written for the tecan output but don't use this one.
