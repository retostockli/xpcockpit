from tkinter import *
from calc_projector_screen import *
import params
import warpblend_window
from read_ini import save_ini
from save_matrix import save_xpfile, save_nvfile

root = Tk()

def quit_root_window():
   root.destroy()

def create_root_window():

    root.geometry('{}x{}'.format(params.ROOT_NX, params.ROOT_NY))
    root.title("Main Window")

    # Recalculate Warp / Blend grids
    def recalc_grid():
        params.R = float(R_entry.get())
        params.d_0 = float(d_0_entry.get())
        params.h_0 = float(h_0_entry.get())
        params.tr = float(tr_entry.get())
        params.blendtest = blend_test.get()
        for mon in range(0,params.nmon,1):
            params.epsilon[mon] = float(epsilon_entry[mon].get())
            params.lateral_offset[mon] = float(lateral_offset_entry[mon].get())
            params.vertical_offset[mon] = float(vertical_offset_entry[mon].get())
            params.vertical_shift[mon] = float(vertical_shift_entry[mon].get())
            params.vertical_scale[mon] = float(vertical_scale_entry[mon].get())
            params.blend_left_top[mon] = float(blend_left_top_entry[mon].get())
            params.blend_left_bot[mon] = float(blend_left_bot_entry[mon].get())
            params.blend_right_top[mon] = float(blend_right_top_entry[mon].get())
            params.blend_right_bot[mon] = float(blend_right_bot_entry[mon].get())
            warpblend_window.redraw_warpblend_window(mon)

    # Save Configuration file
    def get_inifilename():
        params.inifile = inifile_entry.get()
        save_ini()

    # Save X-Plane PRF file
    def get_xpfilename():
        params.xpfile = xpfile_entry.get()
        save_xpfile()

    # Save NVIDIA Warp and Blend Grid file
    def get_nvfilename():
        params.nvfile = nvfile_entry.get()
        save_nvfile()


    # Create a Recalc Button
    recalc_button = Button(root, text="Recalc", command=recalc_grid)
    recalc_button.grid(row=0, column=0, padx = 5, pady=5, sticky ="n")

    # Create a Save Config Button
    save_config_button = Button(root, text="Save INI", command=get_inifilename)
    save_config_button.grid(row=0, column=1, padx = 5, pady=5, sticky ="n")

    # Create a Save X-Plane Button
    save_xplane_button = Button(root, text="Save X-Plane", command=get_xpfilename)
    save_xplane_button.grid(row=0, column=2, padx = 5, pady=5, sticky ="n")

    # Create a Save NVIDIA Button
    save_nvidia_button = Button(root, text="Save NVIDIA", command=get_nvfilename)
    save_nvidia_button.grid(row=0, column=3, padx = 5, pady=5, sticky ="n")

    # Create a Quit Button
    quit_button=Button(root,text="Quit", command= quit_root_window)
    quit_button.grid(row=0, column=4, padx=5, pady=5, sticky="n")

    # Create labels
    geometry_label = Label(root, text="PROJECTOR GEOMETRY")
    geometry_label.grid(row=1, column=0, padx=5, pady=5, sticky="e")
    R_label = Label(root, text="R [cm] ")
    R_label.grid(row=2, column=0, padx=5, pady=5, sticky="e")
    d_0_label = Label(root, text="d0 [cm] ")
    d_0_label.grid(row=3, column=0, padx=5, pady=5, sticky="e")
    h_0_label = Label(root, text="h0 [cm] ")
    h_0_label.grid(row=4, column=0, padx=5, pady=5, sticky="e")
    tr_label = Label(root, text="tr [-] ")
    tr_label.grid(row=5, column=0, padx=5, pady=5, sticky="e")

    inifile_label = Label(root, text="INI File: ")
    inifile_label.grid(row=2, column=2, padx=5, pady=5, sticky="e")

    xpfile_label = Label(root, text="X-Plane File: ")
    xpfile_label.grid(row=3, column=2, padx=5, pady=5, sticky="e")

    nvfile_label = Label(root, text="NV Warp&Blend File: ")
    nvfile_label.grid(row=4, column=2, padx=5, pady=5, sticky="e")

    row0 = 6
    monitor_label = [None] * (params.nmon + 1)
    monitor_label[0] = Label(root, text="IMAGE PARAMETERS")
    monitor_label[0].grid(row=row0, column=0, padx=5, pady=5, sticky="e")
    for mon in range(0,params.nmon,1):
        monitor_label[mon+1] = Label(root, text=str(mon))
        monitor_label[mon+1].grid(row=row0, column=1+mon, padx=5, pady=5, sticky="e")
    epsilon_label = Label(root, text="epsilon [deg] ")
    epsilon_label.grid(row=row0+1, column=0, padx=5, pady=5, sticky="e")
    lateral_offset_label = Label(root, text="lateral offset [deg] ")
    lateral_offset_label.grid(row=row0+2, column=0, padx=5, pady=5, sticky="e")
    vertical_offset_label = Label(root, text="vertical offset [deg] ")
    vertical_offset_label.grid(row=row0+3, column=0, padx=5, pady=5, sticky="e")
    vertical_shift_label = Label(root, text="vertical shift [-] ")
    vertical_shift_label.grid(row=row0+4, column=0, padx=5, pady=5, sticky="e")
    vertical_scale_label = Label(root, text="vertical scale [-] ")
    vertical_scale_label.grid(row=row0+5, column=0, padx=5, pady=5, sticky="e")
    
    blend_label = Label(root, text="IMAGE BLENDING ")
    blend_label.grid(row=row0+6, column=0, padx=5, pady=5, sticky="e")
    blend_left_top_label = Label(root, text="left top [-] ")
    blend_left_top_label.grid(row=row0+7, column=0, padx=5, pady=5, sticky="e")
    blend_left_bot_label = Label(root, text="left bottom [-] ")
    blend_left_bot_label.grid(row=row0+8, column=0, padx=5, pady=5, sticky="e")
    blend_right_top_label = Label(root, text="right top [-] ")
    blend_right_top_label.grid(row=row0+9, column=0, padx=5, pady=5, sticky="e")
    blend_right_bot_label = Label(root, text="right bottom [-] ")
    blend_right_bot_label.grid(row=row0+10, column=0, padx=5, pady=5, sticky="e")

    # Create entry fields
    R_entry = Entry(root, width=10)
    R_entry.grid(row=2, column=1, padx=5, pady=5)
    R_entry.insert(0,params.R)
    d_0_entry = Entry(root, width=10)
    d_0_entry.grid(row=3, column=1, padx=5, pady=5)
    d_0_entry.insert(0,params.d_0)
    h_0_entry = Entry(root, width=10)
    h_0_entry.insert(0,params.h_0)
    h_0_entry.grid(row=4, column=1, padx=5, pady=5)
    tr_entry = Entry(root, width=10)
    tr_entry.insert(0,params.tr)
    tr_entry.grid(row=5, column=1, padx=5, pady=5)

    inifile_entry = Entry(root, width=30)
    inifile_entry.grid(row=2, column=3, columnspan = 3, padx=5, pady=5)
    inifile_entry.insert(0,params.inifile)

    xpfile_entry = Entry(root, width=30)
    xpfile_entry.grid(row=3, column=3, columnspan = 3, padx=5, pady=5)
    xpfile_entry.insert(0,params.xpfile)
 
    nvfile_entry = Entry(root, width=30)
    nvfile_entry.grid(row=4, column=3, columnspan = 3, padx=5, pady=5)
    nvfile_entry.insert(0,params.nvfile)

    row0 = 6
    epsilon_entry = [None] * params.nmon
    lateral_offset_entry = [None] * params.nmon
    vertical_offset_entry = [None] * params.nmon
    vertical_shift_entry = [None] * params.nmon
    vertical_scale_entry = [None] * params.nmon
    blend_left_top_entry = [None] * params.nmon
    blend_left_bot_entry = [None] * params.nmon
    blend_right_top_entry = [None] * params.nmon
    blend_right_bot_entry = [None] * params.nmon
    for mon in range(0,params.nmon,1):
        epsilon_entry[mon] = Entry(root, width=10)
        epsilon_entry[mon].insert(0,params.epsilon[mon])
        epsilon_entry[mon].grid(row=row0+1, column=1+mon, padx=5, pady=5)
        lateral_offset_entry[mon] = Entry(root, width=10)
        lateral_offset_entry[mon].insert(0,params.lateral_offset[mon])
        lateral_offset_entry[mon].grid(row=row0+2, column=1+mon, padx=5, pady=5)
        vertical_offset_entry[mon] = Entry(root, width=10)
        vertical_offset_entry[mon].insert(0,params.vertical_offset[mon])
        vertical_offset_entry[mon].grid(row=row0+3, column=1+mon, padx=5, pady=5)
        vertical_shift_entry[mon] = Entry(root, width=10)
        vertical_shift_entry[mon].insert(0,params.vertical_shift[mon])
        vertical_shift_entry[mon].grid(row=row0+4, column=1+mon, padx=5, pady=5)
        vertical_scale_entry[mon] = Entry(root, width=10)
        vertical_scale_entry[mon].insert(0,params.vertical_scale[mon])
        vertical_scale_entry[mon].grid(row=row0+5, column=1+mon, padx=5, pady=5)
        blend_left_top_entry[mon] = Entry(root, width=10)
        blend_left_top_entry[mon].insert(0,params.blend_left_top[mon])
        blend_left_top_entry[mon].grid(row=row0+7, column=1+mon, padx=5, pady=5)
        blend_left_bot_entry[mon] = Entry(root, width=10)
        blend_left_bot_entry[mon].insert(0,params.blend_left_bot[mon])
        blend_left_bot_entry[mon].grid(row=row0+8, column=1+mon, padx=5, pady=5)
        blend_right_top_entry[mon] = Entry(root, width=10)
        blend_right_top_entry[mon].insert(0,params.blend_right_top[mon])
        blend_right_top_entry[mon].grid(row=row0+9, column=1+mon, padx=5, pady=5)
        blend_right_bot_entry[mon] = Entry(root, width=10)
        blend_right_bot_entry[mon].insert(0,params.blend_right_bot[mon])
        blend_right_bot_entry[mon].grid(row=row0+10, column=1+mon, padx=5, pady=5)

    blend_test = BooleanVar(value=params.blendtest)
    blend_checkbox = Checkbutton(root, text="Test", variable=blend_test)
    blend_checkbox.grid(row=row0+6, column=2, padx=5, pady=5)
