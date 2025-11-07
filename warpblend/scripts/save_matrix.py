import numpy as np
from scipy.ndimage import map_coordinates
import params
from utility import r2d, d2r
from calc_warpgrid import calc_warpgrid, calc_warppoint
from calc_blendgrid import calc_blendimage_unwarped, calc_blendgrid_warped
from calc_projector_screen import calc_fov

def save_xpfile():
	
    print("Saving to X-Plane PRF: "+params.xpfile)

    # Generate X-Plane Window Preferences Output File 
    con = open(params.xpfile,"w")
    con.write("I\n")
    con.write("10 Version\n")
    con.write("num_monitors "+str(params.nmon)+"\n")

    # loop through monitors
    for mon in range(0,params.nmon,1):

        print("------------------------")
        print("Monitor: "+str(mon))

		## reset projection grid
        xabs = np.zeros((params.ngx, params.ngy))
        yabs = np.zeros((params.ngx, params.ngy))
        xdif = np.zeros((params.ngx, params.ngy))
        ydif = np.zeros((params.ngx, params.ngy))

        if (params.cylindrical[mon] or params.projection[mon]):

            # For Screen Alignment Not all calculations are needed
            alignment = False

            xabs, yabs, xdif, ydif = calc_warpgrid(params.nx[mon], params.ny[mon], params.ngx, params.ngy, 
                                                    params.R, params.h_0, params.d_0, params.d_1, params.w_h, 
                                                    params.gamma, params.epsilon[mon], params.frustum,
                                                    params.vertical_scale[mon], params.vertical_shift[mon], 
                                                    params.cylindrical[mon], params.projection[mon], params.ceiling, alignment)

        # print(xdif[0,0])
        # print(ydif[0,0])
        # print(xdif[params.ngx-1,params.ngy-1])
        # print(ydif[params.ngx-1,params.ngy-1])

        FOVx, FOVy, h, w = calc_fov(params.nx[mon], params.ny[mon], params.w_h, params.gamma)

        # write header per monitor
        if mon == 0:
            con.write("monitor/"+str(mon)+"/m_window_idx 0"+"\n")
        else:
            con.write("monitor/"+str(mon)+"/m_window_idx -1"+"\n")

        if params.forwin:
            con.write("monitor/"+str(mon)+"/m_monitor "+str(mon)+"\n")
        else:
            con.write("monitor/"+str(mon)+"/m_monitor 0"+"\n")

        con.write("monitor/"+str(mon)+"/m_window_bounds/0 0"+"\n")
        con.write("monitor/"+str(mon)+"/m_window_bounds/1 0"+"\n")
        con.write("monitor/"+str(mon)+"/m_window_bounds/2 "+str(params.nx[mon])+"\n")
        con.write("monitor/"+str(mon)+"/m_window_bounds/3 "+str(params.ny[mon])+"\n")
        con.write("monitor/"+str(mon)+"/m_arbitrary_bounds/0 0"+"\n")
        con.write("monitor/"+str(mon)+"/m_arbitrary_bounds/1 0"+"\n")
        con.write("monitor/"+str(mon)+"/m_arbitrary_bounds/2 "+str(params.nx[mon])+"\n")
        con.write("monitor/"+str(mon)+"/m_arbitrary_bounds/3 "+str(params.ny[mon])+"\n")
        con.write("monitor/"+str(mon)+"/m_bpp 24"+"\n")
        con.write("monitor/"+str(mon)+"/m_refresh_rate 0"+"\n")
        con.write("monitor/"+str(mon)+"/m_x_res_full "+str(params.nx[mon])+"\n")
        con.write("monitor/"+str(mon)+"/m_y_res_full "+str(params.ny[mon])+"\n")
        con.write("monitor/"+str(mon)+"/m_is_fullscreen wmgr_mode_fullscreen"+"\n")
        if (mon == 0) and (params.nmon > 1):
            con.write("monitor/"+str(mon)+"/m_usage wmgr_usage_panel_only"+"\n")
        else:
            con.write("monitor/"+str(mon)+"/m_usage wmgr_usage_normal_visuals"+"\n")
        if params.xp12:
            con.write("monitor/"+str(mon)+"/proj/proj_cylinder 0"+"\n")
            con.write("monitor/"+str(mon)+"/proj/proj_sphere 0"+"\n")
        else:
            con.write("monitor/"+str(mon)+"/proj/window_2d_off 1"+"\n") 
        if params.cylindrical[mon]:
            con.write("monitor/"+str(mon)+"/proj/diff_FOV 1"+"\n")
        else:
            con.write("monitor/"+str(mon)+"/proj/diff_FOV 0"+"\n")    
        con.write("monitor/"+str(mon)+"/proj/FOVx_renopt "+str(format(FOVx,('.6f')))+"\n")
        con.write("monitor/"+str(mon)+"/proj/FOVy_renopt "+str(format(FOVy,('.6f')))+"\n")
        con.write("monitor/"+str(mon)+"/proj/os_x_rat 0.000000"+"\n")
        con.write("monitor/"+str(mon)+"/proj/os_y_rat 0.000000"+"\n")
        con.write("monitor/"+str(mon)+"/proj/off_vrt_deg "+str(format(params.vertical_offset[mon],('.6f')))+"\n")
        con.write("monitor/"+str(mon)+"/proj/off_lat_deg "+str(format(params.lateral_offset[mon],('.6f')))+"\n")
        con.write("monitor/"+str(mon)+"/proj/off_phi_deg 0.000000"+"\n")
        if params.xp12:
            if params.gridtest:
                con.write("monitor/"+str(mon)+"/proj/grid_rat_config 1"+"\n")
            else:
                con.write("monitor/"+str(mon)+"/proj/grid_rat_config 0"+"\n")           
            if (params.projection[mon] or params.cylindrical[mon]) and params.savegrid:
                con.write("monitor/"+str(mon)+"/proj/grid_rat_render 1"+"\n")
            else:
                con.write("monitor/"+str(mon)+"/proj/grid_rat_render 0"+"\n")
            con.write("monitor/"+str(mon)+"/proj/grid_opacity_pct 100.000000"+"\n")
            con.write("monitor/"+str(mon)+"/proj/grid_drag_dim_i "+str(params.dragx)+"\n")
            con.write("monitor/"+str(mon)+"/proj/grid_drag_dim_j "+str(params.dragy)+"\n")
            con.write("monitor/"+str(mon)+"/proj/grid_step_dim_i "+str(params.stepx)+"\n")
            con.write("monitor/"+str(mon)+"/proj/grid_step_dim_j "+str(params.stepy)+"\n")

        else:
            if params.gridtest:
                con.write("monitor/"+str(mon)+"/proj/grid_os_on_test 1"+"\n")
            else:
                con.write("monitor/"+str(mon)+"/proj/grid_os_on_test 0"+"\n")           
            if (params.projection[mon] or params.cylindrical[mon]) and params.savegrid:
                con.write("monitor/"+str(mon)+"/proj/grid_os_on_render 1"+"\n")
            else:
                con.write("monitor/"+str(mon)+"/proj/grid_os_on_render 0"+"\n")
            con.write("monitor/"+str(mon)+"/proj/grid_os_drag_dim_i "+str(params.dragx)+"\n")
            con.write("monitor/"+str(mon)+"/proj/grid_os_drag_dim_j "+str(params.dragy)+"\n")

        # write grid per monitor
        if params.xp12:
            if (params.projection[mon] or params.cylindrical[mon]) and params.savegrid:
                for gx in range(0,params.ngx,1):
                    for gy in range(0,params.ngy,1):
                        con.write("monitor/"+str(mon)+"/proj/grid_ini_x"+str(gx)+"/"+str(gy)+" "
                             +str(format((xabs[gx,gy]+xdif[gx,gy])/float(params.nx[mon]),('.6f')))+"\n")
                for gx in range(0,params.ngx,1):
                    for gy in range(0,params.ngy,1):
                        con.write("monitor/"+str(mon)+"/proj/grid_ini_y"+str(gx)+"/"+str(gy)+" "
                            +str(format((yabs[gx,gy]+ydif[gx,gy])/float(params.ny[mon]),('.6f')))+"\n")
                for gx in range(0,params.ngx,1):
                    for gy in range(0,params.ngy,1):
                        con.write("monitor/"+str(mon)+"/proj/grid_off_x"+str(gx)+"/"+str(gy)+" "
                            +str(format(0.0,('.6f')))+"\n")
                for gx in range(0,params.ngx,1):
                    for gy in range(0,params.ngy,1):
                        con.write("monitor/"+str(mon)+"/proj/grid_off_y"+str(gx)+"/"+str(gy)+" "
                            +str(format(0.0,('.6f')))+"\n")

                if params.blending[mon]:
                    # Generate XP12 Blending Grid in warped and not display space
                    blendimage = calc_blendimage_unwarped(params.nx[mon], params.ny[mon], 
                                        params.blend_left_top[mon],params.blend_left_bot[mon],
                                        params.blend_right_top[mon],params.blend_right_bot[mon])
                    blendgrid = calc_blendgrid_warped(params.nx[mon], params.ny[mon], params.ngx, params.ngy, 
                                                        xabs, xdif, yabs, ydif, blendimage)

                    if params.blendtest:
                        blendgrid[blendgrid<0.99] = 0.0

                    blendgrid = np.flip(blendgrid,1)
                    
                    for gx in range(0,params.ngx,1):
                        for gy in range(0,params.ngy,1):
                            con.write("monitor/"+str(mon)+"/proj/grid_mul_r"+str(gx)+"/"+str(gy)+" "
                                +str(format(blendgrid[gx,gy],('.6f')))+"\n")
                    for gx in range(0,params.ngx,1):
                        for gy in range(0,params.ngy,1):
                            con.write("monitor/"+str(mon)+"/proj/grid_mul_g"+str(gx)+"/"+str(gy)+" "
                                +str(format(blendgrid[gx,gy],('.6f')))+"\n")
                    for gx in range(0,params.ngx,1):
                        for gy in range(0,params.ngy,1):
                            con.write("monitor/"+str(mon)+"/proj/grid_mul_b"+str(gx)+"/"+str(gy)+" "
                                +str(format(blendgrid[gx,gy],('.6f')))+"\n")
        else:
            if (params.projection[mon] or params.cylindrical[mon]) and params.savegrid:
                for gx in range(0,params.ngx,1):
                    for gy in range(0,params.ngy,1):
                        con.write("monitor/"+str(mon)+"/proj/grid_os_x/"+str(gx)+"/"+str(gy)+" "+str(format(xdif[gx,gy],('.6f')))+"\n")
                for gx in range(0,params.ngx,1):
                    for gy in range(0,params.ngy,1):
                        con.write("monitor/"+str(mon)+"/proj/grid_os_y/"+str(gx)+"/"+str(gy)+" "+str(format(ydif[gx,gy],('.6f')))+"\n")


        # write blending per monitor

        if params.xp12:
            # Blending in XP12 is done via the RGB blending map (above grid)
            con.write("monitor/"+str(mon)+"/proj/edge_blend_config 0"+"\n")
            if params.blending[mon]:
                con.write("monitor/"+str(mon)+"/proj/edge_blend_lft 0"+"\n")
                con.write("monitor/"+str(mon)+"/proj/edge_blend_rgt 0"+"\n")
                con.write("monitor/"+str(mon)+"/proj/edge_blend_bot 0"+"\n")
                con.write("monitor/"+str(mon)+"/proj/edge_blend_top 0"+"\n")
            else:
                con.write("monitor/"+str(mon)+"/proj/edge_blend_lft 0"+"\n")
                con.write("monitor/"+str(mon)+"/proj/edge_blend_rgt 0"+"\n")
                con.write("monitor/"+str(mon)+"/proj/edge_blend_bot 0"+"\n")
                con.write("monitor/"+str(mon)+"/proj/edge_blend_top 0"+"\n")

            con.write("monitor/"+str(mon)+"/proj/edge_blend_deg_lft -15.000000"+"\n")
            con.write("monitor/"+str(mon)+"/proj/edge_blend_deg_rgt 15.000000"+"\n")
            con.write("monitor/"+str(mon)+"/proj/edge_blend_deg_bot -15.000000"+"\n")
            con.write("monitor/"+str(mon)+"/proj/edge_blend_deg_top 15.000000"+"\n")

            # We also save the actual blend distance values here for the NVIDIA blending
            # Note that these values cannot be used by X-Plane. It uses the RGB blend map
            #con.write("monitor/"+str(mon)+"/proj/edge_blend_fade_lft "+str(format(params.blend_left_top[mon],('.6f')))+"\n")
            #con.write("monitor/"+str(mon)+"/proj/edge_blend_fade_rgt "+str(format(params.blend_right_top[mon],('.6f')))+"\n")
            #con.write("monitor/"+str(mon)+"/proj/edge_blend_fade_bot "+str(format(params.blend_left_bot[mon],('.6f')))+"\n")
            #con.write("monitor/"+str(mon)+"/proj/edge_blend_fade_top "+str(format(params.blend_right_bot[mon],('.6f')))+"\n")
            con.write("monitor/"+str(mon)+"/proj/edge_blend_fade_lft 0.000000"+"\n")
            con.write("monitor/"+str(mon)+"/proj/edge_blend_fade_rgt 0.000000"+"\n")
            con.write("monitor/"+str(mon)+"/proj/edge_blend_fade_bot 0.000000"+"\n")
            con.write("monitor/"+str(mon)+"/proj/edge_blend_fade_top 0.000000"+"\n")
            
        else:
            if params.blending[mon]:
                con.write("monitor/"+str(mon)+"/proj/gradient_on 1"+"\n")
            else:
                con.write("monitor/"+str(mon)+"/proj/gradient_on 0"+"\n")

            con.write("monitor/"+str(mon)+"/proj/gradient_width_top/0 "+str(format(params.blend_left_top[mon]*params.nx[mon],('.6f')))+"\n")
            con.write("monitor/"+str(mon)+"/proj/gradient_width_top/1 "+str(format(params.blend_right_top[mon]*params.nx[mon],('.6f')))+"\n")
            con.write("monitor/"+str(mon)+"/proj/gradient_width_ctr/0 0.000000"+"\n")
            con.write("monitor/"+str(mon)+"/proj/gradient_width_ctr/1 0.000000"+"\n")
            con.write("monitor/"+str(mon)+"/proj/gradient_width_bot/0 "+str(format(params.blend_left_bot[mon]*params.nx[mon],('.6f')))+"\n")
            con.write("monitor/"+str(mon)+"/proj/gradient_width_bot/1 "+str(format(params.blend_right_bot[mon]*params.nx[mon],('.6f')))+"\n")
            if params.blendtest:
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/0 1.000000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/1 0.000000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/2 0.000000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/3 0.000000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/0 1.000000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/1 0.000000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/2 0.000000"+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/3 0.000000"+"\n")
            else:
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/0 "+str(format(params.blend_alpha[3],('.6f')))+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/1 "+str(format(params.blend_alpha[2],('.6f')))+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/2 "+str(format(params.blend_alpha[1],('.6f')))+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/0/3 "+str(format(params.blend_alpha[0],('.6f')))+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/0 "+str(format(params.blend_alpha[3],('.6f')))+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/1 "+str(format(params.blend_alpha[2],('.6f')))+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/2 "+str(format(params.blend_alpha[1],('.6f')))+"\n")
                con.write("monitor/"+str(mon)+"/proj/gradient_alpha/1/3 "+str(format(params.blend_alpha[0],('.6f')))+"\n")


    con.close()


