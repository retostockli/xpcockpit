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

#inifile="singlemon.ini"
inifile="test.ini"

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

def draw_azimuthgrid(mon):
    FOVx, FOVy, h, w = calc_fov(nx[mon], ny[mon], w_h, gamma)

    canvas[mon].delete("shape")

    div = 5  # 5 divisions per box
    deltalabel = 10  # label every x degrees horizontal FOV (includes two boxes horizontal)
    deltax = deltalabel // div // 2 # angle spacing for thin violet grid lines
    ANGmin = lateral_offset[mon] - FOVx/2 # minimum angle in horizontal direction
    ANGmax = lateral_offset[mon] + FOVx/2 # maximum angle in horizontal direction

    deltay = ny[mon] // 36

    # plot grid and lines
    for ang in range(int(ANGmin/deltax)*deltax-deltax,int(ANGmax/deltax)*deltax+deltax,deltax):
        x0 = (float(ang)-ANGmin)/(ANGmax-ANGmin) * nx[mon]
        x1 = (float(ang+deltax)-ANGmin)/(ANGmax-ANGmin) * nx[mon]

        oddx = True if (ang // div) % 2 == 0 else False
        labelx = True if ang % deltalabel == 0 else False

        alignment = True # warp for screen alignment

        for y0 in range(0,ny[mon]+1,deltay):
            y1 = y0 + deltay
            oddy = True if (y0 // (div*deltay)) % 2 == 0 else False
            labely = True if (y0 // deltay) % (div*2) == 0 else False

            fillcolor = "lightgray" if (oddx and oddy) or (not oddx and not oddy) else "darkgray"

            px0, py0 = calc_warppoint(nx[mon], ny[mon], x0, y0, R, h_0, d_0, d_1, w_h, gamma, epsilon[mon], frustum,
                                            vertical_scale[mon], vertical_shift[mon], cylindrical[mon], projection[mon], ceiling, alignment)
            px1, py1 = calc_warppoint(nx[mon], ny[mon], x1, y0, R, h_0, d_0, d_1, w_h, gamma, epsilon[mon], frustum,
                                            vertical_scale[mon], vertical_shift[mon], cylindrical[mon], projection[mon], ceiling, alignment)
            px2, py2 = calc_warppoint(nx[mon], ny[mon], x1, y1, R, h_0, d_0, d_1, w_h, gamma, epsilon[mon], frustum,
                                            vertical_scale[mon], vertical_shift[mon], cylindrical[mon], projection[mon], ceiling, alignment)
            px3, py3 = calc_warppoint(nx[mon], ny[mon], x0, y1, R, h_0, d_0, d_1, w_h, gamma, epsilon[mon], frustum,
                                            vertical_scale[mon], vertical_shift[mon], cylindrical[mon], projection[mon], ceiling, alignment)
            canvas[mon].create_polygon(px0, py0, px1, py1, px2, py2, px3, py3, fill=fillcolor,tags="shape")

            fillcolor = "white" if labelx else "violet"
            canvas[mon].create_line(px0, py0, px3, py3, fill = fillcolor,width=2,tags="shape")

            fillcolor = "white" if labely else "violet"
            canvas[mon].create_line(px0, py0, px1, py1, fill = fillcolor,width=2,tags="shape")

    # plot azimuth labels
    for ang in range(int(ANGmin/deltalabel)*deltalabel,int(ANGmax/deltalabel)*deltalabel,deltalabel):
        x0 = (float(ang)-ANGmin)/(ANGmax-ANGmin) * nx[mon]
        y0 = ny[mon]//2
        px, py = calc_warppoint(nx[mon], ny[mon], x0, y0, R, h_0, d_0, d_1, w_h, gamma, epsilon[mon], frustum,
                    vertical_scale[mon], vertical_shift[mon], cylindrical[mon], projection[mon], ceiling, alignment)
        canvas[mon].create_text(px, py,text=str(ang),fill="red",font=("Helvetica", 18, "bold"),tags="shape")

def draw_blendlines(mon):
    deltay = 10
    fillcolor = "red"
    if blending[mon]:
        if blend_left_top[mon] != 0.0 or blend_left_bot[mon] != 0.0:
            for y0 in range(0,ny[mon]+1,deltay):
                y1 = y0+deltay
                x0 = (y0/(ny[mon]-1) * blend_left_bot[mon] + (1.0-y0/(ny[mon]-1)) * blend_left_top[mon])*(nx[mon]-1)
                x1 = (y1/(ny[mon]-1) * blend_left_bot[mon] + (1.0-y1/(ny[mon]-1)) * blend_left_top[mon])*(nx[mon]-1)
                canvas[mon].create_line(x0, y0, x1, y1, fill = fillcolor,width=2,tags="shape")
        if blend_right_top[mon] != 0.0 or blend_right_bot[mon] != 0.0:
            for y0 in range(0,ny[mon]+1,deltay):
                y1 = y0+deltay
                x0 = (nx[mon]-1) - (y0/(ny[mon]-1) * blend_right_bot[mon] + (1.0-y0/(ny[mon]-1)) * blend_right_top[mon])*(nx[mon]-1)
                x1 = (nx[mon]-1) - (y1/(ny[mon]-1) * blend_right_bot[mon] + (1.0-y1/(ny[mon]-1)) * blend_right_top[mon])*(nx[mon]-1)
                canvas[mon].create_line(x0, y0, x1, y1, fill = fillcolor,width=1,tags="shape")

# ----- MAIN -----

root = Tk()
root.geometry('{}x{}'.format(ROOT_NX, ROOT_NY))
root.title("Main Window")


def quit_root():
   root.destroy()

# Create a Quit Button
root_button=Button(root,text="Quit", font=('Comic Sans', 13, 'bold'), command= quit_root)

# Place the button in the window
root_button.grid(row=0, column=2, padx=0, pady=0, sticky="n")

# Create a label
label = Label(root, text="R [cm] ")
label.grid(row=1, column=0, padx=10, pady=10, sticky="e")

# Create an entry field
entry = Entry(root, width=20)
entry.grid(row=1, column=1, padx=10, pady=10)

entry.insert(0,R)

# Button to read input
def recalc_grid():
    global R 
    R = float(entry.get())
    print(R)
    for mon in range(0,nmon,1):
        if (cylindrical[mon] or projection[mon]):

            # For Screen Alignment Not all calculations are needed
            draw_azimuthgrid(mon)
            draw_blendlines(mon)

recalc_button = Button(root, text="Recalc", command=recalc_grid)
recalc_button.grid(row=5, column=2, columnspan=2, pady=10)

# List to store windows/canvas/buttons
win = [None] * nmon
canvas = [None] * nmon
button = [None] * nmon

for mon in range(0,nmon,1):
    print("------------------------")
    print("Monitor: "+str(mon))

    if (cylindrical[mon] or projection[mon]):

        # # For Screen Alignment Not all calculations are needed
        # alignment = True

        # xabs, yabs, xdif, ydif = calc_warpgrid(nx[mon], ny[mon], ngx, ngy, R, h_0, d_0, d_1, w_h, gamma, epsilon[mon], frustum,
        #                                     vertical_scale[mon], vertical_shift[mon], cylindrical[mon], projection[mon], ceiling, alignment)

        # print(xdif[0,0])
        # print(ydif[0,0])
        # print(xdif[0,ngy-1])
        # print(ydif[0,ngy-1])

        # px, py = calc_warppoint(nx[mon], ny[mon], 0, 0, R, h_0, d_0, d_1, w_h, gamma, epsilon[mon], frustum,
        #                                     vertical_scale[mon], vertical_shift[mon], cylindrical[mon], projection[mon], ceiling, alignment)
        
        # print(px)
        # print(py)

        if (mon == 0):
            x0 = nx[0]//2
        else:
            x0 = sum(nx[0:mon])
   
        win[mon] = Toplevel(root)
        win[mon].geometry('{}x{}+{}+{}'.format(nx[mon], ny[mon], x0, 0))
        win[mon].title(f"Window {mon + 1}")
        win[mon].overrideredirect(True)  # no border

        canvas[mon] = Canvas(win[mon], width=nx[0], height=ny[0])
        canvas[mon].pack()

        # Create a Quit Button inside the canvas
        button[mon]=Button(win[mon],text="Quit", font=('Comic Sans', 13, 'bold'), command= quit_root)
        canvas[mon].create_window(nx[mon]/2, ny[mon]/10, window=button[mon])

        draw_azimuthgrid(mon)
        draw_blendlines(mon)

root.mainloop()
