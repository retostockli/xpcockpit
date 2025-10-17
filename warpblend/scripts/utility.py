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


def create_blendimage_unwarped(nx, ny, blend_alpha, left_top,left_bot,right_top,right_bot):
		# returns full blendimage (e.g. for NVIDIA driver) in unwarped display space
		# would only be applied if warp-after-blend is working in NVIDIA
		
		blendimage = np.zeros((nx, ny))

		for y in range(0,ny,1):
				xl = (y/(ny-1) * left_bot + (1.0-y/(ny-1)) * left_top)*(nx-1)
				xr = (y/(ny-1) * right_bot + (1.0-y/(ny-1)) * right_top)*(nx-1)

				xval = [0.0,0.33*xl,0.66*xl,1.0*xl,nx-1-1.0*xr,nx-1-0.66*xr,nx-1-0.33*xr,nx-1]
				yval = [blend_alpha[0],blend_alpha[1],blend_alpha[2],blend_alpha[3],
								blend_alpha[3],blend_alpha[2],blend_alpha[1],blend_alpha[0]]

				# no blending if blend corner is at edge
				if (xl == 0.0):
						yval[0] = 1.0
						yval[1] = 1.0
						yval[2] = 1.0
						yval[3] = 1.0
				if (xr == 0.0):
						yval[4] = 1.0
						yval[5] = 1.0
						yval[6] = 1.0
						yval[7] = 1.0
				blendimage[:,y] = np.interp(range(0,nx),xval,yval)        

		return blendimage

def create_blendgrid_warped(nx, ny, ngx, ngy, xdif, blend_exp, left_top,left_bot,right_top,right_bot,xwarp,ywarp):
		# returns blending grid in warped space. Currently simplified assumption
		# of warping is done.
		# safe for applying in XP12 natively (after version 12.10, when a blending image can be read)
		
		blendgrid = np.zeros((ngx, ngy))

		for gy in range(0,ngy,1):
		#for gy in range(0,1,1):

				x0 = xdif[0,gy]
				x1 = xdif[ngx-1,gy]
				xl = (gy/(ngy-1) * left_bot + (1.0-gy/(ngy-1)) * left_top) * (nx-1)
				xr = (gy/(ngy-1) * right_bot + (1.0-gy/(ngy-1)) * right_top)*(nx-1)
				#print(str(xl)+" "+str(xr))
				for gx in range(0,ngx,1):

						if (gx < ngx/2):
								# left blending until center of image
								if (xl == 0.0):
										blendgrid[gx,gy] = 1.0
								else:
										blendgrid[gx,gy] = min((max(xwarp[gx,gy],0.0)/xl)**blend_exp,1.0)
						else:
								# right blending starting at center of image
								if (xr == 0.0):
										blendgrid[gx,gy] = 1.0
								else:
										blendgrid[gx,gy] = min((max(nx-xwarp[gx,gy],0.0)/xr)**blend_exp,1.0)
										
						#print(str(gx)+" "+str(xwarp[gx,gy])+" "+str(xl)+" "+str(xr)+" "+str(blendgrid[gx,gy]))

		blendgrid = np.flip(blendgrid,1)
						
		return blendgrid


