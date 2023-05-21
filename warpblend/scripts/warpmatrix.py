# GTX970 Output Numbering
# DVI0 (DVI-I-1): 0
# HDMI/HP (DP-1): 1
# DVI1 (DVI-D-0): 2
# HDMI (HDMI-0): 3

# X-Plane 11: need to activate VULKAN in Graphics Settings
# However: With Vulkan, Cylindrical projection has limited rendering in corners
# To have scenery only view: default_view 1 in X-Plane.prf
# To have cylindrical projection: proj_cyl 1 in X-Plane.prf

# Import Libraries
import math
import numpy as np
import matplotlib.pyplot as plot

# Settings
setting = 1

# Graphics
doplot = False

# Parameters
d2r = math.pi/180.
r2d = 180./math.pi

# Dimensions (in cm)
R = 169.0   # Screen Radius
d_0 = 30.0   # Distance of Projector focal point from center of cylinder (positive is towards screen)
# Projector focal point may be behind projector lens. For me it is around 2-4 cm behind
# as it seems. No documentation found.
tr = 0.49   # Projector Throw ratio
h_0 = 15.0   # lower height of image above center of lens when projected on planar screen from untilted projector

if setting == 1:
    # Cylindrical + Projection + Blending
    nmon = 4  # number of monitors
    cylindrical = [False,True,True,True]  # apply flat plane to cylinder warping
    projection = [False,True,True,True]  # apply projection onto curved surface
    epsilon = [0.0,0.0,5.75,0.0]         # projector tilt [deg]
    lateral_offset = [0.0,-62.0,0.0,62.5]  # lateral offset [deg]
    vertical_offset = [0.0,0.0,0.0,0.0]    # vertical offset [deg]
    blending = [False,True,True,True]   # apply blending at sides
    blend_left_top = [0.0,0.0,287.0,234.0]
    blend_left_bot = [0.0,0.0,341.0,318.0]
    blend_right_top = [0.0,249.0,257.0,0.0]
    blend_right_bot = [0.0,333.0,318.0,0.0]
    gridtest = False # display grid test pattern
    forwin = False  # create for windows or for linux
elif setting == 2:
    # Projection + Blending
    nmon = 4  # number of monitors
    cylindrical = [False,False,False,False]  # apply flat plane to cylinder warping
    projection = [False,True,True,True]  # apply projection onto curved surface
    epsilon = [0.0,0.0,5.75,0.0]         # projector tilt [deg]
    lateral_offset = [0.0,-62.0,0.0,62.5]  # lateral offset [deg]
    vertical_offset = [0.0,0.0,0.0,0.0]    # vertical offset [deg]
    blending = [False,True,True,True]   # apply blending at sides
    blend_left_top = [0.0,0.0,287.0,234.0]
    blend_left_bot = [0.0,0.0,341.0,318.0]
    blend_right_top = [0.0,249.0,257.0,0.0]
    blend_right_bot = [0.0,333.0,318.0,0.0]
    gridtest = False # display grid test pattern
    forwin = False  # create for windows or for linux
elif setting == 3:
    # None
    nmon = 4  # number of monitors
    cylindrical = [False,False,False,False]  # apply flat plane to cylinder warping
    projection = [False,False,False,False]  # apply projection onto curved surface
    epsilon = [0.0,0.0,0.0,0.0]         # projector tilt [deg]
    lateral_offset = [0.0,-62.0,0.0,62.5]  # lateral offset [deg]
    vertical_offset = [0.0,0.0,0.0,0.0]    # vertical offset [deg]
    blending = [False,False,False,False]   # apply blending at sides
    blend_left_top = [0.0,0.0,0.0,0.0]
    blend_left_bot = [0.0,0.0,0.0,0.0]
    blend_right_top = [0.0,0.0,0.0,0.0]
    blend_right_bot = [0.0,0.0,0.0,0.0]
    gridtest = False # display grid test pattern
    forwin = False  # create for windows or for linux
elif setting == 4:
    # Testing
    nmon = 1  # number of monitors
    cylindrical = [True]  # apply flat plane to cylinder warping
    projection = [True]  # apply projection onto curved surfae
    epsilon = [5.75]         # projector tilt [deg]
    lateral_offset = [0.0]  # lateral offset [deg]
    vertical_offset = [0.0]    # vertical offset [deg]
    blending = [False]   # apply blending at sides
    blend_left_top = [0.0]
    blend_left_bot = [0.0]
    blend_right_top = [0.0]
    blend_right_bot = [0.0]
    gridtest = False # display grid test pattern
    forwin = False  # create for windows or for linux

# define output file
outfile = "X-Plane Window Positions.prf"

# pixel dimensions of projector
nx = 1920
ny = 1080

