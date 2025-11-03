from utility import *
from calc_vert_stretch import *
from calc_planar_to_cylindrical import *
from calc_projector_screen import *


# calculate a warped coordinate for a single point px/py
def calc_warppoint(nx, ny, px, py, R, h_0, d_0, d_1, w_h, gamma, epsilon, frustum, vertical_scale, vertical_shift, cylindrical, projection, ceiling, alignment):

    FOVx, FOVy, h, w = calc_fov(nx, ny, w_h, gamma)
			
    # For Cylindrical Projection we have to expand image in the vertical
    # and increase vertical FOV:
    if cylindrical:
        vert_stretch = calc_vert_stretch(FOVx,FOVy)
 
    # 1. Transformation from planar to cylindrical rendering
    # --> This has nothing to do with the projector orientation and mount etc.
    # --> This is needed since X-Plane renders on a plane but we need a cylindrical rendering
    # Please see planar_to_cylindrical.pdf
    # No planar-to-cylindrical adjustment for visual alignment of screen coordinates
    if cylindrical and not alignment:
        px, py = calc_planar_to_cylindrical(px, py, nx, ny, FOVx, FOVy, frustum, vert_stretch)

    # 2. Add vertical shift and scale if needed
    # This has to go after planar to cylindrical projection
    # since that projection works in original input coordinates
    # where horizon is centered in image
    if (vertical_scale != 0.0) or (vertical_shift != 0.0):
            py = py * vertical_scale + ny * vertical_shift
                                         
    # 3. Warping the planar projection of a regular table or ceiling mounted projector
    # onto a cylindrical screen. This is needed since the projector image is only ok
    # on a flat screen. We need to squeeze it onto a cylinder.
    # Please see projection_geometry.pdf
    if projection:
        px, py = calc_projector_screen(px, py, nx, ny, R, h, h_0, d_0, d_1, w, w_h, gamma, epsilon)
                
    return px, py

# calculate a warping grid for all control points
def calc_warpgrid(nx, ny, ngx, ngy, R, h_0, d_0, d_1, w_h, gamma, epsilon, frustum, vertical_scale, vertical_shift, cylindrical, projection, ceiling, alignment):

    FOVx, FOVy, h, w = calc_fov(nx, ny, w_h, gamma)
    
    print("Flat Screen Dimension at distance d0+d1: ")
    print("Horizontal: "+str(w)+" Vertical: "+str(h))

    print("FOVx:    "+str(FOVx))
    print("FOVy:    "+str(FOVy))
					 
    ## reset projection grid
    xabs = np.zeros((ngx, ngy))
    yabs = np.zeros((ngx, ngy))
    xdif = np.zeros((ngx, ngy))
    ydif = np.zeros((ngx, ngy))
		
    # For Cylindrical Projection we have to expand image in the vertical
    # and increase vertical FOV:
    if cylindrical:
        vert_stretch = calc_vert_stretch(FOVx,FOVy)
        print("Vert Stretch to correct Flattening: ",vert_stretch)

    # loop through grid
    # grid vertical: gy goes from bottom to top
    # grid horizontal: gx from left to right
    for gy in range(0,ngy,1):
        for gx in range(0,ngx,1):
            
            # Calculate Pixel position of grid point (top-left is 0/0 and bottom-right is nx/ny)
            # ARE WE SURE WE RANGE FROM 0 .. nx? OR RATHER 0 .. nx-1?
            px = float(nx) * float(gx) / float(ngx-1)
            py = float(ny) * float(gy) / float(ngy-1)
            xabs[gx,gy] = px
            yabs[gx,gy] = py
 
            # 1. Transformation from planar to cylindrical rendering
            # --> This has nothing to do with the projector orientation and mount etc.
            # --> This is needed since X-Plane renders on a plane but we need a cylindrical rendering
            # Please see planar_to_cylindrical.pdf
            # No planar-to-cylindrical adjustment for visual alignment of screen coordinates
            if cylindrical and not alignment:
                px, py = calc_planar_to_cylindrical(px, py, nx, ny, FOVx, FOVy, frustum, vert_stretch)

            # 2. Add vertical shift and scale if needed
            # This has to go after planar to cylindrical projection
            # since that projection works in original input coordinates
            # where horizon is centered in image
            if (vertical_scale != 0.0) or (vertical_shift != 0.0):
                    py = py * vertical_scale + ny * vertical_shift
                                                    
            # 3. Warping the planar projection of a regular table or ceiling mounted projector
            # onto a cylindrical screen. This is needed since the projector image is only ok
            # on a flat screen. We need to squeeze it onto a cylinder.
            # Please see projection_geometry.pdf
            if projection:
                px, py = calc_projector_screen(px, py, nx, ny, R, h, h_0, d_0, d_1, w, w_h, gamma, epsilon)

            xdif[gx,gy] = px - xabs[gx,gy]
            ydif[gx,gy] = py - yabs[gx,gy]
 		
    # inverse x and y array for ceiling mount
    # not needed for visual alignment of screen coordinates
    if ceiling and not alignment:
        ydif = -np.flip(ydif,axis=1)
        xdif = np.flip(xdif,axis=1)
                
    return xabs, yabs, xdif, ydif
