# This code will generate a X-Plane Warp and Blend Preferenec file by specifying
# projector and screen geometric boundary conditions.
# The Preference file can then be used in X-Plane directly to drive X-Plane's internal
# Warping Code. It can also be used to drive the NVIDIA native warping and blending
# API found in the src/ directory. The latter comes with no FPS loss and runs fully on the GPU
# while X-Plane's warping and blending uses a substantial amount of CPU. So if you're CPU limited
# in X-Plane, you might want to try the native NVIDIA warping and blending.

# NOTE: The NVIDIA Warping and Blending presented here is only implemented on Linux so far.
# It also exists on Windows, so please feel free to port the code in src/ to Windows.

# View Settings in X-Plane:
# To have scenery only view: default_view 1 in X-Plane.prf
# X-Plane internal Cylindrical Projection (not needed with this script)
# To have cylindrical projection: proj_cyl 1 in X-Plane.prf

# Import Libraries
from __future__ import print_function
import math
import numpy as np
import matplotlib.pyplot as plot

# Which of the settings below do you want
setting = 4

# Plot Warping grid for Checking
doplot = False

# Utilize new X-Plane >= 12.08 Window Position File Format
newformat = True

# Projector and Screen Dimensions [cm]
# Please see projector_setup.pdf
R = 169.5    # Cylindrical Screen Radius
d_0 = 28.0   # Distance of Projector focal point from center of cylinder (positive is towards screen)
# Projector focal point may be behind projector lens front. No documentation found.
# Assume the position of the lens front if in doubt
h_0 = 12.5   # lower height of image above focal point when projected on planar screen from untilted projector
# a) Hypothetical planar screen is mounted at distance d_1 from lens.
# b) If focal point is not known, assume lens center as reference to lower height of image
tr = 0.49    # Projector Throw ratio (distance / width of screen)

# define output file
outfile = "X-Plane Window Positions.prf"

# pixel dimensions of projector
nx = 1920
ny = 1080

# number of x and y grid points for warping grid (101 is needed for X-Plane 11 / 12)
if newformat:
    ngx = 40
    ngy = 40
else:
    ngx = 101
    ngy = 101

# vertical offset [deg] is not implemented, but could be implemented in planar to cylindrical projection
if setting == 1:
    # Cylindrical + Projection + Blending
    nmon = 4  # number of monitors
    ceiling = True  # projector ceiling mount instead of table mount
    cylindrical = [False,True,True,True]  # apply flat plane to cylinder warping
    projection = [False,True,True,True]  # apply projection onto curved surface
    epsilon = [0.0,0.0,6.5,0.0]         # projector tilt [deg]
    frustum = 0.0 # vertical shift of horizon in normalized image coordinates [-1..1]
    lateral_offset = [0.0,-68.1,0.0,67.9]  # lateral offset [deg]
    vertical_offset = [0.0,0.0,0.0,0.0]    # vertical offset [deg]
    vertical_shift = [0.0,12.0,0.0,7.0]    # vertical shift [pixel]
    vertical_scale = [1.0,0.99,1.0,0.970]    # vertical scale [-]
    blending = [False,True,True,True]   # apply blending at sides
    blend_left_top =  [  0.0,   0.0, 279.0, 282.0]
    blend_left_bot =  [  0.0,   0.0, 192.0, 197.0]
    blend_right_top = [  0.0, 283.0, 287.0,   0.0]
    blend_right_bot = [  0.0, 192.0, 205.0,   0.0]
    gridtest = False # display grid test pattern
    blendtest = False  # cut blend sharp for testing
    forwin = False  # create for windows or for linux
    savegrid = True # save proection grid or just FOV
