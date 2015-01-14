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
        plt.show()
        print(' ')
        
    # Finds peaks using peak detect
        def peak_detect(self):
            print self.date

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
    


