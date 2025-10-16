import configparser
import ast

def read_ini(inifile):

    config = configparser.ConfigParser(
        comment_prefixes=('#'),
        inline_comment_prefixes=('#',),)
    config.sections()

    config.read(inifile)

    xp12 = config.getboolean('GENERAL','xp12')
    outfile = config.get('GENERAL','outfile')
    nmon = config.getint('GENERAL','nmon')
    ngx = config.getint('GENERAL','ngx')
    ngy = config.getint('GENERAL','ngy')
    dragx = config.getint('GENERAL','dragx')
    dragy = config.getint('GENERAL','dragy')

    R = config.getfloat('PROJECTION','R')
    d_0 = config.getfloat('PROJECTION','d_0')
    h_0 = config.getfloat('PROJECTION','h_0')
    tr = config.getfloat('PROJECTION','tr')
    nx = ast.literal_eval(config.get('PROJECTION','nx'))
    ny = ast.literal_eval(config.get('PROJECTION','ny'))
    ceiling = config.getboolean('PROJECTION','ceiling')
    cylindrical = ast.literal_eval(config.get('PROJECTION','cylindrical'))
    projection = ast.literal_eval(config.get('PROJECTION','projection'))
    epsilon = ast.literal_eval(config.get('PROJECTION','epsilon'))
    frustum = config.getfloat('PROJECTION','frustum')
    lateral_offset = ast.literal_eval(config.get('PROJECTION','lateral_offset'))
    vertical_offset = ast.literal_eval(config.get('PROJECTION','vertical_offset'))
    vertical_shift = ast.literal_eval(config.get('PROJECTION','vertical_shift'))
    vertical_scale = ast.literal_eval(config.get('PROJECTION','vertical_scale'))

    blending = ast.literal_eval(config.get('BLENDING','blending'))
    blend_left_top = ast.literal_eval(config.get('BLENDING','blend_left_top'))
    blend_left_bot = ast.literal_eval(config.get('BLENDING','blend_left_bot'))
    blend_right_top = ast.literal_eval(config.get('BLENDING','blend_right_top'))
    blend_right_bot = ast.literal_eval(config.get('BLENDING','blend_right_bot'))

    gridtest = config.getboolean('FLAGS','gridtest')
    blendtest = config.getboolean('FLAGS','blendtest')
    forwin = config.getboolean('FLAGS','forwin')
    savegrid = config.getboolean('FLAGS','savegrid')

    print("*** GENERAL ***")
    print("XP12:             "+str(xp12))
    print("Warp+Blend File:  "+outfile)
    print("# Monitors:       "+str(nmon))
    print("ngx:              "+str(ngx))
    print("ngy:              "+str(ngy))
    print("dragx:            "+str(dragx))
    print("dragy:            "+str(dragy))
    print()
    print("*** PROJECTION ***")
    print("Screen Radius:    "+str(R))
    print("Dist from center: "+str(d_0))
    print("Hgt from bottom:  "+str(h_0))
    print("Throw Ratio:      "+str(tr))
    print("NX:               "+str(nx))
    print("NY:               "+str(ny))
    print("Ceiling:          "+str(ceiling))
    print("Cylindrical:      "+str(cylindrical))
    print("Projection:       "+str(projection))
    print("Epsilon:          "+str(epsilon))
    print("Frustum:          "+str(frustum))
    print("Lateral Offset:   "+str(lateral_offset))
    print("Vertical Offset:  "+str(vertical_offset))
    print("Vertical Shift:   "+str(vertical_shift))
    print("Vertical Scale:   "+str(vertical_scale))
    print()
    print("*** BLENDING ***")
    print("Blending:         "+str(blending))
    print("Left Top:         "+str(blend_left_top))
    print("Left Bottom:      "+str(blend_left_bot))
    print("Right Top:        "+str(blend_right_top))
    print("Right Bottom:     "+str(blend_right_bot))
    print()
    print("*** FLAGS ***")
    print("Gridtest:         "+str(gridtest))
    print("Blendtest:        "+str(blendtest))
    print("Save for WIN:     "+str(forwin))
    print("Save GRID:        "+str(savegrid))

    return xp12, outfile, nmon, ngx, ngy, dragx, dragy, R, d_0, h_0, tr, nx, ny, \
            ceiling, cylindrical, projection, epsilon, frustum, \
            lateral_offset, vertical_offset, vertical_shift, vertical_scale, \
            blending, blend_left_top, blend_left_bot, blend_right_top, blend_right_bot, \
            gridtest, blendtest, forwin, savegrid

