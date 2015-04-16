import bpy

#set boxes to floor
active = bpy.context.selected_objects
for object in active:
    minZ = 1000
    minLocation = (0,0,0)
    for vert in object.data.vertices:
        globalLoc = object.matrix_world * vert.co
        if globalLoc.z < minZ:
            minZ = globalLoc.z
            minLocation = globalLoc

    # store the location of current 3d cursor
    saved_location = bpy.context.scene.cursor_location.copy()  # returns a copy of the vector

    # give 3dcursor new coordinates
    bpy.context.scene.cursor_location = minLocation
    print(minLocation)

    # set the origin on the current object to the 3dcursor location
    bpy.ops.object.origin_set(type='ORIGIN_CURSOR')

    # set 3dcursor location back to the stored location
    bpy.context.scene.cursor_location = saved_location
    
    object.location.z = 0.0