# number of x and y grid points
ngx = 101
ngy = 101

# Generate Output File and write header
con = open(outfile,"w")
con.write("I\n")
con.write("10 Version\n")
con.write("num_monitors "+str(nmon)+"\n")


# General Calculations independent of pixel position

ar =  float(nx) / float(ny)  # aspect ratio of projector image
beta = math.atan(1.0/(2.0*tr))*r2d
beta1 = 180.-beta
delta = math.asin(d_0/R*math.sin(beta1*d2r))*r2d
gamma = 180.-beta1-delta
R_1 = R*math.sin(gamma*d2r)/math.sin(beta1*d2r)
d_1 = R_1*math.cos(beta*d2r)
w_2 = R_1*math.sin(beta*d2r)

w = 2.0*w_2    # planar image width at screen distance
h = w / ar     # planar image height at screen distance

# loop through monitors
for mon in range(0,nmon,1):

    # calculate FOV of monitor
    FOVx = 2.0*gamma
    FOVy = 2.0*math.atan(0.5*h/(d_1+d_0))*r2d

    ## reset projection grid
    xabs = np.zeros((ngx, ngy))
    yabs = np.zeros((ngx, ngy))
    xdif = np.zeros((ngx, ngy))
    ydif = np.zeros((ngx, ngy))
    
    # loop through grid
    for gy in range(0,ngy,1):
        for gx in range(0,ngx,1):
            
            # Calculate Pixel position of grid point
            px = float(nx) * float(gx) / float(ngx-1)
            py = float(ny) * float(gy) / float(ngy-1)
            xabs[gx,gy] = px
            yabs[gx,gy] = py
            
            if cylindrical[mon]:

    # SCREEN CENTER Coordinate System
                # Graphic projection_geometry.pdf
                # BUT where x1/y1 is at x0/x0
                # AND where z1 is at center of screen

                # Calculate horizontal position on hypothetical
                # planar screen in distance d_1+d_0 from screen center
                # and respective horizontal projection angle
                a = w * (px - 0.5 * float(nx)) / float(nx)
                # Calculate horizontal view angle of pixel from screen center
                theta = math.atan(a/(d_1+d_0))*r2d

                # New X position in Cylindrical coordinates
                xdif[gx,gy] += 0.5 * float(nx) * theta / gamma + 0.5 * float(nx) - px

                # Calculate intersection of projected line with the cylindrical screen
                # behind hypothetical planar screen. Cylinder is in (0,0) position
                # https://mathworld.wolfram.com/Circle-LineIntersection.html
                x_1 = 0
                x_2 = d_1+d_0
                y_1 = 0.0
                y_2 = a
                d_x = x_2-x_1
                d_y = y_2-y_1
                d_r = math.sqrt(math.pow(d_x,2.0)+math.pow(d_y,2.0))
                D   = x_1*y_2-x_2*y_1
                DSCR = math.pow(R,2.0)*math.pow(d_r,2.0) - math.pow(D,2.0)  # Discriminant: if > 0 (true for our problem: intersection)

                if d_y < 0:
                    sgn_dy = -1.0
                else:
                    sgn_dy = 1.0

                x_c = (D*d_y + sgn_dy + d_x * math.sqrt(DSCR))/math.pow(d_r,2.0)
                y_c = (-D*d_x + d_y * math.sqrt(DSCR))/math.pow(d_r,2.0)

                # now calculate how much we need to shift pixel coordinates to match
                # cylindrical screen again
                d_2 = math.sqrt(math.pow(x_c-x_2,2.0) + math.pow(y_c-y_2,2.0))  # overshoot distance (should be 0 at screen edges)

                # Calculate Elevation of pixel at Planar Screen
                z_2 = (py-0.5*float(ny))/float(ny) * h
                # Calculate Elevation angle of pixel (relative to screen center height)
                alpha = math.atan(z_2/d_r)*r2d
                # Calculate Elevation of pixel at Cylinder
                z_c = math.tan(alpha*d2r) * R

                # New Y position in Cylindrical coordinates
                ydif[gx,gy] += float(ny) * z_c / h + 0.5 * float(ny) - py

                # print(str(px)+" "+str(theta)+" "+str(xdif[gx,gy])+" "+str(ydif[gx,gy]))
                # print(str(px)+" "+str(theta)+" "+str(z_2)+" "+str(z_c)+" "+str(h/2))

                # New FOVy due to larger vertical rendering at distance of cylinder center
                # BULLSHIT, FOVy does not change ...
                #if (gx == (ngx-1)/2) and (gy == (ngy-1)):
                #if (gx == (ngx-1)/4) and (gy == (ngy-1)):
                #    FOVy = 2.0*math.atan(z_c/(d_1+d_0))*r2d
                #    FOVy = 2.0*math.atan(z_c/R)*r2d
                
                # update grid coordinates for projector calculation
                px += xdif[gx,gy]
                py += ydif[gx,gy]
                
            if projection[mon]:

                # PROJECTOR CENTER Coordinate System
                # Graphic projection_geometry.pdf

                # Calculate horizontal position on hypothetical
                # planar screen in distance d_1 from projector
                # and respective horizontal projection angle
                a = w * (px - 0.5 * float(nx)) / float(nx)
                # Calculate horizontal view angle for pixel from projector
                theta = math.atan(a/d_1)*r2d

                # Calculate intersection of projected line with the cylindrical screen
                # behind hypothetical planar screen. Cylinder is in (0,0) position
                # https://mathworld.wolfram.com/Circle-LineIntersection.html
                x_1 = d_0
                x_2 = d_1+d_0
                y_1 = 0.0
                y_2 = a
                d_x = x_2-x_1
                d_y = y_2-y_1
                d_r = math.sqrt(math.pow(d_x,2.0)+math.pow(d_y,2.0))
                D   = x_1*y_2-x_2*y_1
                DSCR = math.pow(R,2.0)*math.pow(d_r,2.0) - math.pow(D,2.0)  # Discriminant: if > 0 (true for our problem: intersection)

                if d_y < 0:
                    sgn_dy = -1.0
                else:
                    sgn_dy = 1.0

                x_c = (D*d_y + sgn_dy + d_x * math.sqrt(DSCR))/math.pow(d_r,2.0)
                y_c = (-D*d_x + d_y * math.sqrt(DSCR))/math.pow(d_r,2.0)

                # now calculate how much we need to shift pixel coordinates to match
                # cylindrical screen again
                d_2 = math.sqrt(math.pow(x_c-x_2,2.0) + math.pow(y_c-y_2,2.0))  # overshoot distance (should be 0 at screen edges)
                d_d = d_r + d_2 # total horizontal distance from projector to cylindrical screen

                # Calculate Elevation angle of pixel
                z_2 = h_0 + py/float(ny) * h
                alpha = math.atan(z_2/d_r)*r2d

                z_c = math.tan(alpha*d2r) * d_d
                phi = math.atan(z_2/d_d)*r2d
                z_e = math.tan(phi*d2r) * d_r

                ex = px
                ey = py

                # Apply Keystone Correction
                # image gets compressed in both horizontal and vertical dimensions with projector tilt
                # so uncompress and untrapezoid the image pixel right in the first place and place them
                # where they would be with a untilted projector
                if epsilon[mon] != 0.0:
                    alpha_0 = math.atan(z_2/d_1)*r2d  # elevation angle at center line of projection
                    u = d_1 / math.cos(alpha_0*d2r)   # image distance at line py when projector would be level
                    u_k = d_1 / math.cos((alpha_0-epsilon[mon])*d2r)  # image distance at line py with tilt

                    ex = (ex-float(nx)/2.0)*u/u_k +  float(nx)/2.0  # correct keystone in horizontal pixels
                    ey = ey * u / u_k  # correct keystone in vertical pixels

                # Apply Cylindrical Warping Correction
                ex = ex
                ey = (z_e - h_0) / h * float(ny) - py + ey

                xdif[gx,gy] += ex - px
                ydif[gx,gy] += ey - py

    # End loop of xy grid

    print("Monitor: "+str(mon))
    print("FOVx:    "+str(FOVx))
    print("FOVy:    "+str(FOVy))
    
    # write header per monitor
    if mon == 0:
        con.write("monitor/"+str(mon)+"/m_window_idx 0"+"\n")
    else:
        con.write("monitor/"+str(mon)+"/m_window_idx -1"+"\n")

    if forwin:
        con.write("monitor/"+str(mon)+"/m_monitor ",mon+"\n")
    else:
        con.write("monitor/"+str(mon)+"/m_monitor 0"+"\n")

    con.write("monitor/"+str(mon)+"/m_window_bounds/0 0"+"\n")
    con.write("monitor/"+str(mon)+"/m_window_bounds/1 0"+"\n")
    con.write("monitor/"+str(mon)+"/m_window_bounds/2 "+str(nx)+"\n")
    con.write("monitor/"+str(mon)+"/m_window_bounds/3 "+str(ny)+"\n")
    con.write("monitor/"+str(mon)+"/m_arbitrary_bounds/0 0"+"\n")
    con.write("monitor/"+str(mon)+"/m_arbitrary_bounds/1 0"+"\n")
    con.write("monitor/"+str(mon)+"/m_arbitrary_bounds/2 "+str(nx)+"\n")
    con.write("monitor/"+str(mon)+"/m_arbitrary_bounds/3 "+str(ny)+"\n")
    con.write("monitor/"+str(mon)+"/m_bpp 24"+"\n")
    con.write("monitor/"+str(mon)+"/m_refresh_rate 0"+"\n")
    con.write("monitor/"+str(mon)+"/m_x_res_full "+str(nx)+"\n")
    con.write("monitor/"+str(mon)+"/m_y_res_full "+str(ny)+"\n")
    con.write("monitor/"+str(mon)+"/m_is_fullscreen wmgr_mode_fullscreen"+"\n")
    con.write("monitor/"+str(mon)+"/m_usage wmgr_usage_normal_visuals"+"\n")
    con.write("monitor/"+str(mon)+"/proj/window_2d_off 1"+"\n")
    if cylindrical[mon]:
        con.write("monitor/"+str(mon)+"/proj/diff_FOV 1"+"\n")
    else:
        con.write("monitor/"+str(mon)+"/proj/diff_FOV 0"+"\n")    
    con.write("monitor/"+str(mon)+"/proj/FOVx_renopt "+str(format(FOVx,('.6f')))+"\n")
    con.write("monitor/"+str(mon)+"/proj/FOVy_renopt "+str(format(FOVy,('.6f')))+"\n")
    con.write("monitor/"+str(mon)+"/proj/os_x_rat 0.000000"+"\n")
    con.write("monitor/"+str(mon)+"/proj/os_y_rat 0.000000"+"\n")
    con.write("monitor/"+str(mon)+"/proj/off_vrt_deg "+str(format(vertical_offset[mon],('.6f')))+"\n")
    con.write("monitor/"+str(mon)+"/proj/off_lat_deg "+str(format(lateral_offset[mon],('.6f')))+"\n")
    con.write("monitor/"+str(mon)+"/proj/off_phi_deg 0.000000"+"\n")
    if gridtest:
        con.write("monitor/"+str(mon)+"/proj/grid_os_on_test 1"+"\n")
    else:
        con.write("monitor/"+str(mon)+"/proj/grid_os_on_test 0"+"\n")

    if projection[mon]:
        con.write("monitor/"+str(mon)+"/proj/grid_os_on_render 1"+"\n")
    else:
        con.write("monitor/"+str(mon)+"/proj/grid_os_on_render 0"+"\n")

    con.write("monitor/"+str(mon)+"/proj/grid_os_drag_dim_i 4"+"\n")
    con.write("monitor/"+str(mon)+"/proj/grid_os_drag_dim_j 4"+"\n")

    # write grid per monitor
    if (projection[mon] or cylindrical[mon]):
        for gx in range(0,ngx,1):
            for gy in range(0,ngy,1):
                con.write("monitor/"+str(mon)+"/proj/grid_os_x/"+str(gx)+"/"+str(gy)+" "+str(format(xdif[gx,gy],('.6f')))+"\n")

        for gx in range(0,ngx,1):
            for gy in range(0,ngy,1):
                con.write("monitor/"+str(mon)+"/proj/grid_os_y/"+str(gx)+"/"+str(gy)+" "+str(format(ydif[gx,gy],('.6f')))+"\n")


    # write blending per monitor
    if blending[mon]:
        con.write("monitor/"+str(mon)+"/proj/gradient_on 1"+"\n")
    else:
        con.write("monitor/"+str(mon)+"/proj/gradient_on 0"+"\n")


    con.write("monitor/"+str(mon)+"/proj/gradient_width_top/0 "+str(format(blend_left_top[mon],('.6f')))+"\n")
    con.write("monitor/"+str(mon)+"/proj/gradient_width_top/1 "+str(format(blend_right_top[mon],('.6f')))+"\n")
    con.write("monitor/"+str(mon)+"/proj/gradient_width_ctr/0 0.000000"+"\n")
    con.write("monitor/"+str(mon)+"/proj/gradient_width_ctr/1 0.000000"+"\n")
    con.write("monitor/"+str(mon)+"/proj/gradient_width_bot/0 "+str(format(blend_left_bot[mon],('.6f')))+"\n")
    con.write("monitor/"+str(mon)+"/proj/gradient_width_bot/1 "+str(format(blend_right_bot[mon],('.6f')))+"\n")
    con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/0 1.000000"+"\n")
    con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/1 0.650000"+"\n")
    con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/2 0.330000"+"\n")
    con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/3 0.000000"+"\n")
    con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/0 1.000000"+"\n")
    con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/1 0.650000"+"\n")
    con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/2 0.330000"+"\n")
    con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/3 0.000000"+"\n")

    if doplot:
        plot.scatter(xabs+xdif,yabs+ydif,color="red",marker=".")
        plot.show()




# end loop of monitors

con.close()
