fun_import_raw_spectra <- function(filename, path) {
        # Converts data from SpectraMax190 to columnar format.
        # Handles end point, kinetic, and spectrum data
        # See: https://docs.google.com/spreadsheets/d/1XMQXqH1nv7I3iTECpaY-j_JbLKNK9JIwRpfaAqC2y5s/edit?usp=sharing for sample data
        #
        # Requires:
        #   melt (function in reshape2 package)
        #       install by: install.packages("reshape2")
        #       load for each R session: library(reshape2)
        #
        # Args:
        #   filename: the name of a file you want to import
        #   path: the path to the folder containing the file specified by filename.  e.g. "./raw_data"  (note not "./raw_data/")
        #
        # Returns:
        #   One well scan per row.  One row per time or wavelength as it varies.  Each row contains the name typed in describing each scan. 
        filename_import <- paste0(path, "/", filename)
        df.raw <- read.csv(file=filename_import, skip = 0, fill=T, stringsAsFactors = F) #imports the first row as column names
        # make a data.frame that tells you the row numbers for the start & stop of each chunk
        End_rows_raw <- grep("~End",df.raw[,1] ) # find all rows that have "~End" as a string in the first column.
        end_rows <- End_rows_raw -2  # discard the row that is labaled "~End" and the row above that is blank
        start_rows <- c(1, End_rows_raw[-length(End_rows_raw)]+1) # The first row is a start row.  There are also start rows the row below each row with "~End".
                # End_rows_raw[-length(End_rows_raw)] removes the last row from the list; there isn't a start row below the last "~End"
                        # e.g. If there are 5 entries in End_rows_raw, End_rows_raw(-length(End_rows_raw)) returns the first 4 entries.
        i_info <- data.frame("start_row"=start_rows, "end_row"=end_rows)
        print(i_info) # lists the row numbers that the chunks start and stop.  Note: the "##BLOCKS= XX" row in the raw CSV does not count.
        print(paste("nrow of i info:", nrow(i_info)))
        # break apart the data by chunk
                #   num.of.reads <- str_extract(colnames(df.raw)[1], pattern = "[0-9]$") # "##BLOCKS= 5" is imported as "X..BLOCKS..5"; this row returns 5.
        print(paste("1:nrow(i_info): ", 1:nrow(i_info)))
        for (i in 1:nrow(i_info)){
                print(paste("i =", i)) # print which set you are on
                print(i_info[i,])  # prints start_row, end_row
                i_info_row <- i_info[i,] # the row that has information about the scan.  Wavelength(16) Endpoint/Kinetic (5)
                d.subset <- df.raw[i_info_row$start_row:i_info_row$end_row,]
                scan.name <- d.subset[1,2]
                print(scan.name)
                data.type <- d.subset[1, 5]
                print(paste("data type:", data.type))
                # BREAK APART DEPENDING ON DATA TYPE
                if (data.type == "Endpoint") {
                        # This data type has one reading at one wavelength, one temperature, and one time.
                        plate.temp <- as.numeric(d.subset[3,2])
                        wavelength <- as.numeric(d.subset[1,16])
                        # extract the data to melt:
                        d.temp.col <- data.frame("temperature" = as.numeric(d.subset[3:nrow(d.subset), 2])) # in 2nd column
                        d.subset <- d.subset[2:nrow(d.subset), 3:ncol(d.subset)] # trim off the non-well/value data
                        colnames(d.subset) <- d.subset[1,]  # need to define column names
                        d.subset <- d.subset[-1,]  # delete the row that represents the column names
                        d.subset <- cbind(d.temp.col, d.subset)
                        print(head(d.subset, 3))
                        # add on columns for the values identified
                        d.subset$scan.name <- scan.name
                        d.subset$data.type <- data.type
                        d.subset$time <- NA  # NECESSARY to merge w/ spectrum or kinetic data
                        melt_columns <- colnames(d.subset)[2:97]  # will pass this list of columns to the melt function
                        print("melt columns:")
                        print(melt_columns)
                        # MELT the data
                        d.subset <- melt(d.subset, measure.vars = melt_columns, variable.name="scan.well", # scan.well = A1, B1, ...
                                         value.name = "absorbance")
                        d.subset$wavelength <- wavelength
                        d.subset$absorbance = as.numeric(d.subset$absorbance)
                        d.subset <- d.subset[is.na(d.subset$absorbance)==FALSE, ]
                        print(head(d.subset))
                        if(i==1) d = d.subset   # This works; not sure if you can rbind to an empty data frame, so I did this.
                        if(i>1) d = rbind(d, d.subset)
                }
                if (data.type == "Kinetic") {
                        # This data type has one reading at one wavelength, one temperature, and one time.
                        # Unfortunately if you have it scan every 20 minutes, the scans will look like this:  20:00:00, 40:00:00, 1:00:00
                            # this change of format for times > 1 hr is troublesome for parsing because the format changes.
                        wavelength <- as.numeric(d.subset[1,16])
                        # extract the data to melt:
                        d.temp.col <- data.frame("temperature" = as.numeric(d.subset[3:nrow(d.subset), 2])) # in 2nd column
                        d.time.col <- data.frame("time" = d.subset[3:nrow(d.subset), 1]) # in the 1st column
                        print("d.time.col:")
                        print(d.time.col)
                        print("class of d.time.col:")
                        print(class(d.time.col))
                        print("type of d.time.col:")
                        print(typeof(d.time.col))
                        d.time.col.parsed <- sapply(strsplit(as.character(d.time.col$time), ":"),  function(x) { x <- as.numeric(x)
                                                                                                  x[1]+x[2]/60   })
                        print("d.time.col.parsed:")
                        print(d.time.col.parsed)
                        print("d.time.col.parsed[[2]]")
                        print(d.time.col.parsed[[2]])
                        print("d.time.col.parsed[[1]]")
                        print(d.time.col.parsed[[1]])
                        d.time.step <- d.time.col.parsed[[2]] - d.time.col.parsed[[1]] # The time step is always the same on this machine
                        print("d time step (minutes):")
                        print(d.time.step)
                        # make a column that counts up that number of minutes each row: like c(0:4)*c(10, 10, 10, 10, 10)
                        d.time.col.minutes <- c(1:length(d.time.col$time)-1)*rep(d.time.step, length(d.time.col$time))
                        print("d.time.col.minutes:")
                        print(d.time.col.minutes)
                        d.subset <- d.subset[2:nrow(d.subset), 3:ncol(d.subset)] # trim off the non-well/value data
                        colnames(d.subset) <- d.subset[1,]  # need to define column names
                        d.subset <- d.subset[-1,]  # delete the row that represents the column names
                        d.subset <- cbind(d.subset, d.temp.col) # bind descriptive columns after the well read values
                        d.subset <- cbind(d.subset, d.time.col)
                        # how to bind on a data frame with a new name:
                                # cbind (data.frame(col1 = c(1,2,3)), d = data.frame(no = c(4,5,6)))
                        d.subset <- cbind(d.subset, minutes = d.time.col.minutes) # bind descriptive columns after the well read values
                        print(head(d.subset, 3))
                        # add on columns for the values identified
                        melt_columns <- colnames(d.subset)[1:96]  # will pass this list of columns to the melt function
                        # MELT the data
                        d.subset <- melt(d.subset, measure.vars = c(melt_columns), variable.name="scan.well", # scan.well = A1, B1, ...
                                         value.name = "absorbance")
                        d.subset$wavelength <- wavelength
                        d.subset$absorbance = as.numeric(d.subset$absorbance)
                        d.subset <- d.subset[is.na(d.subset$absorbance)==FALSE, ]
                        d.subset$scan.name <- scan.name
                        d.subset$data.type <- data.type
                        print(head(d.subset))
                        if(i==1) d = d.subset   # This works; not sure if you can rbind to an empty data frame, so I did this.
                        if(i>1) d = merge(d, d.subset, all=TRUE)
                }
                if (data.type == "Spectrum") {
                        # This data type has one reading per wavelength.  There is an associated temperature value but no time value
                        # extract the data to melt:
                        d.temp.col <- data.frame("temperature" = as.numeric(d.subset[3:nrow(d.subset), 2])) # in 2nd column
                        d.wavelength.col <- data.frame("wavelength" = as.numeric(d.subset[3:nrow(d.subset), 1])) # in the 1st column
                        d.subset <- d.subset[2:nrow(d.subset), 3:ncol(d.subset)] # trim off the non-well/value data
                        colnames(d.subset) <- d.subset[1,]  # need to define column names
                        d.subset <- d.subset[-1,]  # delete the row that represents the column names
                        d.subset <- cbind(d.subset, d.temp.col) # bind descriptive columns after the well read values
                        d.subset <- cbind(d.subset, d.wavelength.col) # bind descriptive columns after the well read values
                        print(head(d.subset, 3))
                        # add on columns for the values identified
                        melt_columns <- colnames(d.subset)[1:96]  # will pass this list of columns to the melt function
                        # MELT the data
                        d.subset <- melt(d.subset, measure.vars = c(melt_columns), variable.name="scan.well", # scan.well = A1, B1, ...
                                         value.name = "absorbance")
                        d.subset$absorbance = as.numeric(d.subset$absorbance)
                        d.subset <- d.subset[is.na(d.subset$absorbance)==FALSE, ]
                        d.subset$scan.name <- scan.name
                        d.subset$data.type <- data.type
                        print(head(d.subset))
                        if(i==1) d = d.subset   # This works; not sure if you can rbind to an empty data frame, so I did this.
                        if(i>1) d = merge(d, d.subset, all=TRUE)
                }
        }
        return(d)
}
