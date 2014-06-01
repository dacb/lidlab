This function changes the levels of a data.frame to be the order they appear in the dataframe.

If you sort a dataframe by some variable, then use 
dataframe$colname <- fix_order(dataframe, "colname")
it will reset the levels of dataframe$colname to be the order they appear after sorting.  

This is very handy for ggplot2, as the default order that it plots things is the order of the levels. 
