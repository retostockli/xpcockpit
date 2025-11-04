import params
from read_ini import *
from utility import *
from calc_warpgrid import *
from warpblend_window import init_warpblend_window, create_warpblend_window
from root_window import root, create_root_window
from save_matrix import save_xpfile
from calc_blendgrid import calc_blendimage_unwarped


params.inifile="../inidata/singlemon.ini"
#params.inifile="../inidata/fourmon.ini"

params.xpfile="../data/X-Plane Window Positions.prf"
params.nvfile="../data/nv_warpblend_grid.dat"

read_ini()

# # General Calculations independent of pixel position (cm or degrees)
# params.beta = math.atan(1.0/(2.0*params.tr))*r2d # Maximum horizontal FOV from Projector [deg]
# params.beta1 = 180.-params.beta   # well ... check out the drawing yourself
# params.delta = math.asin(params.d_0/params.R*math.sin(params.beta1*d2r))*r2d  # same here
# params.gamma = 180.-params.beta1-params.delta  # Maximum horizontal FOV from Screen Center [deg]
# params.R_1 = params.R*math.sin(params.gamma*d2r)/math.sin(params.beta1*d2r) # Maximum Distance of Projector to screen edge
# params.d_1 = params.R_1*math.cos(params.beta*d2r) # distance of projector to hypothetical planar screen in front of cylindrical screen
# params.w_h = params.R_1*math.sin(params.beta*d2r) # half of hypothetical planar image width at screen distance

# save_xpfile()


init_warpblend_window()
create_root_window()

for mon in range(0,params.nmon,1):

    create_warpblend_window(mon)


root.mainloop()