elif setting == 2:
    # Projection + Blending
    nmon = 4  # number of monitors
    ceiling = True  # projector ceiling mount instead of table mount
    cylindrical = [False,False,False,False]  # apply flat plane to cylinder warping
    projection = [False,True,True,True]  # apply projection onto curved surface
    epsilon = [0.0,0.0,6.5,0.0]         # projector tilt [deg]
    frustum = 0.0 # vertical shift of horizon in normalized image coordinates [-1..1]
    lateral_offset = [0.0,-68.1,0.0,67.9]  # lateral offset [deg]
    vertical_offset = [0.0,0.0,0.0,0.0]    # vertical offset [deg]
    vertical_shift = [0.0,12.0,0.0,7.0]    # vertical shift [pixel]
    vertical_scale = [1.0,0.99,1.0,0.970]    # vertical scale [-]
    blending = [False,True,True,True]   # apply blending at sides
    blend_left_top =  [  0.0,   0.0, 228.0, 224.0]
    blend_left_bot =  [  0.0,   0.0, 124.0, 156.0]
    blend_right_top = [  0.0, 224.0, 236.0,   0.0]
    blend_right_bot = [  0.0, 152.0, 136.0,   0.0]
    gridtest = False # display grid test pattern
    blendtest = False  # cut blend sharp for testing
    forwin = True  # create for windows or for linux
    savegrid = True # save proection grid or just FOV
elif setting == 3:
    # Projection
    nmon = 4  # number of monitors
    ceiling = True  # projector ceiling mount instead of table mount
    cylindrical = [False,False,False,False]  # apply flat plane to cylinder warping
    projection = [False,True,True,True]  # apply projection onto curved surface
    epsilon = [0.0,0.0,6.5,0.0]         # projector tilt [deg]
    frustum = 0.0 # vertical shift of horizon in normalized image coordinates [-1..1]
    lateral_offset = [0.0,-68.1,0.0,67.9]  # lateral offset [deg]
    vertical_offset = [0.0,0.0,0.0,0.0]    # vertical offset [deg]
    vertical_shift = [0.0,12.0,0.0,7.0]    # vertical shift [pixel]
    vertical_scale = [1.0,0.99,1.0,0.970]    # vertical scale [-]
    blending = [False,False,False,False]   # apply blending at sides
    blend_left_top =  [  0.0,   0.0, 228.0, 224.0]
    blend_left_bot =  [  0.0,   0.0, 124.0, 156.0]
    blend_right_top = [  0.0, 224.0, 236.0,   0.0]
    blend_right_bot = [  0.0, 152.0, 136.0,   0.0]
    gridtest = False # display grid test pattern
    blendtest = False  # cut blend sharp for testing
    forwin = True  # create for windows or for linux
    savegrid = True # save proection grid or just FOV
elif setting == 4:
    # None (use this for X-Plane when using warpblend with NVIDIA directly)
    nmon = 4  # number of monitors
    ceiling = True  # projector ceiling mount instead of table mount
    cylindrical = [False,True,True,True]  # apply flat plane to cylinder warping
#    cylindrical = [False,False,False,False]  # apply flat plane to cylinder warping
    projection = [False,False,False,False]  # apply projection onto curved surface
    epsilon = [0.0,0.0,0.0,0.0]         # projector tilt [deg]
    frustum = 0.0 # vertical shift of horizon in normalized image coordinates [-1..1]
    lateral_offset = [0.0,-68.1,0.0,67.9]  # lateral offset [deg]
    vertical_offset = [0.0,0.0,0.0,0.0]    # vertical offset [deg]
    vertical_shift = [0.0,0.0,0.0,0.0]    # vertical shift [pixel]
    vertical_scale = [1.0,1.0,1.0,1.0]    # vertical scale [-]
    blending = [False,False,False,False]   # apply blending at sides
    blend_left_top = [0.0,0.0,0.0,0.0]
    blend_left_bot = [0.0,0.0,0.0,0.0]
    blend_right_top = [0.0,0.0,0.0,0.0]
    blend_right_bot = [0.0,0.0,0.0,0.0]
    gridtest = False # display grid test pattern
    blendtest = False  # cut blend sharp for testing
    forwin = False  # create for windows or for linux
    savegrid = False # save proection grid or just FOV
