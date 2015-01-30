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
    bpy.ops.object.mode_set(mode = 'EDIT') 
    obj = bpy.context.active_object
    #bm = bmesh.from_edit_mesh(obj.data)

    selected, unselected = select_vertical_faces(obj.data)
    unselected = sorted(unselected, key=lambda face: face.material_index)
    #popped = []
    slot_empty_mat = len(obj.data.materials)
    empty = bpy.data.materials.new('empty')
    obj.data.materials.append(empty)



    for face in unselected[::-1]:
        face.material_index = slot_empty_mat
        # if index not in popped:
        #     print("pop index" + str(index))
        #     obj.data.materials.pop(index, update_data=False)
        #     popped.append(index)

    obj.data.update()
    bpy.ops.object.mode_set(mode = 'OBJECT')

def copy_needed_textures():
    obj = bpy.context.active_object
    selected, unselected = select_vertical_faces(obj.data)
    items = obj.material_slots.items()
    abspath = "D:\\Blender\\troensimulator\\Berlin3ds\\Berlin3ds\\3860_5819\\"
    for face in selected:
        tex_name = items[face.material_index][0]
        shutil.copyfile(abspath + tex_name+".jpg", abspath + "needed/"+tex_name+".jpg")


unlink_unused_textures()
copy_needed_textures()
