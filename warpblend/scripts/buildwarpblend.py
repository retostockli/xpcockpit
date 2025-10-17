from tkinter import *
#from __future__ import print_function
import math
import numpy as np
import matplotlib.pyplot as plot
from read_ini import *
from utility import *
from calc_vert_stretch import *
from calc_planar_to_cylindrical import *
from calc_projector_screen import *

inifile="singlemon.ini"

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

# loop through monitors
def calc_warpgrid(mon):

    print("------------------------")
    print("Monitor: "+str(mon))

    ar =  float(nx[mon]) / float(ny[mon])  # aspect ratio of projector image
    w = 2.0*w_h    # planar image width at screen distance (as if projection would be on planar scren)
    h = w / ar     # planar image height at screen distance (as if projection ... )

    print("Flat Screen Dimension at distance d0+d1: ")
    print(w,h)

    # calculate FOV of monitor
    FOVx = 2.0*gamma
                    
    # calculate larger FOVy for planar to cylindrical transformation
    # need this also for the no projection file for X-Plane
    f = w_h / math.tan(0.5*FOVx*d2r)
    FOVy = 2.0*math.atan(0.5*h/f)*r2d

    print("FOVx:    "+str(FOVx))
    print("FOVy:    "+str(FOVy))
					 
    ## reset projection grid
    xabs = np.zeros((ngx, ngy))
    yabs = np.zeros((ngx, ngy))
    xdif = np.zeros((ngx, ngy))
    ydif = np.zeros((ngx, ngy))
		
    # For Cylindrical Projection we have to expand image in the vertical
    # and increase vertical FOV:
    if cylindrical[mon]:
        vert_stretch = calc_vert_stretch(FOVx,FOVy)

    # loop through grid
    # grid vertical: gy goes from bottom to top
    # grid horizontal: gx from left to right
    for gy in range(0,ngy,1):
        for gx in range(0,ngx,1):
            
            # Calculate Pixel position of grid point (top-left is 0/0 and bottom-right is nx/ny)
            # ARE WE SURE WE RANGE FROM 0 .. nx? OR RATHER 0 .. nx-1?
            px = float(nx[mon]) * float(gx) / float(ngx-1)
            py = float(ny[mon]) * float(gy) / float(ngy-1)
            xabs[gx,gy] = px
            yabs[gx,gy] = py
            
            # 1. Transformation from planar to cylindrical rendering
            # --> This has nothing to do with the projector orientation and mount etc.
            # --> This is needed since X-Plane renders on a plane but we need a cylindrical rendering
            # Please see planar_to_cylindrical.pdf
            if cylindrical[mon]:
                ex, ey = calc_planar_to_cylindrical(px, py, nx[mon], ny[mon], FOVx, FOVy, frustum, vert_stretch)
               
                xdif[gx,gy] += ex - px
                ydif[gx,gy] += ey - py

                # update grid coordinates for keystone and projection calculation
                px += xdif[gx,gy]
                py += ydif[gx,gy]

            # 2. Add vertical shift and scale if needed
            # This has to go after planar to cylindrical projection
            # since that projection works in original input coordinates
            # where horizon is centered in image
            if (vertical_scale[mon] != 0.0) or (vertical_shift[mon] != 0.0):
                    ey = py * vertical_scale[mon] + vertical_shift[mon]
                    ydif[gx,gy] += ey - py
                    py += ydif[gx,gy]

                                                    
            # 3. Warping the planar projection of a regular table or ceiling mounted projector
            # onto a cylindrical screen. This is needed since the projector image is only ok
            # on a flat screen. We need to squeeze it onto a cylinder.
            # Please see projection_geometry.pdf
            if projection[mon]:
                ex, ey = calc_projector_screen(px, py, nx[mon], ny[mon], R, h, h_0, d_0, d_1, w, w_h, gamma, epsilon[mon])

                # print(ex,ey)
                
                xdif[gx,gy] += ex - px
                ydif[gx,gy] += ey - py

#                if (gx == 0) & (gy == 0):
#                    print(ex)
#                    print(ey)


 		
    # inverse x and y array for ceiling mount
    if ceiling:
        ydif = -np.flip(ydif,axis=1)
        xdif = np.flip(xdif,axis=1)
                
    return xabs, yabs, xdif, ydif

# ----- PROJECTION GRID -----

def draw_projectiongrid():
   for gy in range(0,ngy,1):
        for gx in range(0,ngx,1):

            x = xabs[gx,gy] + xdif[gx,gy]
            y = yabs[gx,gy] + ydif[gx,gy]

            r = 3 # radius of the point
            canvas.create_oval(x - r, y - r, x + r, y + r, fill="red", outline="blue")            



# ----- MAIN -----

xabs, yabs, xdif, ydif = calc_warpgrid(0)

print(xdif[0,0])
print(ydif[0,0])
print(xdif[0,ngy-1])
print(ydif[0,ngy-1])

root = Tk()
root.geometry('{}x{}+{}+{}'.format(nx[0], ny[0], 1920, 0))

# No border
root.overrideredirect(True)

#Full Screen Window
#root.attributes('-fullscreen', True)

def quit_root():
   root.destroy()

canvas = Canvas(root, width=nx[0], height=ny[0])
canvas.pack()

# Create a Quit Button
button=Button(root,text="Quit", font=('Comic Sans', 13, 'bold'), command= quit_root)

draw_projectiongrid()

r = 8
x = 0
y = 0
canvas.create_oval(x - r, y - r, x + r, y + r, fill="green", outline="white")  
x = 0
y = ny[0]-1
canvas.create_oval(x - r, y - r, x + r, y + r, fill="green", outline="white")  
r = 8
x = nx[0]-1
y = 0
canvas.create_oval(x - r, y - r, x + r, y + r, fill="green", outline="white")  
x = nx[0]-1
y = ny[0]-1
canvas.create_oval(x - r, y - r, x + r, y + r, fill="green", outline="white")  
# Place (embed) the button inside the canvas at (200, 150)
canvas.create_window(nx[0]/2, ny[0]/10, window=button)

root.mainloop()
