# PlotGrowthCurves R_Shiny App with samples growth curve data

**Features**
*Adjustable Axes
*Choose which data to plot interactively
*Choice of error bars for averaged data
*Capture plots that you've made

**Inputs**
*For each data set, you need a file of the raw data and another file that has information about each tube or well
*Adjust file names and parameters to fit your data

**Packages Used**
*RShiny
*reshape
*XLConnect
*ggplot2

**Current Issues**
*Performance is slow because in order to make the file input interactive, it reloads everything for small adjustments
