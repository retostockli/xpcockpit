from __future__ import print_function
# GTX970 Output Numbering
# DVI0 (DVI-I-1): 0
# HDMI/HP (DP-1): 1
# DVI1 (DVI-D-0): 2
# HDMI (HDMI-0): 3

# GTX4080 Output Numbering
# DP-3: DPY-5
# DP-5: DPY-7
# DP-1: DPY-2
# HDMI-0: DPY-0

# X-Plane 11: need to activate VULKAN in Graphics Settings
# However: With Vulkan, Cylindrical projection has limited rendering in corners
# To have scenery only view: default_view 1 in X-Plane.prf
# To have cylindrical projection: proj_cyl 1 in X-Plane.prf

# Import Libraries
import math
import numpy as np
import matplotlib.pyplot as plot

def LinePlaneCollision(planeNormal, planePoint, rayDirection, rayPoint, epsilon=1e-6):

	ndotu = planeNormal.dot(rayDirection)
	if abs(ndotu) < epsilon:
		raise RuntimeError("no intersection or line is within plane")

	w = rayPoint - planePoint
	si = -planeNormal.dot(w) / ndotu
#	Psi = w + si * rayDirection + planePoint
	Psi = rayPoint + si * rayDirection
	return Psi

def LineCircleCollision(linePoint1,linePoint2,circleRadius):
                
    # Calculate intersection of line with circle. Circle has
    # Radius R and is centered at (0,0)
    # https://mathworld.wolfram.com/Circle-LineIntersection.html
       
    x_1 = linePoint1[0]
    x_2 = linePoint2[0]
    y_1 = linePoint1[1]
    y_2 = linePoint2[1]
    R = circleRadius
    
    d_x = x_2-x_1
    d_y = y_2-y_1
    d_r = math.sqrt(math.pow(d_x,2.0)+math.pow(d_y,2.0))
    D   = x_1*y_2-x_2*y_1
    DSCR = math.pow(R,2.0)*math.pow(d_r,2.0) - math.pow(D,2.0)  # Discriminant: if > 0 (true for our problem: intersection)
    
    if d_y < 0:
        sgn_dy = -1.0
    else:
        sgn_dy = 1.0
        
    # this is where we need to have the pixel on the cylinder in cylindrical coordinates
    x_c = (D*d_y + sgn_dy + d_x * math.sqrt(DSCR))/math.pow(d_r,2.0)
    y_c = (-D*d_x + d_y * math.sqrt(DSCR))/math.pow(d_r,2.0)

    return np.array([x_c,y_c])

    
# Testing a function
test = False

# Settings
setting = 1

# Graphics
doplot = False

# Parameters
d2r = math.pi/180.
r2d = 180./math.pi

# Dimensions (in cm)
# Please see projector_setup.pdf
R = 169.5   # Screen Radius
d_0 = 28.0   # Distance of Projector focal point from center of cylinder (positive is towards screen)
# Projector focal point may be behind projector lens. For me it is around 2-4 cm behind
# as it seems. No documentation found.
h_0 = 12.5   # lower height of image above center of lens when projected on planar screen from untilted projector
tr = 0.49   # Projector Throw ratio

if setting == 1:
    # Cylindrical + Projection + Blending
    nmon = 4  # number of monitors
    ceiling = True  # projector ceiling mount instead of table mount
    cylindrical = [False,True,True,True]  # apply flat plane to cylinder warping
    projection = [False,True,True,True]  # apply projection onto curved surface
    epsilon = [0.0,0.0,5.45,0.0]         # projector tilt [deg]
    lateral_offset = [0.0,-64.0,0.0,63.75]  # lateral offset [deg]
    vertical_offset = [0.0,0.0,0.0,0.0]    # vertical offset [deg]
    vertical_shift = [0.0,10.0,0.0,0.0]    # vertical shift [pixel]
    vertical_scale = [1.0,1.006,1.0,0.98]    # vertical scale [-]
    vertical_scale = [1.0,0.98,1.0,0.965]    # vertical scale [-]
    blending = [False,True,True,True]   # apply blending at sides
    blend_left_top = [0.0,0.0,287.0,234.0]
    blend_left_bot = [0.0,0.0,341.0,318.0]
    blend_right_top = [0.0,249.0,257.0,0.0]
    blend_right_bot = [0.0,333.0,318.0,0.0]
    gridtest = False # display grid test pattern
    forwin = True  # create for windows or for linux