elif setting == 5:
    # Only Testing Grid
    nmon = 4  # number of monitors
    ceiling = True  # projector ceiling mount instead of table mount
    cylindrical = [False,False,False,False]  # apply flat plane to cylinder warping
    projection = [False,False,False,False]  # apply projection onto curved surface
    epsilon = [0.0,0.0,0.0,0.0]         # projector tilt [deg]
    frustum = 0.0 # vertical shift of horizon in normalized image coordinates [-1..1]
    lateral_offset = [0.0,-68.1,0.0,67.9]  # lateral offset [deg]
    vertical_offset = [0.0,0.0,0.0,0.0]    # vertical offset [deg]
    vertical_shift = [0.0,0.0,0.0,0.0]    # vertical shift [pixel]
    vertical_scale = [1.0,1.0,1.0,1.0]    # vertical scale [-]
    blending = [False,False,False,False]   # apply blending at sides
    blend_left_top = [0.0,0.0,0.0,0.0]
    blend_left_bot = [0.0,0.0,0.0,0.0]
    blend_right_top = [0.0,0.0,0.0,0.0]
    blend_right_bot = [0.0,0.0,0.0,0.0]
    gridtest = True # display grid test pattern
    blendtest = False  # cut blend sharp for testing
    forwin = False  # create for windows or for linux
    savegrid = True # save proection grid or just FOV
elif setting == 6:
    # Testing Single Monitor
    nmon = 1  # number of monitors
    ceiling = False  # projector ceiling mount instead of table mount
    cylindrical = [True]  # apply flat plane to cylinder warping
    projection = [False]  # apply projection onto curved surfae
    epsilon = [0.0]         # projector tilt [deg]
    frustum = 0.0 # vertical shift of horizon in normalized image coordinates [-1..1]
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
    blendtest = False  # cut blend sharp for testing
    forwin = False  # create for windows or for linux
    savegrid = False # save proection grid or just FOV
elif setting == 7:
    # Testing Two Monitors
    nmon = 2  # number of monitors
    ceiling = True  # projector ceiling mount instead of table mount
    cylindrical = [False,True]  # apply flat plane to cylinder warping
    projection = [False,True]  # apply projection onto curved surfae
    epsilon = [0.0,6.42]         # projector tilt [deg]
    frustum = 0.0 # vertical shift of horizon in normalized image coordinates [-1..1]
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
    blendtest = False  # cut blend sharp for testing
    forwin = False  # create for windows or for linux
    savegrid = True # save proection grid or just FOV

#----------- FUNCTIONS --------------

def LinePlaneCollision(planeNormal, planePoint, rayDirection, rayPoint, epsilon=1e-6):

	ndotu = planeNormal.dot(rayDirection)
	if abs(ndotu) < epsilon:
		raise RuntimeError("no intersection or line is within plane")

	w = rayPoint - planePoint
	si = -planeNormal.dot(w) / ndotu
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

def Extrap(xmin,ymin,xmax,ymax,x):
        # Linear Scaling Function
        return (x - xmin) / (xmax - xmin) * (ymax - ymin) + ymin


