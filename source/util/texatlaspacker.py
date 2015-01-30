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


atlas_path = "D:\\Blender\\troensimulator\\Berlin3ds\\Berlin3ds\\3860_5819\\texatlas\\"

def process_part(name):
    with open(atlas_path+name) as fp:
        value_dict = json.load(fp)

    obj = bpy.context.active_object

    image_width = int(value_dict["meta"]["size"]["w"])
    image_height = int(value_dict["meta"]["size"]["h"])

    filename = value_dict["meta"]["image"].split("/")[-1]
    image = bpy.data.images.get(filename)
    if image is None:
        image = bpy.data.images.load(atlas_path+filename)  

    for frame in value_dict["frames"]:
        mat_name = frame["filename"].split(".")[0] #remove .jpg

        bm = bmesh.new()
        bm.from_mesh(obj.data)

        standard_uv_tex = bm.loops.layers.uv["UVMap"]
        atlas_uv_tex = bm.loops.layers.uv["texatlas"]#obj.data.uv_textures["texatlas"]

        faces = get_faces_for_material(obj, bm, mat_name)

        frame_x = frame["frame"]["x"]
        frame_top = frame["frame"]["y"]
        tile_w = frame["sourceSize"]["w"]
        tile_h = frame["sourceSize"]["h"]

        print(mat_name)
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

        
        
        bm.to_mesh(obj.data)
        bm.free()





jsons = [ f for f in listdir(atlas_path) if isfile(join(atlas_path,f)) and f.split(".")[1] == "json" ]
for name in jsons:
    process_part(name)