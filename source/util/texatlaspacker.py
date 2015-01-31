import bpy
import json
import bmesh
from os import listdir
from os.path import isfile, join

def get_faces_for_material(ob, bm, mat_name):

    mat = ob.data.materials.get(mat_name)
    mat_slots = [id for id, _mat in enumerate(ob.data.materials) if _mat == mat]

    faces_mat = []


    for face in bm.faces:
        if face.material_index in mat_slots:
            # face has mat_c1 assigned
            faces_mat.append(face)
    return faces_mat


atlas_path = "D:\\Blender\\troensimulator\\Berlin3ds\\Berlin3ds\\packed\\"


def make_material(obj, name_trimmed, image):
    textlas_material = bpy.data.materials.get(name_trimmed+"-mat")
    if textlas_material is None:
        slot_texatlas_mat = len(obj.data.materials)
        textlas_material =  bpy.data.materials.new(name_trimmed+"-mat")
        obj.data.materials.append(textlas_material)

        cTex = bpy.data.textures.new(name_trimmed, type = 'IMAGE')
        cTex.image = image

        # Add texture slot for color texture
        mtex = textlas_material.texture_slots.add()
        mtex.texture = cTex
        mtex.texture_coords = 'UV'
        mtex.use_map_color_diffuse = True 
        mtex.mapping = 'FLAT'
        mtex.uv_layer = 'texatlas'
    else:
        for i, slot in enumerate(obj.material_slots):
            if slot.material == textlas_material:
                slot_texatlas_mat = i
    return slot_texatlas_mat

def gen_iterable(value_dict):
    if type(value_dict["frames"]) == list:
        iterable = value_dict["frames"]
        isdict = False
    elif type(value_dict["frames"]) == dict:
        iterable = value_dict["frames"].keys()
        isdict = True

    for _iter in iterable:
        if not isdict:
            frame = _iter 
            mat_name = frame["filename"].split(".")[0] #remove .jpg
        else:
            frame = value_dict["frames"][_iter] #iter is a key
            mat_name = _iter.split(".")[0]
            
        yield frame, mat_name


def process_part(name):
    with open(atlas_path+name) as fp:
        value_dict = json.load(fp)

    
    obj = bpy.context.active_object
    print(name)

    obj_mat_names = [mat.name.split(".")[0] for mat in obj.data.materials.values()]

    for frame, mat_name in gen_iterable(value_dict):
        if mat_name in obj_mat_names:
            break
    else:
        return #no matching textures where found, can continue

    image_width = int(value_dict["meta"]["size"]["w"])
    image_height = int(value_dict["meta"]["size"]["h"])

    filename = value_dict["meta"]["image"].split("/")[-1]
    image = bpy.data.images.get(filename)
    if image is None:
        image = bpy.data.images.load(atlas_path+filename)  

    name_trimmed = filename.split(".")[0]

    slot_texatlas_mat = make_material(obj, name_trimmed, image)


    for frame, mat_name in gen_iterable(value_dict):

        bm = bmesh.new()
        bm.from_mesh(obj.data)

        standard_uv_tex = bm.loops.layers.uv["UVMap"]
        atlas_uv_tex = bm.loops.layers.uv["texatlas"]#obj.data.uv_textures["texatlas"]

        faces = get_faces_for_material(obj, bm, mat_name)



        frame_x = frame["frame"]["x"]
        frame_top = frame["frame"]["y"]
        tile_w = frame["sourceSize"]["w"]
        tile_h = frame["sourceSize"]["h"]

        #print(mat_name)
        for face in faces:
            for loop in face.loops:
                uv = loop[standard_uv_tex].uv
                #uv_map_x, uv_map_y = standard_uv_tex.data[loop].uv
                if not frame["rotated"]:
                    x_co = frame_x + tile_w*uv.x
                    y_co = image_height - frame_top - tile_h + tile_h*uv.y
                else :
                    #rotate clockwise 90 degrees =(y,-x)
                    x_co = frame_x + tile_h*uv.y 
                    y_co = image_height - frame_top - tile_w*uv.x

                loop[atlas_uv_tex].uv.x = x_co / float(image_width)
                loop[atlas_uv_tex].uv.y = y_co / float(image_height)

            layer = bm.faces.layers.tex.get("texatlas")
            face[layer].image = image
            face.material_index = slot_texatlas_mat

        
        
        bm.to_mesh(obj.data)
        bm.free()
        obj.data.update()

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




jsons = [ f for f in listdir(atlas_path) if isfile(join(atlas_path,f)) and f.split(".")[1] == "json" ]
for name in jsons:
    process_part(name)
    
unlink_unused_textures()