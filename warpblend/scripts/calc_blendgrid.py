import math
import numpy as np
from scipy.ndimage import map_coordinates

def calc_blendimage_unwarped(nx, ny, left_top, left_bot, right_top, right_bot):
    # returns full blendimage in unwarped display space
    # since the blend lines on left and right are drawn in the regular display coordinates
    
    scale = 3.0  # sigmoid function scaling (ranges from - scale to scale)
    # since at -scale and scale the values are not at 0.0 / 1.0 we now apply a vertical scaling
    # around the midpoint (see below)
    minval = 1.0/(1+math.exp(scale))
    maxval = 1.0/(1+math.exp(-scale))

    blendimage = np.zeros((nx, ny))

    for y in range(0,ny,1):
    #for y in range(0,1,1):
        xl = (y/(ny-1) * left_bot + (1.0-y/(ny-1)) * left_top)*(nx-1)
        xr = (y/(ny-1) * right_bot + (1.0-y/(ny-1)) * right_top)*(nx-1)
        
        if xl != 0.0:
            for x in range(0,int(xl),1):
                xs = (x - xl/2.0)/(xl/2.0) * scale
                ys = (1.0/(1+math.exp(-xs)) - 0.5)/(maxval-minval) + 0.5
                blendimage[x,y] = ys
        if xr != 0.0:
            for x in range(0,int(xr),1):
                xs = (x - xr/2.0)/(xr/2.0) * scale
                ys = (1.0/(1+math.exp(-xs)) - 0.5)/(maxval-minval) + 0.5
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

    # now we may have some pixels that fall out of the screen on the left and the right after warping. So shrink blending distance if needed
    # for y in range(0,ngy,1):
    #     # only shrink if needed
    #     if blendgrid[0,y] == miss:
    #         x0 = miss
    #         x1 = miss
    #         for x in range(0,int(ngx/2),1):
            
    #             if (blendgrid[x,y] != miss) and (x0 == miss): x0 = x
    #             if (x0 != miss) and (blendgrid[x,y] == miss) and (x1 == miss): x1 = x-1
    #             if (x0 != miss) and (blendgrid[x,y] == 1.0) and (x1 == miss): x1 = x-1

    #         print(y,x0,x1,blendgrid_ext[0,y],blendgrid[x0,y],blendgrid[x1,y])

    # print(blendgrid[:,0])
    # print(blendgrid_ext[:,0])
    # print(blendgrid[:,ngy-1])  
    # print(blendgrid_ext[:,ngy-1])  

    #print(blendgrid[ngx-5:ngx-1,ngy-1])
                    
    return blendgrid_ext

