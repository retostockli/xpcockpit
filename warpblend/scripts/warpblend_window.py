from tkinter import *
from calc_projector_screen import *
from calc_warpgrid import *
import root_window
import params

# List to store windows/canvas/buttons
win = [] 
canvas = [] 
button = [] 

# ----- PROJECTION GRID -----

def draw_azimuthgrid(mon):

    global canvas

    alignment = True # warp for screen alignment

    FOVx, FOVy, h, w = calc_fov(params.nx[mon], params.ny[mon], params.w_h, params.gamma)

    canvas[mon].delete("shape")

    div = 5  # 5 divisions per box
    deltalabel = 10  # label every x degrees horizontal FOV (includes two boxes horizontal)
    deltax = deltalabel // div // 2 # angle spacing for thin violet grid lines
    ANGmin = params.lateral_offset[mon] - FOVx/2 # minimum angle in horizontal direction
    ANGmax = params.lateral_offset[mon] + FOVx/2 # maximum angle in horizontal direction

    deltay = params.ny[mon] // 36

    # plot grid and lines
    for ang in range(int(ANGmin/deltax)*deltax-deltax,int(ANGmax/deltax)*deltax+deltax,deltax):
        x0 = (float(ang)-ANGmin)/(ANGmax-ANGmin) * params.nx[mon]
        x1 = (float(ang+deltax)-ANGmin)/(ANGmax-ANGmin) * params.nx[mon]

        oddx = True if (ang // div) % 2 == 0 else False
        labelx = True if ang % deltalabel == 0 else False

        for y0 in range(0,params.ny[mon]+1,deltay):
            y1 = y0 + deltay
            oddy = True if (y0 // (div*deltay)) % 2 == 0 else False
            labely = True if (y0 // deltay) % (div*2) == 0 else False

            fillcolor = "lightgray" if (oddx and oddy) or (not oddx and not oddy) else "darkgray"

            px0, py0 = calc_warppoint(params.nx[mon], params.ny[mon], x0, y0, params.R, params.h_0, params.d_0, params.d_1, params.w_h, params.gamma, params.epsilon[mon], params.frustum,
                                            params.vertical_scale[mon], params.vertical_shift[mon], params.cylindrical[mon], params.projection[mon], params.ceiling, alignment)
            px1, py1 = calc_warppoint(params.nx[mon], params.ny[mon], x1, y0, params.R, params.h_0, params.d_0, params.d_1, params.w_h, params.gamma, params.epsilon[mon], params.frustum,
                                            params.vertical_scale[mon], params.vertical_shift[mon], params.cylindrical[mon], params.projection[mon], params.ceiling, alignment)
            px2, py2 = calc_warppoint(params.nx[mon], params.ny[mon], x1, y1, params.R, params.h_0, params.d_0, params.d_1, params.w_h, params.gamma, params.epsilon[mon], params.frustum,
                                            params.vertical_scale[mon], params.vertical_shift[mon], params.cylindrical[mon], params.projection[mon], params.ceiling, alignment)
            px3, py3 = calc_warppoint(params.nx[mon], params.ny[mon], x0, y1, params.R, params.h_0, params.d_0, params.d_1, params.w_h, params.gamma, params.epsilon[mon], params.frustum,
                                            params.vertical_scale[mon], params.vertical_shift[mon], params.cylindrical[mon], params.projection[mon], params.ceiling, alignment)
            canvas[mon].create_polygon(px0, py0, px1, py1, px2, py2, px3, py3, fill=fillcolor,tags="shape")

            fillcolor = "white" if labelx else "violet"
            canvas[mon].create_line(px0, py0, px3, py3, fill = fillcolor,width=2,tags="shape")

            fillcolor = "white" if labely else "violet"
            canvas[mon].create_line(px0, py0, px1, py1, fill = fillcolor,width=2,tags="shape")

    # plot azimuth labels
    for ang in range(int(ANGmin/deltalabel)*deltalabel,int(ANGmax/deltalabel)*deltalabel,deltalabel):
        x0 = (float(ang)-ANGmin)/(ANGmax-ANGmin) * params.nx[mon]
        y0 = params.ny[mon]//2
        px, py = calc_warppoint(params.nx[mon], params.ny[mon], x0, y0, params.R, params.h_0, params.d_0, params.d_1, params.w_h, params.gamma, params.epsilon[mon], params.frustum,
                    params.vertical_scale[mon], params.vertical_shift[mon], params.cylindrical[mon], params.projection[mon], params.ceiling, alignment)
        canvas[mon].create_text(px, py,text=str(ang),fill="red",font=("Helvetica", 18, "bold"),tags="shape")

def draw_blendlines(mon):

    global canvas

    deltay = 10
    fillcolor = "red"
    if params.blending[mon]:
        if params.blend_left_top[mon] != 0.0 or params.blend_left_bot[mon] != 0.0:
            for y0 in range(0,params.ny[mon]+1,deltay):
                y1 = y0+deltay
                x0 = (y0/(params.ny[mon]-1) * params.blend_left_bot[mon] + (1.0-y0/(params.ny[mon]-1)) * params.blend_left_top[mon])*(params.nx[mon]-1)
                x1 = (y1/(params.ny[mon]-1) * params.blend_left_bot[mon] + (1.0-y1/(params.ny[mon]-1)) * params.blend_left_top[mon])*(params.nx[mon]-1)
                canvas[mon].create_line(x0, y0, x1, y1, fill = fillcolor,width=2,tags="shape")
        if params.blend_right_top[mon] != 0.0 or params.blend_right_bot[mon] != 0.0:
            for y0 in range(0,params.ny[mon]+1,deltay):
                y1 = y0+deltay
                x0 = (params.nx[mon]-1) - (y0/(params.ny[mon]-1) * params.blend_right_bot[mon] + (1.0-y0/(params.ny[mon]-1)) * params.blend_right_top[mon])*(params.nx[mon]-1)
                x1 = (params.nx[mon]-1) - (y1/(params.ny[mon]-1) * params.blend_right_bot[mon] + (1.0-y1/(params.ny[mon]-1)) * params.blend_right_top[mon])*(params.nx[mon]-1)
                canvas[mon].create_line(x0, y0, x1, y1, fill = fillcolor,width=3,tags="shape")


def init_warpblend_window():

    global win
    global canvas
    global button

    win = [None] * params.nmon
    canvas = [None] * params.nmon
    button = [None] * params.nmon

def create_warpblend_window(mon):

    global win
    global canvas
    global button

    print("------------------------")
    print("Monitor: "+str(mon))

    if (params.cylindrical[mon] or params.projection[mon]):

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
            x0 = params.nx[0]//2
        else:
            x0 = sum(params.nx[0:mon])

        win[mon] = Toplevel(root_window.root)
        win[mon].geometry('{}x{}+{}+{}'.format(params.nx[mon], params.ny[mon], x0, 0))
        win[mon].title(f"Window {mon + 1}")
        win[mon].overrideredirect(True)  # no border

        canvas[mon] = Canvas(win[mon], width=params.nx[0], height=params.ny[0])
        canvas[mon].pack()

        # Create a Quit Button inside the canvas
        button[mon]=Button(win[mon],text="Quit", font=('Comic Sans', 13, 'bold'), command=root_window.quit_root_window)
        canvas[mon].create_window(params.nx[mon]/2, params.ny[mon]/10, window=button[mon])

    redraw_warpblend_window(mon)


def redraw_warpblend_window(mon):

    if (params.cylindrical[mon] or params.projection[mon]):

        # For Screen Alignment Not all calculations are needed
        draw_azimuthgrid(mon)
        draw_blendlines(mon)