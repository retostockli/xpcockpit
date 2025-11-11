import math
import numpy as np
from scipy.ndimage import map_coordinates

def calc_blendimage_unwarped(nx, ny, left_top, left_bot, right_top, right_bot, power):
    # returns full blendimage in unwarped display space
    # since the blend lines on left and right are drawn in the regular display coordinates
    # power: non-linear transformation of blend curve (>1.0: darker around midpoint, <1.0: brighter around midpoint)
    
    scale = 3.0  # sigmoid function scaling (ranges from - scale to scale)
    # since at -scale and scale the values are not at 0.0 / 1.0 we now apply a vertical scaling
    # around the midpoint (see below)
    minval = 1.0/(1+math.exp(scale))
    maxval = 1.0/(1+math.exp(-scale))

    blendimage = np.zeros((nx, ny))

    for y in range(0,ny,1):
#    for y in range(0,1,1):
        xl = (y/(ny-1) * left_bot + (1.0-y/(ny-1)) * left_top)*(nx-1)
        xr = (y/(ny-1) * right_bot + (1.0-y/(ny-1)) * right_top)*(nx-1)

        # modify blending edge if we have blending
        # unsolved issue with shifted blending edge in NVIDIA and possibly XP
        # Where does it come from? (TBD)
        if xl > 0.1: xl *= 1.0275
        if xr > 0.1: xr *= 1.0275
        
        if xl != 0.0:
            for x in range(0,int(xl),1):
                xs = (x - xl/2.0)/(xl/2.0) * scale
                ys = math.pow((1.0/(1+math.exp(-xs)) - 0.5)/(maxval-minval) + 0.5,power)
                blendimage[x,y] = ys
                #print(x,xs,ys)
        if xr != 0.0:
            for x in range(0,int(xr),1):
                xs = (x - xr/2.0)/(xr/2.0) * scale
                ys = math.pow((1.0/(1+math.exp(-xs)) - 0.5)/(maxval-minval) + 0.5,power)
                blendimage[nx-1-x,y] = ys
                #print(nx-1-x,xs,ys)

        blendimage[int(xl):nx-int(xr),y] = 1.0    

    return blendimage

def calc_blendgrid_warped(nx, ny, ngx, ngy, xabs, xdif, yabs, ydif, blendimage):
    # returns blending grid in warped space. 
    # Use if NVIDIA blend after warp is false or not working
    
    xwarp = xabs + xdif
    ywarp = yabs + ydif

    #miss = -99

    coords = np.array([xwarp,ywarp])

    # Interpolate
    #blendgrid = map_coordinates(blendimage, coords, order=1, mode = 'constant',cval = miss)  # order=1 = bilinear, constant: fill with 0.0
    blendgrid_ext = map_coordinates(blendimage, coords, order=1, mode = 'nearest')  # order=1 = bilinear, nearest: fill with nearest valid value

                    
    return blendgrid_ext

