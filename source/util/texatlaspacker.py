import bpy
import json
import bmesh
from os import listdir
from os.path import isfile, join

def get_faces_for_material(ob, bm, mat_name):
    for slot, _mat in enumerate(ob.data.materials):
        if _mat.name.startswith(mat_name):
            slot_index = slot
            break
    else:
        #print("material %s not found"%mat_name)
        return []

    faces_mat = []


    for face in bm.faces:
        if face.material_index == slot_index:
            # face has mat_c1 assigned
            faces_mat.append(face)
    return faces_mat


atlas_path = "D:\\Blender\\troensimulator\\Berlin3ds\\Berlin3ds\\packed\\"


def make_material(obj, name_trimmed, image):
    textlas_material = obj.data.materials.get(name_trimmed+"-mat")
    if textlas_material is None:
        slot_texatlas_mat = len(obj.data.materials)
        global_mat = bpy.data.materials.get(name_trimmed+"-mat")
        if global_mat == None:
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
            obj.data.materials.append(global_mat)
    else:
        for i, slot in enumerate(obj.material_slots):
            if trimmed(slot.material.name) == textlas_material.name:
                slot_texatlas_mat = i
                break
        else:
            print("material %s is not in obj: %s"%(textlas_material, obj))
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
            mat_name = frame["filename"].rsplit(".",1)[0] #remove .jpg
        else:
            frame = value_dict["frames"][_iter] #iter is a key
            mat_name = _iter.rsplit(".",1)[0]
            
        yield frame, mat_name

def trimmed(name):
    return name.split(".")[0]

def process_part(name, obj):
    with open(atlas_path+name) as fp:
        value_dict = json.load(fp)


    obj_mat_names = [trimmed(mat.name) for mat in obj.data.materials.values()]

    for frame, mat_name in gen_iterable(value_dict):
        if trimmed(mat_name) in obj_mat_names:
            break
    else:
        print("skipping:"+name)
        return #no matching textures where found, can continue

    print("processing:" + name)
    image_width = int(value_dict["meta"]["size"]["w"])
    image_height = int(value_dict["meta"]["size"]["h"])

    filename = value_dict["meta"]["image"].split("/")[-1]
    image = bpy.data.images.get(filename)
    if image is None:
        image = bpy.data.images.load(atlas_path+filename)  

    slot_texatlas_mat = None


    bm = bmesh.new()
    bm.from_mesh(obj.data)

    standard_uv_tex = bm.loops.layers.uv["UVMap"]
    atlas_uv_tex = bm.loops.layers.uv["texatlas"]#obj.data.uv_textures["texatlas"]
    layer = bm.faces.layers.tex.get("texatlas")
    
    for frame, mat_name in gen_iterable(value_dict):

        faces = get_faces_for_material(obj, bm, mat_name)
        if len(faces) == 0:
            continue
        #print(mat_name + " " + str(len(faces)))

        
        frame_x = frame["frame"]["x"]
        frame_top = frame["frame"]["y"]
        tile_w = frame["sourceSize"]["w"]
        tile_h = frame["sourceSize"]["h"]

        for face in faces:
            for loop in face.loops:
                uv = loop[standard_uv_tex].uv
                if not frame["rotated"]:
                    x_co = frame_x + tile_w*uv.x
                    y_co = image_height - frame_top - tile_h + tile_h*uv.y
                else :
                    #rotate clockwise 90 degrees =(y,-x)
                    x_co = frame_x + tile_h*uv.y 
                    y_co = image_height - frame_top - tile_w*uv.x

                loop[atlas_uv_tex].uv.x = x_co / float(image_width)
                loop[atlas_uv_tex].uv.y = y_co / float(image_height)

            
            face[layer].image = image
            if slot_texatlas_mat == None:
                slot_texatlas_mat = make_material(obj, trimmed(filename), image)
            face.material_index = slot_texatlas_mat

        
        
    bm.to_mesh(obj.data)
    bm.free()
    obj.data.update()

def unlink_unused_textures(obj):
    pop_index = 0

    while pop_index < len(obj.data.materials): 
        first = obj.data.materials[pop_index].name
        if first.startswith("tex") and "packed" not in first:
            obj.data.materials.pop(pop_index, update_data=True)
        else:
            pop_index += 1
  
    obj.data.update()



for obj in bpy.context.selected_objects:
    jsons = [ f for f in listdir(atlas_path) if isfile(join(atlas_path,f)) and f.split(".")[1] == "json" ]
    filtered = [j for j in jsons if "3860_5818" in j]
    for name in filtered:
        process_part(name, obj)
        
    unlink_unused_textures(obj)