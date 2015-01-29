import bpy, mathutils
# Select by direction
PI = 3.14159218
def by_direction():
        bpy.ops.object.mode_set(mode='OBJECT')
        mesh = bpy.context.active_object.data
        upVec = mathutils.Vector((0,0,1))
        # Loop through all the given faces
        for f in mesh.polygons:  
            if f.normal.length > 0:
                isSelected = f.select
                
                angle = upVec.angle(f.normal)
                
                if angle >= PI/2 - 0.1 and angle <= PI/2 + 0.1:
                	print("select")
                	f.select = True
                else:
                	f.select = False
        bpy.ops.object.mode_set(mode='EDIT')


by_direction()