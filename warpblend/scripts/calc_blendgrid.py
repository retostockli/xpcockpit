import math
import numpy as np

def calc_blendimage_unwarped(nx, ny, left_top, left_bot, right_top, right_bot):
    # returns full blendimage (e.g. for NVIDIA driver) in unwarped display space
    # would only be applied if warp-after-blend is working in NVIDIA
    
    scale = 4.0  # sigmoid function scaling (ranges from - scale to scale)

    blendimage = np.zeros((nx, ny))

    for y in range(0,ny,1):
    #for y in range(0,1,1):
        xl = (y/(ny-1) * left_bot + (1.0-y/(ny-1)) * left_top)*(nx-1)
        xr = (y/(ny-1) * right_bot + (1.0-y/(ny-1)) * right_top)*(nx-1)

        if xl != 0.0:
            for x in range(0,int(xl),1):
                xs = (x - xl/2.0)/(xl/2.0) * scale
                ys = 1.0/(1+math.exp(-xs))
                blendimage[x,y] = ys
        if xr != 0.0:
            for x in range(0,int(xr),1):
                xs = (x - xl/2.0)/(xl/2.0) * scale
                ys = 1.0/(1+math.exp(-xs))
                blendimage[nx-1-x,y] = ys
                #print(nx-1-x,xs,ys)

        blendimage[int(xl):nx-int(xr),y] = 1.0    

    return blendimage

def calc_blendgrid_warped(nx, ny, ngx, ngy, xabs, xdif, yabs, ydif, blendimage):
    # returns blending grid in warped space. Currently simplified assumption
    # of warping is done.
    # safe for applying in XP12 natively (after version 12.10, when a blending image can be read)
    
    xwarp = xabs + xdif
    ywarp = yabs + ydif

    blendgrid = np.zeros((ngx, ngy))

    for gy in range(0,ngy,1):
    #for gy in range(0,1,1):
        for gx in range(0,ngx,1):

            if (xwarp[gx,gy] >= 0.0) and (xwarp[gx,gy] <= (nx-1)) and (ywarp[gx,gy] >= 0.0) and (ywarp[gx,gy] <= (ny-1)):
                blendgrid[gx,gy] = blendimage[int(xwarp[gx,gy]),int(ywarp[gx,gy])]
            else:
                blendgrid[gx,gy] = 0.0

            #print(gx,xwarp[gx,gy],ywarp[gx,gy],blendgrid[gx,gy])

    #blendgrid = np.flip(blendgrid,1)
                    
    return blendgrid