def PanoramaProj_inverse(thetaphi,FOVx,FOVy,vert_stretch):
        # Formulas gracefully provided by Austin Meyer of X-Plane
        # X-Plane Panorama Projection Inverse function: input is in normalized degrees horizontal
        # and vertical from -1 to +1 and output is in normalized screen coordinates from -1 to 1
        
        fovratx = math.tan(0.5 * FOVx * d2r)
        fovraty = math.tan(0.5 * FOVy * d2r)
	# These are the angle limits that we WANT to see on screen at the borders of our screen.
        max_theta = math.atan(fovratx)*r2d
        max_psi = math.atan(fovraty)*r2d

        # print(max_theta,max_psi)
        
	# Figure out the ray angle we are trying to "look through" for this pixel
	# in our panorama projection - it's a straight LINEAR mapping of
	# screen space to degrees in BOTH angles!
        theta_rad_cylinder = Extrap(-1,-max_theta , 1, max_theta , thetaphi[0]) * d2r
        psi_tan_cylinder = Extrap(-1,-fovraty , 1, fovraty , thetaphi[1])

        # CYLINDRICAL RENDERING:
        psi_rad_cylinder = math.atan(psi_tan_cylinder)

        # SPHERICAL RENDERING
        # psi_rad_cylinder = Extrap(-1,-max_psi , 1, max_psi , thetaphi[1]) * d2r

        # print(theta_rad_cylinder,psi_rad_cylinder)
        
	# Now here's how we "back project" from panorama to planar projection: in planar projection
	# you can think of the X and Y coordinates on screen
        # (in normalized ratios from -fov-rat to fov-rat)
	# to be in "tangent" space.
	# So we can take our look vector, divide out Z (which is always the divisor
        # of the two right triangles
	# in eye space) and we get a pair of tangent ratios.
        vec = np.zeros(3)
        vec[0] = math.cos(psi_rad_cylinder) * math.sin(theta_rad_cylinder)
        vec[1] = math.sin(psi_rad_cylinder)
        vec[2] = math.cos(psi_rad_cylinder) * math.cos(theta_rad_cylinder)

        # print(vec)
        
	# This is the tangent ratios that match our desired look vector (which was computed from degrees
	# since the destination screen is mapped in degrees.)
	# Since a planar projection is distributed IN tangent ratios,
        # we can just do a linear lookup and we are done.
        xy = np.zeros(2)
        xy[0] = Extrap(-fovratx,-1 , fovratx,1 , vec[0]/vec[2])
        xy[1] = Extrap(-fovraty,-1 , fovraty,1 , vec[1]/vec[2]) * vert_stretch

        return xy

def PanoramaProj_forward(xy,FOVx,FOVy,vert_stretch):
        # Formulas gracefully provided by Austin Meyer of X-Plane
        # X-Plane Panorama Projection forward function: input is in normalized screen coordinates
        # from -1 to 1 and output normalized degrees horizontal and vertical from -1 to +1

        fovratx = math.tan(0.5 * FOVx * d2r)
        fovraty = math.tan(0.5 * FOVy * d2r)
	# These are the angle limits that we WANT to see on screen at the borders of our screen.
        max_theta = math.atan(fovratx)*r2d
        max_psi = math.atan(fovraty)*r2d

        # print(max_theta,max_psi)
        
	# Figure out the ray angle we are trying to "look through" for this pixel
	# in our panorama projection - it's a straight LINEAR mapping of
	# screen space to degrees in BOTH angles!
        x_tan_cylinder = Extrap(-1,-fovratx , 1, fovratx , xy[0])
        theta_rad_cylinder = math.atan(x_tan_cylinder)
        y_tan_cylinder = Extrap(-1,-fovraty , 1, fovraty , xy[1]/vert_stretch)
        psi_rad_cylinder = math.atan(y_tan_cylinder / math.sqrt(x_tan_cylinder*x_tan_cylinder + 1))

        # print(theta_rad_cylinder,psi_rad_cylinder)
        
	# Now here's how we "back project" from panorama to planar projection: in planar projection
	# you can think of the X and Y coordinates on screen
        # (in normalized ratios from -fov-rat to fov-rat)
	# to be in "tangent" space.
	# So we can take our look vector, divide out Z (which is always the divisor
        # of the two right triangles
	# in eye space) and we get a pair of tangent ratios.
        vec = np.zeros(3)
        vec[0] = math.cos(psi_rad_cylinder) * math.sin(theta_rad_cylinder)
        vec[1] = math.sin(psi_rad_cylinder)
        vec[2] = math.cos(psi_rad_cylinder) * math.cos(theta_rad_cylinder)

        # print(vec)
        
	# This is the tangent ratios that match our desired look vector (which was computed from degrees
	# since the destination screen is mapped in degrees.)
	# Since a planar projection is distributed IN tangent ratios,
        # we can just do a linear lookup and we are done.
        thetaphi = np.zeros(2)
        thetaphi[0] = Extrap(-max_theta,-1 , max_theta,1 , theta_rad_cylinder * r2d)

        # CYLINDRICAL RENDERING:
        thetaphi[1] = Extrap(-fovraty,-1 , fovraty,1 , math.tan(psi_rad_cylinder))

        # SPHERICAL RENDERING:
        # thetaphi[1] = Extrap(-max_psi,-1 , max_psi,1 , psi_rad_cylinder * r2d) 

        return thetaphi


