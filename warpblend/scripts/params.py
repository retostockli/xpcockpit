#inifile name
inifile = ""

# Parameters stored in ini File
xp12 = True
outfile = ""
nmon = 0
ngx = 0
ngy = 0
dragx = 0
dragy = 0
R = 0.0
d_0 = 0.0
h_0 = 0.0
tr = 0.0
nx = []
ny = []
ceiling = False
cylindrical = False
projection = False
epsilon = 0.0
frustum = 0.0
lateral_offset = []
vertical_offset = []
vertical_shift = []
vertical_scale = []
blending = []
blend_left_top = []
blend_left_bot = []
blend_right_top = []
blend_right_bot = []
gridtest = False
blendtest = False
forwin = False
savegrid = False

# calculated parameters
beta = 0.0
beta1 = 0.0
delta = 0.0
gamma = 0.0
R_1 = 0.0
d_1 = 0.0
w_h = 0.0
stepx = 0
stepy = 0

# blend alpha values (4 steps at distance 0.0, 0.33, 0.67 and 1.0)
blend_alpha = [0.0,0.45,0.8,1.0]
blend_exp = 1.0 # exponent of the blend function (distance^exponent)

# window parameters
ROOT_NX = 800
ROOT_NY = 700
