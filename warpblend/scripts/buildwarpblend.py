import params
from read_ini import *
from utility import *
from calc_warpgrid import *
from warpblend_window import init_warpblend_window, create_warpblend_window
from root_window import root, create_root_window


inifile="singlemon.ini"
#inifile="test.ini"

read_ini(inifile)

init_warpblend_window()
create_root_window()

for mon in range(0,params.nmon,1):

    create_warpblend_window(mon)


root.mainloop()
