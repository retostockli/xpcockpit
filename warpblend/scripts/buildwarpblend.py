from tkinter import *
#from __future__ import print_function
import math
import numpy as np
import matplotlib.pyplot as plot
from read_ini import *
from utility import *
from calc_warpgrid import *

ROOT_NX = 500
ROOT_NY = 500

inifile="singlemon.ini"
#inifile="test.ini"

# vertical_offset is currently not used
xp12, outfile, nmon, ngx, ngy, dragx, dragy, R, d_0, h_0, tr, nx, ny, \
ceiling, cylindrical, projection, epsilon, frustum, \
lateral_offset, vertical_offset, vertical_shift, vertical_scale, \
blending, blend_left_top, blend_left_bot, blend_right_top, blend_right_bot, \
gridtest, blendtest, forwin, savegrid = read_ini(inifile)

stepx = int((ngx-1)/(dragx-1)) # make sure this is integer and stepx * (dragx-1) = ngx-1!
stepy = int((ngy-1)/(dragy-1)) # make sure this is integer and stepy * (dragy-1) = ngy-1!

# blend alpha values (4 steps at distance 0.0, 0.33, 0.67 and 1.0)
blend_alpha = [0.0,0.45,0.8,1.0]
blend_exp = 1.0 # exponent of the blend function (distance^exponent)

# General Calculations independent of pixel position (cm or degrees)
beta = math.atan(1.0/(2.0*tr))*r2d # Maximum horizontal FOV from Projector [deg]
beta1 = 180.-beta   # well ... check out the drawing yourself
delta = math.asin(d_0/R*math.sin(beta1*d2r))*r2d  # same here
gamma = 180.-beta1-delta  # Maximum horizontal FOV from Screen Center [deg]
R_1 = R*math.sin(gamma*d2r)/math.sin(beta1*d2r) # Maximum Distance of Projector to screen edge
d_1 = R_1*math.cos(beta*d2r) # distance of projector to hypothetical planar screen in front of cylindrical screen
w_h = R_1*math.sin(beta*d2r) # half of hypothetical planar image width at screen distance


# ----- PROJECTION GRID -----

def draw_projectiongrid(mon):
   for gy in range(0,ngy-1,1):
        for gx in range(0,ngx-1,1):

            # create subdivisions every 10th control point
            sgx = gx // 5
            sgy = gy // 5

            if sgx % 2 == 0:
                if sgy % 2 == 0:
                    fillcolor = "lightgray"
                else:
                    fillcolor = "darkgray"
            else:
                if sgy % 2 == 0:
                    fillcolor = "darkgray"
                else:
                    fillcolor = "lightgray"

            x0 = xabs[gx,gy] + xdif[gx,gy]
            y0 = yabs[gx,gy] + ydif[gx,gy]
            x1 = xabs[gx+1,gy] + xdif[gx+1,gy]
            y1 = yabs[gx+1,gy] + ydif[gx+1,gy]
            x2 = xabs[gx+1,gy+1] + xdif[gx+1,gy+1]
            y2 = yabs[gx+1,gy+1] + ydif[gx+1,gy+1]
            x3 = xabs[gx,gy+1] + xdif[gx,gy+1]
            y3 = yabs[gx,gy+1] + ydif[gx,gy+1]
            
            canvas[mon].create_polygon(x0, y0, x1, y1, x2, y2, x3, y3, fill=fillcolor)

            if gy % 15 == 0:
                canvas[mon].create_line(x0, y0, x1, y1, fill = "blue")

            if gx % 15 == 0:
                canvas[mon].create_line(x0, y0, x3, y3, fill = "blue")

def draw_azimuthlines(mon):
    FOVx, FOVy, h, w = calc_fov(nx[mon], ny[mon], w_h, gamma)

    delta = 10
    ANGmin = lateral_offset[mon] - FOVx/2
    ANGmax = lateral_offset[mon] + FOVx/2

    for ang in range(int(ANGmin/delta)*delta,int(ANGmax/delta)*delta,delta):
        x = (float(ang)-ANGmin)/(ANGmax-ANGmin) * nx[mon]

        for y in range(0,ny[mon]+1,10):
            if y > 0:
                px0 = px
                py0 = py
            px, py = calc_warppoint(nx[mon], ny[mon], x, y, R, h_0, d_0, d_1, w_h, gamma, epsilon[mon], frustum,
                                            vertical_scale[mon], vertical_shift[mon], cylindrical[mon], projection[mon], ceiling, alignment)
            
            if y > 0:
                canvas[mon].create_line(px0, py0, px, py, fill = "red",width=2)

        y = ny[mon]/2
        px, py = calc_warppoint(nx[mon], ny[mon], x+30, y, R, h_0, d_0, d_1, w_h, gamma, epsilon[mon], frustum,
                                            vertical_scale[mon], vertical_shift[mon], cylindrical[mon], projection[mon], ceiling, alignment)
        canvas[mon].create_text(px, py,text=str(ang),fill="red",font=("Helvetica", 16, "bold"))

            

# ----- MAIN -----

root = Tk()
root.geometry('{}x{}'.format(ROOT_NX, ROOT_NY))
root.title("Main Window")


def quit_root():
   root.destroy()

# Create a Quit Button
root_button=Button(root,text="Quit", font=('Comic Sans', 13, 'bold'), command= quit_root)

# Place the button in the window
root_button.pack(padx=20, pady=20)

# List to store windows/canvas/buttons
win = [None] * nmon
canvas = [None] * nmon
button = [None] * nmon

for mon in range(0,nmon,1):
    print("------------------------")
    print("Monitor: "+str(mon))

    if (cylindrical[mon] or projection[mon]):

        # For Screen Alignment Not all calculations are needed
        alignment = True

        xabs, yabs, xdif, ydif = calc_warpgrid(nx[mon], ny[mon], ngx, ngy, R, h_0, d_0, d_1, w_h, gamma, epsilon[mon], frustum,
                                            vertical_scale[mon], vertical_shift[mon], cylindrical[mon], projection[mon], ceiling, alignment)

        print(xdif[0,0])
        print(ydif[0,0])
        print(xdif[0,ngy-1])
        print(ydif[0,ngy-1])

        px, py = calc_warppoint(nx[mon], ny[mon], 0, 0, R, h_0, d_0, d_1, w_h, gamma, epsilon[mon], frustum,
                                            vertical_scale[mon], vertical_shift[mon], cylindrical[mon], projection[mon], ceiling, alignment)
        
        print(px)
        print(py)

        if (mon == 0):
            x0 = 0
        else:
            x0 = sum(nx[0:mon-1])
        win[mon] = Toplevel(root)
        win[mon].geometry('{}x{}+{}+{}'.format(nx[mon], ny[mon], x0, 0))
        win[mon].title(f"Window {mon + 1}")
        win[mon].overrideredirect(True)  # no border

        canvas[mon] = Canvas(win[mon], width=nx[0], height=ny[0])
        canvas[mon].pack()

        draw_projectiongrid(mon)
        draw_azimuthlines(mon)

        # Place (embed) the button inside the canvas at (200, 150)
        # Create a Quit Button
        button[mon]=Button(win[mon],text="Quit", font=('Comic Sans', 13, 'bold'), command= quit_root)
        canvas[mon].create_window(nx[mon]/2, ny[mon]/10, window=button[mon])

root.mainloop()
