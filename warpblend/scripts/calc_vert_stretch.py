from utility import *

# For Cylindrical Projection we have to expand image in the vertical
# and increase vertical FOV:
def calc_vert_stretch(FOVx, FOVy):
    
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

    # FOV adjustment CURRENTLY TURNED OFF

    # Now we want to increase FOV to fill the
    # whole screen at the edges after planar to cylindricl projection
    # so calculate a new FOV and stetch y axis even more        
    #vert_stretch_1 = vert_stretch * thetaphi[1]
    
    #print("Vert Stretch to fill Screen: ",vert_stretch_1)
    
    #FOVy_1 = 2.0 * math.atan(math.tan(0.5 * FOVy * d2r) * vert_stretch / vert_stretch_1) * r2d
    #FOVy = FOVy_1
    
    #vert_stretch = vert_stretch_1

    #print("New FOVy: ",FOVy)

    return vert_stretch
