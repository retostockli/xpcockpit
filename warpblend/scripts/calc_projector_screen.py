from utility import *

def calc_fov(nx, ny, w_h, gamma):
    
    ar =  float(nx) / float(ny)  # aspect ratio of projector image
    w = 2.0*w_h    # planar image width at screen distance (as if projection would be on planar scren)
    h = w / ar     # planar image height at screen distance (as if projection would be on planar screen )

    # calculate FOV of monitor
    FOVx = 2.0*gamma
                    
    # calculate larger FOVy for planar to cylindrical transformation
    # need this also for the no projection file for X-Plane
    f = w_h / math.tan(0.5*FOVx*d2r)
    FOVy = 2.0*math.atan(0.5*h/f)*r2d

    return FOVx, FOVy, h, w

def calc_projector_screen(px, py, nx, ny, R, h, h_0, d_0, d_1, w, w_h, gamma, epsilon):

    # INPUT IS IN CYLINDRICAL COORDINATES ALREADY
    # SO X IS in degrees and Y is in height
    
    # PROJECTOR CENTER Coordinate System
    
    # minimum elevation angle at h_0
    alpha_h = math.atan(h_0/d_1)*r2d 
    # new minimum height of projected tilted image onto vertical plane
    # equal h_0 without tilt epsilon
    h_1 = math.tan((alpha_h - epsilon)*d2r)*d_1 
    
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
    
    if epsilon != 0.0:
            # now calculate keystone correction: transform new coordinates on vertical
            # hypotetical planar screen to tilted hypothetical planar screen: intersect
            # projector ray with tilted plane
            #Define plane
            e = math.cos(epsilon*d2r)*d_1
            planeNormal = np.array([-e, 0.0, math.sin(epsilon*d2r)*d_1])
            planePoint = np.array([e+d_0, 0.0, -math.sin(epsilon*d2r)*d_1]) # Any point on the plane
            
            #Define ray
            rayDirection = np.array([d_1, a_1, b_1])
            rayPoint = np.array([d_0, 0.0, 0.0]) # Any point along the ray
            
            cp = LinePlaneCollision(planeNormal, planePoint, rayDirection, rayPoint)
            
            # transform intersection to tilted plane coordinates
            # Calculate distance difference of projector line from vertical hypothetical planar screen
            f = cp[0] - e - d_0
            
            a_2 = cp[1]
            b_2 = f / math.sin(epsilon*d2r)

            # print(a_2,b_2)
                                                    
            # Apply Cylindrical Warping Correction with tilted plane
            ex = a_2 / w * float(nx) + 0.5 * float(nx)
            ey = (b_2 - h_0) / h * float(ny)
            
    else:
            # Apply Cylindrical Warping Correction with vertical plane
            ex = a_1 / w * float(nx) + 0.5 * float(nx)
            ey = (b_1 - h_0) / h * float(ny)             

    return ex, ey