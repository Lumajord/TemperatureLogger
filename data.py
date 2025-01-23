# -*- coding: utf-8 -*-
"""
Created on Sat Jun 17 12:47:28 2017

@author: jordan
"""

import numpy as np
import datetime as dt
import matplotlib.dates as pltd



class data_logger_data:
    
    
    def __init__(self, smoothing_length, data_file):

        ####################################################################
        ### READ DATA
        ####################################################################
        delim = np.dtype([('date', str, 16), ('time', str, 16), ('temp', np.float64), ('hum', np.float64)])
        self.data = np.loadtxt(data_file, dtype=delim)
        ####################################################################
        
        ########################################################################
        ### MAKE DATA SMOOTHER
        ########################################################################
        smooth = smoothing_length ## smooth the temperature and humidity  bigger number = smoother
        
        window = np.hamming(smooth)
        self.data['hum'] = np.convolve(self.data['hum'], window/np.sum(window), 'same')
        self.data['temp'] = np.convolve(self.data['temp'], window/np.sum(window), 'same')
        
        cut = int(smooth/2) ## remove edge data to remove edge effects of convolve
        if cut > 0:
            self.data = self.data[cut:-cut]


        ###############################
        # init dates
        ###############################
        N = len(self.data['temp'])
        self.date = []
        for i in range(N):
            d = self.data['date'][i]
            t = self.data['time'][i]
            
            #d_t = d[2:-1]+ ' ' + t[2:-1]
            d_t = d + ' ' + t
            self.date = np.append(self.date, dt.datetime.strptime(d_t, '%d.%m.%Y %H:%M:%S'))
        
        self.dates = pltd.date2num(self.date)
        
        """
        end_date = self.date[-1]
        start_date = end_date - dt.timedelta(7)
        
        self.set_start_and_end(start_date, end_date)
        """
        self.start = 0
        self.end = len(self.date)
        ########################################################################
        

        
    def set_start_and_end(self, start_date, end_date):
        self.start_date = start_date
        self.end_date = end_date
        
        ########################################################################
        ### FIND START AND END DATE
        ########################################################################
        
        #find start and end
        self.start = 0
        self.end = len(self.date)
        for i in range(len(self.date)):
            if(self.date[i] >= self.start_date):
                self.start = i
                break
        
        for i in np.array(range(len(self.date)))[::-1]:
            if(self.date[i] <= self.end_date):
                self.end = i
                break        
        ###########################################################################
        
        
    def get_data(self):        
        ########################################################################
        ### return data logger data
        ########################################################################
        temp = self.data['temp'][self.start:self.end]
        hum = self.data['hum'][self.start:self.end]
        time = self.dates[self.start:self.end]
        
        return temp, hum, time
        ########################################################################
        
        

if __name__ == "__main__":
    import matplotlib.pyplot as plt
    logger = data_logger_data(1, 'data.dat')
    temp, hum, time = logger.get_data()
    
    ########################################################################
    ### PLOT
    ########################################################################
    ## plot holder
    fig, ax1 = plt.subplots()
    ax2 = ax1.twinx()

    ax1.plot_date(time, temp, 'b-', lw = 2)
    ax1.set_xlabel('Time', fontsize = 14)
    # Make the y-axis label, ticks and tick labels match the line color.
    ax1.set_ylabel('Temp in °C', color='b', fontsize = 16)
    ax1.tick_params('y', colors='b', labelsize = 14)
    
    ax2.plot_date(time, hum, 'r-', lw = 2)
    ax2.set_ylabel('Humidity in %', color='r', fontsize = 16)
    ax2.tick_params('y', colors='r', labelsize = 14)
    
    
    
    formatter = pltd.DateFormatter('%d-%m  %H:%M')
    plt.gcf().axes[0].xaxis.set_major_formatter(formatter)
    
    fig.tight_layout()
    
    axs = [ax1, ax2]
    
    
    # rotate labels
    for label in axs[0].get_xmajorticklabels() + axs[1].get_xmajorticklabels():
        label.set_rotation(30)
        label.set_horizontalalignment("right")

    
    ########################################################################
    
