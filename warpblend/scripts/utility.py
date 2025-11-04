import math
import numpy as np

# Parameters
d2r = math.pi/180.
r2d = 180./math.pi

# Utility Functions

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
