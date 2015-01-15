# -*- coding: utf-8 -*-
"""
Created on Tue Jan 13 11:09:43 2015

@author: Amanda
"""
import os
import glob
import read_dat as rd
import numpy as np
import matplotlib.pyplot as plt
import peakdetect
import csv
import datetime
from matplotlib.backends.backend_pdf import PdfPages
import pandas as pd
from ggplot import *

#HPLC file processing 

#Alter path to the correct wd for you
os.chdir('C://Users/Amanda/Onedrive/Assays/HPLC')

#%% Sets up a class structure for the data
class sample_points:
    def __init__(self, name, filename, date, injection_vol, time_points, data_points):
        self.name=name
        self.filename = filename
        self.date = date
        self.injection_vol = injection_vol
        self.time_points = time_points
        self.data_points = data_points
        
    # A simple class function that plots
    def simple_plot(self):
        print('Sample ' + self.name)
        plt.plot(self.time_points,self.data_points)
        plt.title(self.name)
        pdf.savefig()
        plt.close()
        
    # Finds peaks using peak detect
    def peak_detect(self):
        peakdetect._datacheck_peakdetect(self.time_points, self.data_points)
        peaks = peakdetect.peakdetect(self.data_points, self.time_points)
        return peaks

#%% Gets a list of the .dat files in a specific folder
data_dir = './Data/2015_01_10'
file_list = glob.glob(data_dir + '/*.dat')
print file_list

#%% Runs a for loop to go through each data file and import it
sample_list =[]
for x in file_list:
    #print filename (just a check)
    #print x
    #parse the filename to get data out of it 
    filename=x[18:]
    info=filename.split('_')
    time,intensity = rd.process_datafile(x)
    sample_list.append(sample_points(info[2],x, info[0], info[1],time,intensity))

#Creates a files with all the peaks in each sample
for i in range(len(sample_list)):
    maxes, mins = sample_list[i].peak_detect()
    print mins
    print sample_list[i].name
    print maxes
    np.savetxt('peaks_' + sample_list[i].name + '.csv', maxes, delimiter=",")

#Creates a pdf file with simple graphs for each of the samples
with PdfPages('multipage_pdf.pdf') as pdf:
    for i in range(len(sample_list)):
        sample_list[i].simple_plot()

    # We can also set the file's metadata via the PdfPages object:
    # http://matplotlib.org/examples/pylab_examples/multipage_pdf.html
    d = pdf.infodict()
    d['Title'] = 'Multipage PDF Example'
    d['Author'] = u'Amanda Smith\xe4nen'
    d['Subject'] = 'Created a multiple page file with figures'
    d['Keywords'] = 'PdfPages multipage keywords author title subject'
    d['CreationDate'] = datetime.datetime(2015, 01, 14)
    d['ModDate'] = datetime.datetime.today()


#Makes a pandas dataframe out the files
for i in range(len(sample_list)):
    if i==0:
        df = pd.DataFrame({'Time[min]':sample_list[i].time_points, sample_list[i].name:sample_list[i].data_points})
    else:
        df2=pd.DataFrame({sample_list[i].name:sample_list[i].data_points})
        df=df.join(df2)
df.to_csv('try.csv', sep=',')
#Plot traces with ggplot
#melt data first
mdf=pd.melt(df,id_vars=['Time[min]'],var_name='Sample', value_name='Intensity[mV]')
#a = mdf[mdf.Sample =='1-5dil-org-acid-std.dat']
mdf2= mdf[mdf.Sample =='org-acid-std.dat']
print ggplot(mdf2, aes(x='Time[min]', y='Intensity[mV]', group ='Sample')) + geom_point() + geom_line()+facet_wrap("Sample")
print ggplot(mdf, aes(x='Time[min]', y='Intensity[mV]', group ='Sample')) + geom_point() + geom_line()+facet_wrap("Sample")
   
        