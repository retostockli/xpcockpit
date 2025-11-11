import params
from read_ini import *
from utility import *
from calc_warpgrid import *
from warpblend_window import init_warpblend_window, create_warpblend_window
from root_window import root, create_root_window
from save_matrix import save_xpfile, save_nvfile
from calc_blendgrid import calc_blendimage_unwarped

# Main Routine to calculate a Warp and Blend grid for a given set of projectors with known projector and projection geometry
# Specify everything in the ini file (see examples listed below)
# run the code and make final adjustments, then save a warp and blend file for running 
# with the NVIDIA API (see ../src/warpblend.c) or write a X-Plane projection geometry file (X-Plane Window Positions.prf)

# This code assumes that your root window for adjusting the projector geometry is on the first monitor which is not
# going to be warped and blended. If this is not the case, you will need to modify the root window creation in root_window.py
# for testing purposes the warp and blend window is shifted by half of the horizontal monitor size if only one monitor is used for now
# in order to let the root window still be displayed.

# All calculations are done with 0/0 corresponding to top left
# NVIDIA grids are stored with 0/0 corresponding to bottom left
# X-Plane grids are stored with 0/0 corresponding to bottom left

#params.inifile="../inidata/singlemon.ini"
params.inifile="../inidata/fourmon.ini"

params.xpfile="../data/X-Plane Window Positions.prf"
params.nvfile="../data/nv_warpblend_grid"

read_ini()

init_warpblend_window()
create_root_window()

for mon in range(0,params.nmon,1):

    create_warpblend_window(mon)


root.mainloop()
