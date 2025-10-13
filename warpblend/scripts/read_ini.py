import configparser
import ast

config = configparser.ConfigParser(
    comment_prefixes=('#'),
    inline_comment_prefixes=('#',),)
config.sections()

config.read('test.ini')

xp12 = config.getboolean('GENERAL','xp12')
outfile = config.get('GENERAL','outfile')
nmon = config.getint('GENERAL','nmon')

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
print("For XP12:        "+str(xp12))
print("File:            "+outfile)
print("# Monitors:      "+str(nmon))
print()
print("*** PROJECTION ***")
print("Ceiling:         "+str(ceiling))
print("Cylindrical:     "+str(cylindrical))
print("Projection:      "+str(projection))
print("Epsilon:         "+str(epsilon))
print("Frustum:         "+str(frustum))
print("Lateral Offset:  "+str(lateral_offset))
print("Vertical Offset: "+str(vertical_offset))
print("Vertical Shift:  "+str(vertical_shift))
print("Vertical Scale:  "+str(vertical_scale))
print()
print("*** BLENDING ***")
print("Blending:        "+str(blending))
print("Left Top:        "+str(blend_left_top))
print("Left Bottom:     "+str(blend_left_bot))
print("Right Top:       "+str(blend_right_top))
print("Right Bottom:    "+str(blend_right_bot))
print()
print("*** FLAGS ***")
print("Gridtest:        "+str(gridtest))
print("Blendtest:       "+str(blendtest))
print("Save for WIN:    "+str(forwin))
print("Save GRID:       "+str(savegrid))

