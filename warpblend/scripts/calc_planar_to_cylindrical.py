from utility import *

def calc_planar_to_cylindrical(px, py, nx, ny, FOVx, FOVy, frustum, vert_stretch):

    # Input is a planar image in x/y coordinates
    # Output is a cylindrical image in degrees and height
    
    xy = np.zeros(2)
    xy[0] = 2.0 * (px - 0.5 * float(nx)) / float(nx)
    xy[1] = 2.0 * (py - 0.5 * float(ny)) / float(ny) + frustum
    
    thetaphi = PanoramaProj_forward(xy,FOVx,FOVy,vert_stretch)
    
    #print(thetaphi)
    
    ex = 0.5 * thetaphi[0] * float(nx) + 0.5*float(nx)
    ey = 0.5 * (thetaphi[1] - frustum) * float(ny) + 0.5*float(ny)
    
    return ex, ey