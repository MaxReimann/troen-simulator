import bpy, mathutils
import shutil
import bmesh
# Select by direction
PI = 3.14159218
def select_vertical_faces(mesh):
    upVec = mathutils.Vector((0,0,1))

    unselected = []
    selected = []
    # Loop through all the given faces
    for f in mesh.polygons:  
        if f.normal.length > 0:
            isSelected = f.select
            
            angle = upVec.angle(f.normal)
            
            if angle >= PI/2 - 0.1 and angle <= PI/2 + 0.1:
                #f.select = True
                selected.append(f)
            else:
                #f.select = False
                unselected.append(f)

    return selected,unselected
        
def unlink_unused_textures():
    obj = bpy.context.active_object
    first = obj.data.materials[0].name
    if first == "default":
        pop_index = 1
        first = obj.data.materials[1].name
    else:
        pop_index = 0
    
    while first.startswith("tex"):
        obj.data.materials.pop(pop_index, update_data=True)
        if pop_index > len(obj.data.materials) -1 :
            break
        first = obj.data.materials[pop_index].name
    obj.data.update()

def copy_needed_textures():
    obj = bpy.context.active_object
    selected, unselected = select_vertical_faces(obj.data)
    items = obj.material_slots.items()
    abspath = "D:\\Blender\\troensimulator\\Berlin3ds\\Berlin3ds\\3860_5819\\"
    for face in selected:
        tex_name = items[face.material_index][0]
        shutil.copyfile(abspath + tex_name+".jpg", abspath + "needed/"+tex_name+".jpg")


#unlink_unused_textures()
#copy_needed_textures()