# --------- START OF MAIN CODE ------------

# Parameters
d2r = math.pi/180.
r2d = 180./math.pi

# Generate X-Plane Window Preferences Output File and write header
con = open(outfile,"w")
con.write("I\n")
con.write("10 Version\n")
con.write("num_monitors "+str(nmon)+"\n")


# General Calculations independent of pixel position (cm or degrees)
ar =  float(nx) / float(ny)  # aspect ratio of projector image
beta = math.atan(1.0/(2.0*tr))*r2d # Maximum horizontal FOV from Projector [deg]
beta1 = 180.-beta   # well ... check out the drawing yourself
delta = math.asin(d_0/R*math.sin(beta1*d2r))*r2d  # same here
gamma = 180.-beta1-delta  # Maximum horizontal FOV from Screen Center [deg]
R_1 = R*math.sin(gamma*d2r)/math.sin(beta1*d2r) # Maximum Distance of Projector to screen edge
d_1 = R_1*math.cos(beta*d2r) # distance of projector to hypothetical planar screen in front of cylindrical screen
w_h = R_1*math.sin(beta*d2r) # half of hypothetical planar image width at screen distance

w = 2.0*w_h    # planar image width at screen distance (as if projection would be on planar scren)
h = w / ar     # planar image height at screen distance (as if projection ... )
h_h = 0.5 * h  # half height of hypothetical planar image

print("Flat Screen Dimension for all Projectors at distance d0+d1: ")
print(w,h)

