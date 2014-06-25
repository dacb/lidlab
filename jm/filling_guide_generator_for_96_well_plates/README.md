This function rearranges column(s) of a data frame into the shape of 96-well blocks, and is intended to convert planning columns into a picture that you can use to load cultures, resin, etc. into 96-well plates.

This gDoc shows exactly what Janet designed it for, showing input and output
[140601 filling_guide_generator_for_96_well_plates DEMO](https://docs.google.com/spreadsheets/d/1l60FNFIF2afnxNQVqEfH7sgYI51THDVVXns14lnkdhU/edit#gid=1109366614)

it requires:
* a csv with some column that has labels spanning A1 to H12 (with the appropriate set of 96 included)
* a list of column names that you want reformatted into your guide

It gives a data.frame you can save to a csv, then do conditional highlighting on in gSpreadsheets or excel.

See the demo package for more detail.  

Required packages:
library(reshape2)  # for melt, cast
library(RCurl) # necessary for getURL below
library(stringr)  #used to get row and column names from well names: str_extract

Warnings:
* It will break if you give it a column name that doesn't exist
  * Error 140625: Error in structure(ordered, dim = ns) : 
  dims [product 96] do not match the length of object [0] 
