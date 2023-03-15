## Parameters
d2r = pi/180.
r2d = 180./pi

## Dimensions (in cm)
R = 169.5   # Screen Radius
d0 = 27.0   # Distance of Projector lens from center of cylinder (positive is towards screen)
tr = 0.49   # Projector Throw ratio
h0 = 15.0   # lower height of image above center of lens

nmon = 1  # number of monitors

## pixel dimensions of projector
nx = 1920.0
ny = 1080.0

## number of x and y grid points
ngx = 100
ngy = 100
    
## General Calculations independent of pixel position
ar = nx / ny  # aspect ratio of projector image
beta = atan(1.0/(2.0*tr))*r2d
beta1 = 180.-beta
delta = asin(d0/R*sin(beta1*d2r))*r2d
gamma = 180.-beta1-delta
R1 = R*sin(gamma*d2r)/sin(beta1*d2r)
d1 = R1*cos(beta*d2r)
w2 = R1*sin(beta*d2r)
w = 2*w2    # planar image width at screen distance
h = w / ar  # planar image height at screen distance

print(paste(w,h))
browser()


## set up plot
#plot(1, type = "n",                         # Remove all elements of plot
#     xlab = "", ylab = "",
#     xlim = c(0, nx), ylim = c(0, ny))

xdif = array(NA,dim=c(ngx,ngy))
ydif = array(NA,dim=c(ngx,ngy))

## loop through grid
for (mon in (0:(nmon-1))) {
for (gy in (1:ngy)) {
for (gx in (1:ngx)) {

    px = nx * (gx-1) / (ngx-1)
    py = ny * (gy-1) / (ngy-1)
    
    ## Calculate horizontal position on hypothetical
    ## planar screen in distance d1 from projector
    ## and respective horizontal projection angle
    a = w * (px - (nx-1.0)/2.0) / (nx-1.0)
    theta = atan(a/d1)*r2d
    
    ## Calculate intersection of projected line with the cylindrical screen
    ## behind hypothetical planar screen. Cylinder is in (0,0) position
    ## https://mathworld.wolfram.com/Circle-LineIntersection.html
    x_1 = d0
    x_2 = d1+d0
    y_1 = 0
    y_2 = a
    d_x = x_2-x_1
    d_y = y_2-y_1
    d_r = sqrt(d_x^2+d_y^2)
    D   = x_1*y_2-x_2*y_1
    DSCR = R^2*d_r^2 - D^2  # Discriminant: if > 0 (true for our problem: intersection)
    
    if (d_y < 0) {
        sgn_dy = -1.0
    } else {
        sgn_dy = 1.0
    }
    x_c = (D*d_y + sgn_dy + d_x * sqrt(DSCR))/d_r^2
    y_c = (-D*d_x + d_y * sqrt(DSCR))/d_r^2
    
    ## now calculate how much we need to shift pixel coordinates to match
    ## cylindrical screen again
    d_2 = sqrt((x_c-x_2)^2 + (y_c-y_2)^2)  # overshoot distance (should be 0 at screen edges)
    d_d = d_r + d_2 # total horizontal distance from projector to cylindrical screen
    
    z_2 = h0 + py/(ny-1) * h
    alpha = atan(z_2/d_r)*r2d
    z_c = tan(alpha*d2r) * d_d
    phi = atan(z_2/d_d)*r2d
    z_e = tan(phi*d2r) * d_r
    
    ey  = (z_e - h0) / h * (ny-1)
    ex  = px

    ##print(paste(ex,ey))
    ##points(ex,ey)
    ##print(paste(px,py))

    xdif[gx,gy] = ex - px
    ydif[gx,gy] = ey - py
}
}
sink("grid.txt")
for (gy in (1:ngy)) {
    for (gx in (1:ngx)) {
        cat(paste("monitor/",mon,"/proj/grid_os_x/",gx-1,"/",gy-1," ",
                  formatC(xdif[gx,gy],digits=6,format = "f"),"\n",sep=""))
    }
}
for (gy in (1:ngy)) {
    for (gx in (1:ngx)) {
        cat(paste("monitor/",mon,"/proj/grid_os_y/",gx-1,"/",gy-1," ",
                  formatC(ydif[gx,gy],digits=6,format = "f"),"\n",sep=""))
    }
}
sink()

}



#browser()