def save_nvfile():

    # loop through monitors
    for mon in range(0,params.nmon,1):

        # save one file per monitor, only if reprojection was chosen
        if (params.cylindrical[mon] or params.projection[mon]):

            nvfile = params.nvfile+"_"+str(mon)+".dat"

            print("------------------------")
            print("Saving NVIDIA warp & blend grid for Monitor: "+str(mon))
            print("File: "+nvfile)

            # Generate X-Plane Window Preferences Output File 
            con = open(nvfile,"w")
            con.write(str(params.nx[mon])+"\n")
            con.write(str(params.ny[mon])+"\n")
            con.write(str(params.ngx)+"\n")
            con.write(str(params.ngy)+"\n")
            con.write("WARPGRID\n")

            # For Screen Alignment Not all calculations are needed
            alignment = False

            xabs, yabs, xdif, ydif = calc_warpgrid(params.nx[mon], params.ny[mon], params.ngx, params.ngy, 
                                                    params.R, params.h_0, params.d_0, params.d_1, params.w_h, 
                                                    params.gamma, params.epsilon[mon], params.frustum,
                                                    params.vertical_scale[mon], params.vertical_shift[mon], 
                                                    params.cylindrical[mon], params.projection[mon], params.ceiling, alignment)

            # print(xdif[0,0])
            # print(ydif[0,0])
            # print(xdif[params.ngx-1,params.ngy-1])
            # print(ydif[params.ngx-1,params.ngy-1])

            for gy in range(0,params.ngy,1):
                for gx in range(0,params.ngx,1):
                    con.write(str(format((xabs[gx,gy]+xdif[gx,gy])/float(params.nx[mon]),('.6f')))+" ")
                con.write("\n")
            for gy in range(0,params.ngy,1):
                for gx in range(0,params.ngx,1):
                    con.write(str(format((yabs[gx,gy]+ydif[gx,gy])/float(params.ny[mon]),('.6f')))+" ")
                con.write("\n")

            # Generate Blending Grid in warped and not display space
            # Blending grid for NVIDIA is full nx / ny image resolution, 
            xabs = np.zeros((params.nx[mon], params.ny[mon]))
            yabs = np.zeros((params.nx[mon], params.ny[mon]))
            for y in range(0,params.ny[mon],1):
                for x in range(0,params.nx[mon],1):
                    xabs[x,y] = x
                    yabs[x,y] = y

            coords = np.array([xabs/(params.nx[mon]-1)*(params.ngx-1),yabs/(params.ny[mon]-1)*(params.ngy-1)])

             # Interpolate
            xdif_new = map_coordinates(xdif, coords, order=1, mode = 'constant')  # order=1 = bilinear
            ydif_new = map_coordinates(ydif, coords, order=1, mode = 'constant')  # order=1 = bilinear
  
            blendimage = calc_blendimage_unwarped(params.nx[mon], params.ny[mon], 
                                params.blend_left_top[mon],params.blend_left_bot[mon],
                                params.blend_right_top[mon],params.blend_right_bot[mon])
            blendgrid = calc_blendgrid_warped(params.nx[mon], params.ny[mon], params.nx[mon], params.ny[mon], 
                                                xabs, xdif_new, yabs, ydif_new, blendimage)
            
            if params.blendtest:
                blendgrid[blendgrid<0.99] = 0.0
 
            con.write("BLENDGRID\n")
            for y in range(0,params.ny[mon],1):
                for x in range(0,params.nx[mon],1):
                    con.write(str(format(blendgrid[x,y],('.6f')))+" ")
                con.write("\n")

            con.close()
