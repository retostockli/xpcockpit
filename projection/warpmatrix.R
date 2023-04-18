## GTX970 Output Numbering
## DVI0 (DVI-I-1): 0
## HDMI/HP (DP-1): 1
## DVI1 (DVI-D-0): 2
## HDMI (HDMI-0): 3

## X-Plane 11: need to activate VULKAN in Graphics Settings
## However: With Vulkan, Cylindrical projection has limited rendering in corners
## To have scenery only view: default_view 1 in X-Plane.prf
## To have cylindrical projection: proj_cyl 1 in X-Plane.prf

## Graphics
do.plot = FALSE

## Parameters
d2r = pi/180.
r2d = 180./pi

## Dimensions (in cm)
R = 169.0   # Screen Radius
d_0 = 30.0   # Distance of Projector focal point from center of cylinder (positive is towards screen)
## Projector focal point may be behind projector lens. For me it is around 2-4 cm behind
## as it seems. No documentation found.
tr = 0.49   # Projector Throw ratio
h_0 = 15.0   # lower height of image above center of lens when projected on planar screen from untilted projector

nmon = 1  # number of monitors

projection = c(TRUE,TRUE,TRUE,TRUE)
blending = c(FALSE,TRUE,TRUE,TRUE)
epsilon = c(0.0,0.0,5.75,0.0)
lateral_offset = c(0.0,-65.0,0.0,65.0)
vertical_offset = c(0.0,0.0,0.0,0.0)
gridtest = TRUE # display grid test pattern
forwin = FALSE  # create for windows or for linux


outfile = "X-Plane Window Positions.prf"

## pixel dimensions of projector
nx = 1920.0
ny = 1080.0

## number of x and y grid points
ngx = 101
ngy = 101

## Generate Output File and write header
con <- file(outfile)
open(con,open="w")
writeLines("I",con)
writeLines("10 Version",con)
writeLines(paste("num_monitors",nmon),con)


## General Calculations independent of pixel position
ar = nx / ny  # aspect ratio of projector image
beta = atan(1.0/(2.0*tr))*r2d
beta1 = 180.-beta
delta = asin(d_0/R*sin(beta1*d2r))*r2d
gamma = 180.-beta1-delta
R_1 = R*sin(gamma*d2r)/sin(beta1*d2r)
d_1 = R_1*cos(beta*d2r)
w_2 = R_1*sin(beta*d2r)
w = 2*w_2    # planar image width at screen distance
h = w / ar  # planar image height at screen distance


## set up plot
if (do.plot) {
    plot(1, type = "n",                         # Remove all elements of plot
         xlab = "", ylab = "",
         xlim = c(0, nx), ylim = c(0, ny))
}

## reset projection grid
xdif = array(0.0,dim=c(ngx,ngy))
ydif = array(0.0,dim=c(ngx,ngy))

