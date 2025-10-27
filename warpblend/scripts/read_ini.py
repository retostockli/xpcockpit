import math
import configparser
import ast
import params
from utility import r2d, d2r

def read_ini(inifile):

    config = configparser.ConfigParser(
        comment_prefixes=('#'),
        inline_comment_prefixes=('#',),)
    config.sections()

    config.read(inifile)

    params.xp12 = config.getboolean('GENERAL','xp12')
    params.outfile = config.get('GENERAL','outfile')
    params.nmon = config.getint('GENERAL','nmon')
    params.ngx = config.getint('GENERAL','ngx')
    params.ngy = config.getint('GENERAL','ngy')
    params.dragx = config.getint('GENERAL','dragx')
    params.dragy = config.getint('GENERAL','dragy')

    params.R = config.getfloat('PROJECTION','R')
    params.d_0 = config.getfloat('PROJECTION','d_0')
    params.h_0 = config.getfloat('PROJECTION','h_0')
    params.tr = config.getfloat('PROJECTION','tr')
    params.nx = ast.literal_eval(config.get('PROJECTION','nx'))
    params.ny = ast.literal_eval(config.get('PROJECTION','ny'))
    params.ceiling = config.getboolean('PROJECTION','ceiling')
    params.cylindrical = ast.literal_eval(config.get('PROJECTION','cylindrical'))
    params.projection = ast.literal_eval(config.get('PROJECTION','projection'))
    params.epsilon = ast.literal_eval(config.get('PROJECTION','epsilon'))
    params.frustum = config.getfloat('PROJECTION','frustum')
    params.lateral_offset = ast.literal_eval(config.get('PROJECTION','lateral_offset'))
    params.vertical_offset = ast.literal_eval(config.get('PROJECTION','vertical_offset'))
    params.vertical_shift = ast.literal_eval(config.get('PROJECTION','vertical_shift'))
    params.vertical_scale = ast.literal_eval(config.get('PROJECTION','vertical_scale'))

    params.blending = ast.literal_eval(config.get('BLENDING','blending'))
    params.blend_left_top = ast.literal_eval(config.get('BLENDING','blend_left_top'))
    params.blend_left_bot = ast.literal_eval(config.get('BLENDING','blend_left_bot'))
    params.blend_right_top = ast.literal_eval(config.get('BLENDING','blend_right_top'))
    params.blend_right_bot = ast.literal_eval(config.get('BLENDING','blend_right_bot'))

    params.gridtest = config.getboolean('FLAGS','gridtest')
    params.blendtest = config.getboolean('FLAGS','blendtest')
    params.forwin = config.getboolean('FLAGS','forwin')
    params.savegrid = config.getboolean('FLAGS','savegrid')

    # fix for xp11
    if not params.xp12:
        params.ngx = 101
        params.ngy = 101

    params.stepx = int((params.ngx-1)/(params.dragx-1)) # make sure this is integer and stepx * (dragx-1) = ngx-1!
    params.stepy = int((params.ngy-1)/(params.dragy-1)) # make sure this is integer and stepy * (dragy-1) = ngy-1!

    print("*** GENERAL ***")
    print("XP12:             "+str(params.xp12))
    print("Warp+Blend File:  "+params.outfile)
    print("# Monitors:       "+str(params.nmon))
    print("ngx:              "+str(params.ngx))
    print("ngy:              "+str(params.ngy))
    print("dragx:            "+str(params.dragx))
    print("dragy:            "+str(params.dragy))
    print("stepx (calc):     "+str(params.stepx))
    print("stepy (calc):     "+str(params.stepy))
    print()
    print("*** PROJECTION ***")
    print("Screen Radius:    "+str(params.R))
    print("Dist from center: "+str(params.d_0))
    print("Hgt from bottom:  "+str(params.h_0))
    print("Throw Ratio:      "+str(params.tr))
    print("NX:               "+str(params.nx))
    print("NY:               "+str(params.ny))
    print("Ceiling:          "+str(params.ceiling))
    print("Cylindrical:      "+str(params.cylindrical))
    print("Projection:       "+str(params.projection))
    print("Epsilon:          "+str(params.epsilon))
    print("Frustum:          "+str(params.frustum))
    print("Lateral Offset:   "+str(params.lateral_offset))
    print("Vertical Offset:  "+str(params.vertical_offset))
    print("Vertical Shift:   "+str(params.vertical_shift))
    print("Vertical Scale:   "+str(params.vertical_scale))
    print()
    print("*** BLENDING ***")
    print("Blending:         "+str(params.blending))
    print("Left Top:         "+str(params.blend_left_top))
    print("Left Bottom:      "+str(params.blend_left_bot))
    print("Right Top:        "+str(params.blend_right_top))
    print("Right Bottom:     "+str(params.blend_right_bot))
    print()
    print("*** FLAGS ***")
    print("Gridtest:         "+str(params.gridtest))
    print("Blendtest:        "+str(params.blendtest))
    print("Save for WIN:     "+str(params.forwin))
    print("Save GRID:        "+str(params.savegrid))

    # General Calculations independent of pixel position (cm or degrees)
    params.beta = math.atan(1.0/(2.0*params.tr))*r2d # Maximum horizontal FOV from Projector [deg]
    params.beta1 = 180.-params.beta   # well ... check out the drawing yourself
    params.delta = math.asin(params.d_0/params.R*math.sin(params.beta1*d2r))*r2d  # same here
    params.gamma = 180.-params.beta1-params.delta  # Maximum horizontal FOV from Screen Center [deg]
    params.R_1 = params.R*math.sin(params.gamma*d2r)/math.sin(params.beta1*d2r) # Maximum Distance of Projector to screen edge
    params.d_1 = params.R_1*math.cos(params.beta*d2r) # distance of projector to hypothetical planar screen in front of cylindrical screen
    params.w_h = params.R_1*math.sin(params.beta*d2r) # half of hypothetical planar image width at screen distance


