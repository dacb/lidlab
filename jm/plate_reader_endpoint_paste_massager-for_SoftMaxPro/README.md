This function rearranges a csv created from pasting endpoint data from SoftMaxPro (for the Molecular Devices plate reader) into a format suitable for R analysis

This function takes a data frame and returns a data frame that is massaged into a nice dataframe format.  Well labels are assigned to each row, and replicate measurements are indicated in case there are multiple shake & read events.

The input should be a dataframe created from a csv that results from pasting a the results of SoftMaxPro reading a plate on endpoint mode with any number of shakings before reading it.

See the demo folder for a sample input and output file, as well as instructions for running it.

It is for:
* endpoint mode data, not kinetic data (a setting in plate >settings)
* data pasted into excel by using the mouse to select all the wells, and copy paste with c & v into escel or google spreadsheets, not exported from the file menu (which exports more data and is generally a clunky approach because it is hard to predict what your selections mean for the output format)
* SoftMaxPro, not the Tecan machine.  A similar script could be written for the tecan output but don't use this one.