elif setting == 2:
    # Projection + Blending
    nmon = 4  # number of monitors
    ceiling = True  # projector ceiling mount instead of table mount
    cylindrical = [False,False,False,False]  # apply flat plane to cylinder warping
    projection = [False,True,True,True]  # apply projection onto curved surface
    epsilon = [0.0,0.0,5.45,0.0]         # projector tilt [deg]
    lateral_offset = [0.0,-64.0,0.0,63.75]  # lateral offset [deg]
    vertical_offset = [0.0,0.0,0.0,0.0]    # vertical offset [deg]
    vertical_shift = [0.0,0.0,0.0,0.0]    # vertical shift [pixel]
    vertical_scale = [1.0,1.006,1.0,0.98]    # vertical scale [-]
    blending = [False,True,True,True]   # apply blending at sides
    blend_left_top = [0.0,0.0,287.0,234.0]
    blend_left_bot = [0.0,0.0,341.0,318.0]
    blend_right_top = [0.0,249.0,257.0,0.0]
    blend_right_bot = [0.0,333.0,318.0,0.0]
    gridtest = False # display grid test pattern
    forwin = False  # create for windows or for linux
elif setting == 3:
    # Projection
    nmon = 4  # number of monitors
    ceiling = True  # projector ceiling mount instead of table mount
    cylindrical = [False,False,False,False]  # apply flat plane to cylinder warping
    projection = [False,True,True,True]  # apply projection onto curved surface
    epsilon = [0.0,0.0,5.45,0.0]         # projector tilt [deg]
    epsilon = [0.0,0.0,6.5,0.0]         # projector tilt [deg]
    lateral_offset = [0.0,-64.0,0.0,63.75]  # lateral offset [deg]
    vertical_offset = [0.0,0.0,0.0,0.0]    # vertical offset [deg]
    vertical_shift = [0.0,0.0,0.0,0.0]    # vertical shift [pixel]
    vertical_scale = [1.0,1.006,1.0,0.98]    # vertical scale [-]
    blending = [False,False,False,False]   # apply blending at sides
    blend_left_top = [0.0,0.0,287.0,234.0]
    blend_left_bot = [0.0,0.0,341.0,318.0]
    blend_right_top = [0.0,249.0,257.0,0.0]
    blend_right_bot = [0.0,333.0,318.0,0.0]
    gridtest = True # display grid test pattern
    forwin = False  # create for windows or for linux
elif setting == 4:
    # None
    nmon = 4  # number of monitors
    ceiling = True  # projector ceiling mount instead of table mount
    cylindrical = [False,False,False,False]  # apply flat plane to cylinder warping
    projection = [False,False,False,False]  # apply projection onto curved surface
    epsilon = [0.0,0.0,0.0,0.0]         # projector tilt [deg]
    lateral_offset = [0.0,-68.5,0.0,68.25]  # lateral offset [deg]
    vertical_offset = [0.0,0.0,0.0,0.0]    # vertical offset [deg]
    vertical_shift = [0.0,0.0,0.0,0.0]    # vertical shift [pixel]
    vertical_scale = [1.0,1.0,1.0,1.0]    # vertical scale [-]
    blending = [False,False,False,False]   # apply blending at sides
    blend_left_top = [0.0,0.0,0.0,0.0]
    blend_left_bot = [0.0,0.0,0.0,0.0]
    blend_right_top = [0.0,0.0,0.0,0.0]
    blend_right_bot = [0.0,0.0,0.0,0.0]
    gridtest = False # display grid test pattern
    forwin = False  # create for windows or for linux
elif setting == 5:
    # Testing Single Monitor
    nmon = 1  # number of monitors
    ceiling = False  # projector ceiling mount instead of table mount
    cylindrical = [False]  # apply flat plane to cylinder warping
    projection = [True]  # apply projection onto curved surfae
    epsilon = [5.45]         # projector tilt [deg]
    lateral_offset = [0.0]  # lateral offset [deg]
    vertical_offset = [0.0]    # vertical offset [deg]
    vertical_shift = [0.0]    # vertical shift [pixel]
    vertical_scale = [1.0]    # vertical scale [-]
    blending = [False]   # apply blending at sides
    blend_left_top = [0.0]
    blend_left_bot = [0.0]
    blend_right_top = [0.0]
    blend_right_bot = [0.0]
    gridtest = False # display grid test pattern
    forwin = False  # create for windows or for linux