# loop through monitors
for mon in range(0,nmon,1):

    print("------------------------")
    print("Monitor: "+str(mon))

    # calculate FOV of monitor
    FOVx = 2.0*gamma
            
    # calculate larger FOVy for planar to cylindrical transformation
    # need this also for the no projection file for X-Plane
    f = w_h / math.tan(0.5*FOVx*d2r)
    FOVy = 2.0*math.atan(h_h/f)*r2d
 
    print("FOVx:    "+str(FOVx))
    print("FOVy:    "+str(FOVy))
  
    # For Cylindrical Projection we have to expand image in the vertical
    # and increase vertical FOV:
    if cylindrical[mon]:
        in_bl = [ -0.001, -0.001 ]
        in_br = [  0.001, -0.001 ]
        in_tl = [ -0.001,  0.001 ]
        in_tr = [  0.001,  0.001 ]
        
        out_bl = PanoramaProj_inverse(in_bl,FOVx,FOVy,1.0);
        out_br = PanoramaProj_inverse(in_br,FOVx,FOVy,1.0);
        out_tl = PanoramaProj_inverse(in_tl,FOVx,FOVy,1.0);
        out_tr = PanoramaProj_inverse(in_tr,FOVx,FOVy,1.0);
        
        dx = (out_tr[0]-out_tl[0]) + (out_br[0]-out_bl[0]);
        dy = (out_tr[1]-out_br[1]) + (out_tl[1]-out_bl[1]);
        
        vert_stretch = dx / dy
        
        print("Vert Stretch to correct Flattening: ",vert_stretch)
        
        # Vert Stretch corrects for flatteing of the image due to the
        # cylindrical projection. 
        xy = np.zeros(2)
        xy[0] = 1.0
        xy[1] = 1.0
        thetaphi = PanoramaProj_forward(xy,FOVx,FOVy,1.0)
        #print(thetaphi)
        
        xy = np.zeros(2)
        xy[0] = 1.0
        xy[1] = 1.0
        thetaphi = PanoramaProj_forward(xy,FOVx,FOVy,vert_stretch)
        #print(thetaphi)

        # Now we want to increase FOV to fill the
        # whole screen at the edges after planar to cylindricl projection
        # so calculate a new FOV and stetch y axis even more        
        #vert_stretch_1 = vert_stretch * thetaphi[1]
        
        #print("Vert Stretch to fill Screen: ",vert_stretch_1)
        
        #FOVy_1 = 2.0 * math.atan(math.tan(0.5 * FOVy * d2r) * vert_stretch / vert_stretch_1) * r2d
        #FOVy = FOVy_1
        
        #vert_stretch = vert_stretch_1

        #print("New FOVy: ",FOVy)
           
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
#        for gx in range(0,1,1):
            
            # Calculate Pixel position of grid point (top-left is 0/0 and bottom-right is nx/ny)
            px = float(nx) * float(gx) / float(ngx-1)
            py = float(ny) * float(gy) / float(ngy-1)
            xabs[gx,gy] = px
            yabs[gx,gy] = py
            
            # 1. Transformation from planar to cylindrical rendering
            # --> This has nothing to do with the projector orientation and mount etc.
            # --> This is needed since X-Plane renders on a plane but we need a cylindrical rendering
            # Please see planar_to_cylindrical.pdf
            if cylindrical[mon]:

                # COMPLETELY REWORK CYLINDRICAL PROJECTION
                
                # Input is a planar image in x/y coordinates
                # Output is a cylindrical image in degrees and height
                
                xy = np.zeros(2)
                xy[0] = 2.0 * (px - 0.5 * float(nx)) / float(nx)
                xy[1] = 2.0 * (py - 0.5 * float(ny)) / float(ny) + frustum
                
                thetaphi = PanoramaProj_forward(xy,FOVx,FOVy,vert_stretch)
                
                #print(thetaphi)
                
                ex = 0.5 * thetaphi[0] * float(nx) + 0.5*float(nx)
                ey = 0.5 * (thetaphi[1] - frustum) * float(ny) + 0.5*float(ny)
                
                #print(px,py,ex,ey)
                
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
                b = h_1 + py / float(ny) * h
                   
                # Calculate horizontal view angle for pixel from projector
                theta = math.atan(a/d_1)*r2d
                # Calculate horizontal view angle for pixel from screen center
                # assuming input from projector is in degrees FOVx
                psi = a / w_h * gamma

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
    if (mon == 0) and (nmon > 1):
        con.write("monitor/"+str(mon)+"/m_usage wmgr_usage_panel_only"+"\n")
    else:
        con.write("monitor/"+str(mon)+"/m_usage wmgr_usage_normal_visuals"+"\n")
    if newformat:
        con.write("monitor/"+str(mon)+"/proj/proj_cylinder 0"+"\n")
        con.write("monitor/"+str(mon)+"/proj/proj_sphere 0"+"\n")        
    con.write("monitor/"+str(mon)+"/proj/window_2d_off 0"+"\n")
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
    if newformat:
        if gridtest:
            con.write("monitor/"+str(mon)+"/proj/grid_rat_config 1"+"\n")
        else:
            con.write("monitor/"+str(mon)+"/proj/grid_rat_config 0"+"\n")           
        if (projection[mon] or cylindrical[mon]) and savegrid:
            con.write("monitor/"+str(mon)+"/proj/grid_rat_render 1"+"\n")
        else:
            con.write("monitor/"+str(mon)+"/proj/grid_rat_render 0"+"\n")
        con.write("monitor/"+str(mon)+"/proj/grid_rat_drag_dim_i 4"+"\n")
        con.write("monitor/"+str(mon)+"/proj/grid_rat_drag_dim_j 4"+"\n")

    else:
        if gridtest:
            con.write("monitor/"+str(mon)+"/proj/grid_os_on_test 1"+"\n")
        else:
            con.write("monitor/"+str(mon)+"/proj/grid_os_on_test 0"+"\n")           
        if (projection[mon] or cylindrical[mon]) and savegrid:
            con.write("monitor/"+str(mon)+"/proj/grid_os_on_render 1"+"\n")
        else:
            con.write("monitor/"+str(mon)+"/proj/grid_os_on_render 0"+"\n")
        con.write("monitor/"+str(mon)+"/proj/grid_os_drag_dim_i 4"+"\n")
        con.write("monitor/"+str(mon)+"/proj/grid_os_drag_dim_j 4"+"\n")

    # write grid per monitor
    if newformat:
        for gx in range(0,ngx,1):
            for gy in range(0,ngy,1):
                con.write("monitor/"+str(mon)+"/proj/grid_ini_x"+str(gx)+"/"+str(gy)+" "
                          +str(format(xabs[gx,gy]/float(nx),('.6f')))+"\n")
        for gx in range(0,ngx,1):
            for gy in range(0,ngy,1):
                con.write("monitor/"+str(mon)+"/proj/grid_ini_y"+str(gx)+"/"+str(gy)+" "
                          +str(format(yabs[gx,gy]/float(ny),('.6f')))+"\n")
        for gx in range(0,ngx,1):
            for gy in range(0,ngy,1):
                con.write("monitor/"+str(mon)+"/proj/grid_off_x"+str(gx)+"/"+str(gy)+" "
                          +str(format(0.0,('.6f')))+"\n")
        for gx in range(0,ngx,1):
            for gy in range(0,ngy,1):
                con.write("monitor/"+str(mon)+"/proj/grid_off_y"+str(gx)+"/"+str(gy)+" "
                          +str(format(0.0,('.6f')))+"\n")
    else:
        if (projection[mon] or cylindrical[mon]) and savegrid:
            for gx in range(0,ngx,1):
                for gy in range(0,ngy,1):
                    con.write("monitor/"+str(mon)+"/proj/grid_os_x/"+str(gx)+"/"+str(gy)+" "+str(format(xdif[gx,gy],('.6f')))+"\n")
            for gx in range(0,ngx,1):
                for gy in range(0,ngy,1):
                    con.write("monitor/"+str(mon)+"/proj/grid_os_y/"+str(gx)+"/"+str(gy)+" "+str(format(ydif[gx,gy],('.6f')))+"\n")


    # write blending per monitor

    if newformat:
        con.write("monitor/"+str(mon)+"/proj/edge_blend_config 0"+"\n")
        if blending[mon]:
            con.write("monitor/"+str(mon)+"/proj/edge_blend_lft 1"+"\n")
            con.write("monitor/"+str(mon)+"/proj/edge_blend_rgt 1"+"\n")
            con.write("monitor/"+str(mon)+"/proj/edge_blend_bot 1"+"\n")
            con.write("monitor/"+str(mon)+"/proj/edge_blend_top 1"+"\n")
        else:
            con.write("monitor/"+str(mon)+"/proj/edge_blend_lft 0"+"\n")
            con.write("monitor/"+str(mon)+"/proj/edge_blend_rgt 0"+"\n")
            con.write("monitor/"+str(mon)+"/proj/edge_blend_bot 0"+"\n")
            con.write("monitor/"+str(mon)+"/proj/edge_blend_top 0"+"\n")

        con.write("monitor/"+str(mon)+"/proj/edge_blend_deg_lft -15.00000"+"\n")
        con.write("monitor/"+str(mon)+"/proj/edge_blend_deg_rgt 15.00000"+"\n")
        con.write("monitor/"+str(mon)+"/proj/edge_blend_deg_bot -15.00000"+"\n")
        con.write("monitor/"+str(mon)+"/proj/edge_blend_deg_top 15.00000"+"\n")

        con.write("monitor/"+str(mon)+"/proj/edge_blend_fade_lft 1.00000"+"\n")
        con.write("monitor/"+str(mon)+"/proj/edge_blend_fade_rgt 1.00000"+"\n")
        con.write("monitor/"+str(mon)+"/proj/edge_blend_fade_bot 1.00000"+"\n")
        con.write("monitor/"+str(mon)+"/proj/edge_blend_fade_top 1.00000"+"\n")
        
    else:
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
        if blendtest:
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/0 1.000000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/1 0.000000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/2 0.000000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/3 0.000000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/0 1.000000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/1 0.000000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/2 0.000000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/3 0.000000"+"\n")
        else:
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/0 1.000000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/1 0.800000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/2 0.450000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/3 0.000000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/0 1.000000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/1 0.800000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/2 0.450000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/3 0.000000"+"\n")

    if doplot:
        plot.figure(figsize=(16,10))
        plot.scatter(xabs+xdif,yabs+ydif,color="red",marker=".")
        plot.show()




# end loop of monitors

con.close()
