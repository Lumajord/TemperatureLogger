# -*- coding: utf-8 -*-
"""
Created on Sat Jun 17 12:47:28 2017

@author: jordan
"""

import matplotlib
import matplotlib.dates as pltd
import matplotlib.pyplot as plt
#matplotlib.use('TkAgg')
plt.ioff()

from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk
# implement the default mpl key bindings
from matplotlib.backend_bases import key_press_handler


import numpy as np
import datetime as dt

import sys
if sys.version_info[0] < 3:
    import Tkinter as Tk
else:
    import tkinter as Tk


from data import *



def plot(time, temp, hum, fig, ax1, ax2):   
    ########################################################################
    ### PLOT
    ########################################################################


    line1, = ax1.plot(time, temp, 'b-', lw = 2)
    ax1.set_xlabel('Time', fontsize = 14)
    # Make the y-axis label, ticks and tick labels match the line color.
    ax1.set_ylabel('Temp in °C', color='b', fontsize = 16)
    ax1.tick_params('y', colors='b', labelsize = 14)
    
    line2, = ax2.plot(time, hum, 'r-', lw = 2)
    ax2.set_ylabel('Humidity in %', color='r', fontsize = 16)
    ax2.tick_params('y', colors='r', labelsize = 14)
    
    x_len = time.max() - time.min()
    y1_len = temp.max() - temp.min()
    y2_len = hum.max() - hum.min()
    ax1.set_xlim(time.min() - x_len/200, time.max() + x_len/200)
    ax1.set_ylim(temp.min() - y1_len/100, temp.max() + y1_len/100)
    ax2.set_ylim(hum.min() - y2_len/100, hum.max() + y2_len/100)
    
    
    formatter = pltd.DateFormatter('%d.%m  %H:%M')
    plt.gcf().axes[0].xaxis.set_major_formatter(formatter)
    
    fig.tight_layout()
    
    axs = [ax1, ax2]
    
    
    # rotate labels
    for label in axs[0].get_xmajorticklabels() + axs[1].get_xmajorticklabels():
        label.set_rotation(30)
        label.set_horizontalalignment("right")
        
    return line1, line2
    
    ########################################################################
    
    
def replot(time, temp, hum, line1, line2, ax1, ax2):
    line1.set_data(time,temp)
    line2.set_data(time,hum)
    x_len = time.max() - time.min()
    y1_len = temp.max() - temp.min()
    y2_len = hum.max() - hum.min()
    ax1.set_xlim(time.min() - x_len/200, time.max() + x_len/200)
    ax1.set_ylim(temp.min() - y1_len/100, temp.max() + y1_len/100)
    ax2.set_ylim(hum.min() - y2_len/100, hum.max() + y2_len/100)



root = Tk.Tk()
root.geometry("1200x800")


# create the main sections of the layout, 
# and lay them out
top = Tk.Frame(root)
top.pack(side=Tk.TOP, anchor = Tk.CENTER, fill = Tk.X)

## plot holder
fig, ax1 = plt.subplots()
ax2 = ax1.twinx()

## plot initial plot
logger = data_logger_data(15, 'data.dat')
temp, hum, time = logger.get_data()
line1, line2 = plot(time, temp, hum, fig, ax1, ax2)


canvas = FigureCanvasTkAgg(fig, master=root)
canvas.draw()
canvas.get_tk_widget().pack(side=Tk.TOP, fill=Tk.BOTH, expand=True)

toolbar = NavigationToolbar2Tk(canvas, top)
toolbar.update()
canvas._tkcanvas.pack(side=Tk.LEFT, fill=Tk.BOTH)
toolbar.pack(in_ = top, side = Tk.LEFT, fill = Tk.X, expand = True, anchor = Tk.W)

label_start = Tk.Label(root, text="Start date")
label_start.pack(in_=top, side=Tk.LEFT)

e_start_date = Tk.Entry(root)
e_start_date.pack(in_=top, side=Tk.LEFT)
e_start_date.insert(Tk.END, logger.date[0])



label_end = Tk.Label(root, text="End date")
label_end.pack(in_=top, side=Tk.LEFT)

e_end_date = Tk.Entry(root)
e_end_date.pack(in_=top, side=Tk.LEFT)
e_end_date.insert(Tk.END, logger.date[-1])




def on_key_event(event):
    print('you pressed %s' % event.key)
    key_press_handler(event, canvas, toolbar)

canvas.mpl_connect('key_press_event', on_key_event)


def _plot():
    global canvas
    
    start_date = dt.datetime.strptime(e_start_date.get(), "%Y-%m-%d %H:%M:%S")
    end_date = dt.datetime.strptime(e_end_date.get(), "%Y-%m-%d %H:%M:%S")

    logger.set_start_and_end(start_date, end_date)
    temp, hum, time = logger.get_data()
    replot(time, temp, hum, line1, line2, ax1, ax2)
    canvas.draw()



    

label_pad = Tk.Label(root, text="                                    ")
label_pad.pack(in_=top, side=Tk.RIGHT)

button = Tk.Button(master=root, text='Plot', command=_plot)
button.pack(in_=top, side=Tk.RIGHT)

Tk.mainloop( )