## loop through monitors
for (mon in (0:(nmon-1))) {

    if (projection[mon+1]) {
        ## loop through grid
        for (gy in (1:ngy)) {
            for (gx in (1:ngx)) {

                ## Calculate Pixel position
                px = nx * (gx-1) / (ngx-1)
                py = ny * (gy-1) / (ngy-1)

                
                ## Calculate horizontal position on hypothetical
                ## planar screen in distance d_1 from projector
                ## and respective horizontal projection angle
                a = w * (px - (nx-1.0)/2.0) / (nx-1.0)
                theta = atan(a/d_1)*r2d
                
                ## Calculate intersection of projected line with the cylindrical screen
                ## behind hypothetical planar screen. Cylinder is in (0,0) position
                ## https://mathworld.wolfram.com/Circle-LineIntersection.html
                x_1 = d_0
                x_2 = d_1+d_0
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

                ## Calculate Elevation angle of pixel
                z_2 = h_0 + py/(ny-1) * h
                alpha = atan(z_2/d_r)*r2d
                
                z_c = tan(alpha*d2r) * d_d
                phi = atan(z_2/d_d)*r2d
                z_e = tan(phi*d2r) * d_r

                ex = px
                ey = py
                
                ## Apply Keystone Correction
                ## image gets compressed in both horizontal and vertical dimensions with projector tilt
                ## so uncompress and untrapezoid the image pixel right in the first place and place them
                ## where they would be with a untilted projector
                if (epsilon[mon+1] != 0.0) {
                    alpha_0 = atan(z_2/d_1)*r2d  # elevation angle at center line of projection
                    u = d_1 / cos(alpha_0*d2r)   # image distance at line py when projector would be level
                    u_k = d_1 / cos((alpha_0-epsilon[mon+1])*d2r)  # image distance at line py with tilt

                    ex = (ex-nx/2.0)*u/u_k + nx/2.0  # correct keystone in horizontal pixels
                    ey = ey * u / u_k  # correct keystone in vertical pixels
                }

                ## Apply Cylindrical Warping Correction
                ex = ex
                ey = (z_e - h_0) / h * (ny-1) - py + ey
                
                ##print(paste(ex,ey))
                if (do.plot) {
                    points(ex,ey)
                }
                ##print(paste(px,py))

                xdif[gx,gy] = ex - px
                ydif[gx,gy] = ey - py
            }
        }

    }

    ## write header per monitor
    if (mon == 0) {
        writeLines(paste("monitor/",mon,"/m_window_idx 0",sep=""),con)
    } else {
        writeLines(paste("monitor/",mon,"/m_window_idx -1",sep=""),con)
    }
    if (forwin) {
        writeLines(paste("monitor/",mon,"/m_monitor ",mon,sep=""),con)
    } else {
        writeLines(paste("monitor/",mon,"/m_monitor 0",sep=""),con)
    }
    writeLines(paste("monitor/",mon,"/m_window_bounds/0 0",sep=""),con)
    writeLines(paste("monitor/",mon,"/m_window_bounds/1 0",sep=""),con)
    writeLines(paste("monitor/",mon,"/m_window_bounds/2 ",nx,sep=""),con)
    writeLines(paste("monitor/",mon,"/m_window_bounds/3 ",ny,sep=""),con)
    writeLines(paste("monitor/",mon,"/m_arbitrary_bounds/0 0",sep=""),con)
    writeLines(paste("monitor/",mon,"/m_arbitrary_bounds/1 0",sep=""),con)
    writeLines(paste("monitor/",mon,"/m_arbitrary_bounds/2 ",nx,sep=""),con)
    writeLines(paste("monitor/",mon,"/m_arbitrary_bounds/3 ",ny,sep=""),con)
    writeLines(paste("monitor/",mon,"/m_bpp 24",sep=""),con)
    writeLines(paste("monitor/",mon,"/m_refresh_rate 0",sep=""),con)
    writeLines(paste("monitor/",mon,"/m_x_res_full ",nx,sep=""),con)
    writeLines(paste("monitor/",mon,"/m_y_res_full ",ny,sep=""),con)
    writeLines(paste("monitor/",mon,"/m_is_fullscreen wmgr_mode_fullscreen",sep=""),con)
    writeLines(paste("monitor/",mon,"/m_usage wmgr_usage_normal_visuals",sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/window_2d_off 1",sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/diff_FOV 0",sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/FOVx_renopt ",formatC(gamma*2,digits=6,format = "f"),sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/FOVy_renopt ",formatC(gamma*2*9/16,digits=6,format = "f"),sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/os_x_rat 0.000000",sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/os_y_rat 0.000000",sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/off_vrt_deg ",formatC(vertical_offset[mon+1],digits=6,format = "f"),sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/off_lat_deg ",formatC(lateral_offset[mon+1],digits=6,format = "f"),sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/off_phi_deg 0.000000",sep=""),con)
    if (gridtest) {
        writeLines(paste("monitor/",mon,"/proj/grid_os_on_test 1",sep=""),con)
    } else {
        writeLines(paste("monitor/",mon,"/proj/grid_os_on_test 0",sep=""),con)
    }
    if (projection[mon+1]) {
        writeLines(paste("monitor/",mon,"/proj/grid_os_on_render 1",sep=""),con)
    } else {
        writeLines(paste("monitor/",mon,"/proj/grid_os_on_render 0",sep=""),con)
    }
    writeLines(paste("monitor/",mon,"/proj/grid_os_drag_dim_i 4",sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/grid_os_drag_dim_j 4",sep=""),con)

    ## write grid per monitor
    if (projection[mon+1]) {
        for (gx in (1:ngx)) {
            for (gy in (1:ngy)) {
                writeLines(paste("monitor/",mon,"/proj/grid_os_x/",gx-1,"/",gy-1," ",
                                 formatC(xdif[gx,gy],digits=6,format = "f"),sep=""),con)
            }
        }
        for (gx in (1:ngx)) {
            for (gy in (1:ngy)) {
                writeLines(paste("monitor/",mon,"/proj/grid_os_y/",gx-1,"/",gy-1," ",
                                 formatC(ydif[gx,gy],digits=6,format = "f"),sep=""),con)
            }
        }
    }

    ## write blending per monitor
    if (blending[mon+1]) {
        writeLines(paste("monitor/",mon,"/proj/gradient_on 1",sep=""),con)
    } else {
        writeLines(paste("monitor/",mon,"/proj/gradient_on 0",sep=""),con)
    }
        
    writeLines(paste("monitor/",mon,"/proj/gradient_width_top/0 0.000000",sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/gradient_width_top/1 0.000000",sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/gradient_width_ctr/0 0.000000",sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/gradient_width_ctr/1 0.000000",sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/gradient_width_bot/0 0.000000",sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/gradient_width_bot/1 0.000000",sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/gradient_alpha/0/0 1.000000",sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/gradient_alpha/0/1 0.650000",sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/gradient_alpha/0/2 0.330000",sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/gradient_alpha/0/3 0.000000",sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/gradient_alpha/1/0 1.000000",sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/gradient_alpha/1/1 0.650000",sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/gradient_alpha/1/2 0.330000",sep=""),con)
    writeLines(paste("monitor/",mon,"/proj/gradient_alpha/1/3 0.000000",sep=""),con)
    
}

close(con)


                                        #browser()