elif setting == 6:
    # Testing Two Monitors
    nmon = 2  # number of monitors
    ceiling = True  # projector ceiling mount instead of table mount
    cylindrical = [False,True]  # apply flat plane to cylinder warping
    projection = [False,True]  # apply projection onto curved surfae
    epsilon = [0.0,6.42]         # projector tilt [deg]
    lateral_offset = [0.0,0.0]  # lateral offset [deg]
    vertical_offset = [0.0,0.0]    # vertical offset [deg]
    vertical_shift = [0.0,0.0]    # vertical shift [pixel]
    vertical_scale = [1.0,1.0]    # vertical scale [-]
    blending = [False,False]   # apply blending at sides
    blend_left_top = [0.0,0.0]
    blend_left_bot = [0.0,0.0]
    blend_right_top = [0.0,0.0]
    blend_right_bot = [0.0,0.0]
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
beta = math.atan(1.0/(2.0*tr))*r2d # Maximum horizontal FOV from Projector [deg]
beta1 = 180.-beta   # well ... check out the drawing yourself
delta = math.asin(d_0/R*math.sin(beta1*d2r))*r2d  # same here
gamma = 180.-beta1-delta  # Maximum horizontal FOV from Screen Center [deg]
R_1 = R*math.sin(gamma*d2r)/math.sin(beta1*d2r) # Maximum Distance of Projector to screen edge
d_1 = R_1*math.cos(beta*d2r) # distance of projector to hypothetical planar screen in front of cylindrical screen
w_2 = R_1*math.sin(beta*d2r) # half of hypothetical planar image width at screen distance

w = 2.0*w_2    # planar image width at screen distance (as if projection would be on planar scren)
h = w / ar     # planar image height at screen distance (as if projection ... )

print(w,h)

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
    # grid vertical: gy goes from bottom to top
    # grid horizontal: gx from left to right
    for gy in range(0,ngy,1):
        for gx in range(0,ngx,1):
#    for gy in range(0,1,1):
#        for gx in range(50,51,1):
            
            # Calculate Pixel position of grid point (top-left is 0/0 and bottom-right is nx/ny)
            px = float(nx) * float(gx) / float(ngx-1)
            py = float(ny) * float(gy) / float(ngy-1)
            xabs[gx,gy] = px
            yabs[gx,gy] = py
   
            # add vertical shift and scale if needed
            ey = py * vertical_scale[mon] + vertical_shift[mon]
            ydif[gx,gy] = ey - py
            py = ey
            
            # 1. Transformation from planar to cylindrical rendering
            # --> This has nothing to do with the projector orientation and mount etc.
            # --> This is needed since X-Plane renders on a plane but we need a cylindrical rendering
            # Please see planar_to_cylindrical.pdf
            if cylindrical[mon]:

                # COMPLETELY REWORK CYLINDRICAL PROJECTION
                
                # Input is a planar image in x/y coordinates
                # Output is a cylindrical image in degrees and height
                
                # SCREEN CENTER Coordinate System
                # BUT where x1/y1 is at x0/x0
                # AND where z1 is at center of screen
               
                # Calculate horizontal position on hypothetical
                # planar screen in distance d_1+d_0 from screen center
                # and respective horizontal projection angle
                a = w * (px - 0.5 * float(nx)) / float(nx)
                # Calculate Elevation of pixel on hypothetical Planar Screen
                b = (py-0.5*float(ny))/float(ny) * h

                # Calculate horizontal view angle of pixel from screen center
                theta = math.atan(a/(d_1+d_0))*r2d

                # New X position in Cylindrical coordinates (nx is distributed over width 2*gamma)
                xdif[gx,gy] += 0.5 * float(nx) * theta / gamma + 0.5 * float(nx) - px

                # Calculate intersection of projected line with the cylindrical screen
                linePoint1 = np.array([0.0,0.0])
                linePoint2 = np.array([d_0+d_1,a])
                circleRadius = R
                col = LineCircleCollision(linePoint1,linePoint2,circleRadius)
                x_c = col[0]
                y_c = col[1]

                # Calculate Elevation angle of pixel (relative to screen center height)
                d_r = math.sqrt(math.pow(d_0+d_1,2.0)+math.pow(a,2.0))
                alpha = math.atan(b/d_r)*r2d
                # Calculate Elevation of pixel at cylinder
                z_c = math.tan(alpha*d2r) * R

                # New Y position in Cylindrical coordinates
                ydif[gx,gy] += float(ny) * z_c / h + 0.5 * float(ny) - py
        
                # update grid coordinates for keystone and projection calculation
                px += xdif[gx,gy]
                py += ydif[gx,gy]

            # 2. Apply Keystone Correction === TESTING ONLY
            # Keystone correction is directly implemented in warping below
            # image gets compressed in both horizontal and vertical dimensions with projector tilt
            # so uncompress and untrapezoid the image pixel right in the first place and place them
            # where they would be with a untilted projector
            if (epsilon[mon] != 0.0) and test:
              
                # minimum elevation angle at h_0
                alpha_h = math.atan(h_0/d_1)*r2d 
                # new minimum height of projected tilted image onto vertical plane
                # equal h_0 without tilt epsilon
                h_1 = math.tan((alpha_h - epsilon[mon])*d2r)*d_1 
 
                # Calculate horizontal position on hypothetical
                # planar screen in distance d_1 from projector
                # and respective horizontal projection angle
                a = w * (px - 0.5 * float(nx)) / float(nx)
                # Calculate elevation of pixel at hypothetical planar screen
                b = h_1 + py/float(ny) * h                

                # now calculate keystone correction: transform new coordinates on vertical
                # hypotetical planar screen to tilted hypothetical planar screen: intersect
                # projector ray with tilted plane
                #Define plane
                e = math.cos(epsilon[mon]*d2r)*d_1
                planeNormal = np.array([-e, 0.0, math.sin(epsilon[mon]*d2r)*d_1])
                planePoint = np.array([e+d_0, 0.0, -math.sin(epsilon[mon]*d2r)*d_1]) # Any point on the plane

                #Define ray
                rayDirection = np.array([d_1, a, b])
                rayPoint = np.array([d_0, 0.0, 0.0]) # Any point along the ray
                
                cp = LinePlaneCollision(planeNormal, planePoint, rayDirection, rayPoint)
