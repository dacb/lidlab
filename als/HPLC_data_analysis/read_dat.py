import numpy as np
#import matplotlib.pyplot as plt
import olefile
import os
#import pandas as pd


#%% Function definitions

#
def get_detector_data(stream_name, ole):

	dt= ole.openstream(stream_name)

	string=dt.read()

	data=np.fromstring(string,dtype=np.dtype('int32'))

	print np.fromstring(string[0:7*4],dtype=np.dtype('uint32'))
	return data[7:data[1]]

#
def get_stream(stream_name):

	dt= ole.openstream(stream_name)

	string=dt.read()

	data=np.fromstring(string,dtype=np.dtype('int32'))

	print data[0:100]
	return data

#
def get_raw_stream(stream_name):

	dt= ole.openstream(stream_name)

	string=dt.read()
	return string

#opens the .dat file and extracts the 210nM stream data
def process_datafile(filename):
    olefile.set_debug_mode(True)
    ole = olefile.OleFileIO(filename,raise_defects=olefile.DEFECT_INCORRECT)
    
    #ole.dumpdirectory()
    
    #string=get_raw_stream('XY Data/Auxiliary 20000 Trace')
    #print string
    
    data1=get_detector_data('Detector Data/Detector 0 Trace', ole)
    
    #data2=get_detector_data('Detector Data/Detector 1 Trace')
    config=ole.openstream('Configuration')
    data3 = config.read()
    config_ole = olefile.OleFileIO(data3)
    
    config_ole.dumpdirectory()
    
    #print ole.listdir()
    
    #data3d = ole.openstream('\x05SummaryInformation')
    #data3=data3d.read()
    
    
    #data3n=np.fromstring(data3,dtype=np.dtype('float16'))
    
    #for i,n in enumerate(data3n):
    #	print (i,n,data3[i])
    
    time = np.array(range(len(data1)),dtype=np.dtype('float'))/(80*4)
    
    #plt.plot(time,data1)
    #plt.show()
    
    #Makes a pandas data frame from the results
    #time_points = pd.DataFrame({Time[min]:time, Intensity[mV]:data1})
    #print time_points
    
    return time, data1
    
#%% For debugging using one file
#directory = 'C:/Users/Amanda/OneDrive/Assays/HPLC/'
#os.chdir(directory)

#filename = 'Data/2015_01_10/20150110 check std.dat'
#data = process_datafile(filename)