#                print ("intersection at",d_0+d_1,a,b,cp[0],cp[1],cp[2])

                # transform intersection to tilted plane coordinates
                # Calculate distance difference of projector line from vertical hypothetical planar screen
                f = cp[0] - e - d_0

                a_1 = cp[1]
                b_1 = f / math.sin(epsilon[mon]*d2r)

                # Apply Cylindrical Warping Correction
                ex = a_1 / w * float(nx) + 0.5 * float(nx)
                ey = (b_1 - h_0) / h * float(ny) 

                xdif[gx,gy] += ex - px
                ydif[gx,gy] += ey - py
                                
            # 3. Warping the planar projection of a regular table or ceiling mounted projector
            # onto a cylindrical screen. This is needed since the projector image is only ok
            # on a flat screen. We need to squeeze it onto a cylinder.
            # Please see projection_geometry.pdf
            if projection[mon] and not test:

                # INPUT IS IN CYLINDRICAL COORDINATES ALREADY
                # SO X IS in degrees and Y is in height
                
                # PROJECTOR CENTER Coordinate System
               
                # minimum elevation angle at h_0
                alpha_h = math.atan(h_0/d_1)*r2d 
                # new minimum height of projected tilted image onto vertical plane
                # equal h_0 without tilt epsilon
                h_1 = math.tan((alpha_h - epsilon[mon])*d2r)*d_1 
                
                # Calculate horizontal position on hypothetical
                # planar screen in distance d_1 from projector
                # and respective horizontal projection angle
                a = w * (px - 0.5 * float(nx)) / float(nx)
                # Calculate elevation of pixel at hypothetical planar screen
                b = h_1 + py/float(ny) * h
                   
                # Calculate horizontal view angle for pixel from projector
                theta = math.atan(a/d_1)*r2d
                # Calculate horizontal view angle for pixel from screen center
                # assuming input from projector is in degrees FOVx
                psi = a / w_2 * gamma

                # Calculate intersection of projected line with the cylindrical screen
                linePoint1 = np.array([0.0,0.0])
                linePoint2 = np.array([d_0+d_1,math.tan(psi*d2r)*(d_0+d_1)])
                circleRadius = R
                col = LineCircleCollision(linePoint1,linePoint2,circleRadius)
                x_c = col[0]
                y_c = col[1]
                
                # calculate adjusted horizontal view angle from projector center
                theta_1 = math.atan(y_c/(x_c-d_0))*r2d

                # calculate new horizontal pixel position to reach that view angle
                a_1 = math.tan(theta_1*d2r)*d_1
                
                # now calculate how much we need to shift pixel coordinates to match
                # cylindrical screen again. Origin is projector center
                d_2 = math.sqrt(math.pow(x_c-d_1-d_0,2.0) + math.pow(y_c-a_1,2.0))  # overshoot distance (should be 0 at screen edges)
                # Projected image can be behind screen
                if (x_c-d_1-d_0)<0.0:
                    d_2 = -d_2

                d_r = math.sqrt(math.pow(d_1,2.0)+math.pow(a_1,2.0))
                d_d = d_r + d_2 # total horizontal distance from projector to cylindrical screen

                # calculate elevation angle of pixel on hypothetical planar screen
                alpha = math.atan(b/d_r)*r2d              
                # calculate elevation of pixel at cylinder
                z_c = math.tan(alpha*d2r) * d_d
                # calculate new elevation angle of shifted pixel at cylinder
                alpha_1 = math.atan(b/d_d)*r2d
                # calculate new vertical pixel position of shifted pixel at hypothetical planar screen
                b_1 = math.tan(alpha_1*d2r) * d_r

                # print(a_1,b_1)
                
                if epsilon[mon] != 0.0:
                    # now calculate keystone correction: transform new coordinates on vertical
                    # hypotetical planar screen to tilted hypothetical planar screen: intersect
                    # projector ray with tilted plane
                    #Define plane
                    e = math.cos(epsilon[mon]*d2r)*d_1
                    planeNormal = np.array([-e, 0.0, math.sin(epsilon[mon]*d2r)*d_1])
                    planePoint = np.array([e+d_0, 0.0, -math.sin(epsilon[mon]*d2r)*d_1]) # Any point on the plane
                    
                    #Define ray
                    rayDirection = np.array([d_1, a_1, b_1])
                    rayPoint = np.array([d_0, 0.0, 0.0]) # Any point along the ray
                    
                    cp = LinePlaneCollision(planeNormal, planePoint, rayDirection, rayPoint)
                   
                    # transform intersection to tilted plane coordinates
                    # Calculate distance difference of projector line from vertical hypothetical planar screen
                    f = cp[0] - e - d_0
                    
                    a_2 = cp[1]
                    b_2 = f / math.sin(epsilon[mon]*d2r)

                    # print(a_2,b_2)
                                        
                    # Apply Cylindrical Warping Correction with tilted plane
                    ex = a_2 / w * float(nx) + 0.5 * float(nx)
                    ey = (b_2 - h_0) / h * float(ny)
                    
                else:
                    # Apply Cylindrical Warping Correction with vertical plane
                    ex = a_1 / w * float(nx) + 0.5 * float(nx)
                    ey = (b_1 - h_0) / h * float(ny)                                         

                # print(ex,ey)
                
                xdif[gx,gy] += ex - px
                ydif[gx,gy] += ey - py

    # End loop of xy grid
    
    # inverse x and y array for ceiling mount
    if ceiling:
        ydif = -np.flip(ydif,axis=1)
        xdif = np.flip(xdif,axis=1)
        
    print("Monitor: "+str(mon))
    print("FOVx:    "+str(FOVx))
    print("FOVy:    "+str(FOVy))
    
    # write header per monitor
    if mon == 0:
        con.write("monitor/"+str(mon)+"/m_window_idx 0"+"\n")
    else:
        con.write("monitor/"+str(mon)+"/m_window_idx -1"+"\n")

    if forwin:
        con.write("monitor/"+str(mon)+"/m_monitor "+str(mon)+"\n")
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

    if (projection[mon] or cylindrical[mon]):
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
    con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/1 0.660000"+"\n")
    con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/2 0.330000"+"\n")
    con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/3 0.000000"+"\n")
    con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/0 1.000000"+"\n")
    con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/1 0.660000"+"\n")
    con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/2 0.330000"+"\n")
    con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/3 0.000000"+"\n")

    if doplot:
        plot.figure(figsize=(10,6))
        plot.scatter(xabs+xdif,yabs+ydif,color="red",marker=".")
        plot.show()




# end loop of monitors

con.close()
