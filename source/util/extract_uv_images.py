import bpy
import bmesh
import shutil
import os
import json
from os import listdir
from os.path import isfile, join

# image = bpy.data.images['mergeatlas']
# width = image.size[0]
# height = image.size[1]

# pixels = image.pixels[:] # create a copy
# # Use the tuple objects, which is way faster than direct access to Image.pixels

# # Write back to image.
# # Slice notation here means to replace in-place, not sure if it's faster...
# image.pixels[:] = pixels
# # Should probably update image
# image.update()


def get_textures(objs):
    tex_set = set()

    for obj in objs:

        bm = bmesh.new()
        bm.from_mesh(obj.data)

        uvmap_layer =  bm.faces.layers.tex.get("UVMap")
        for face in bm.faces:
            im = face[uvmap_layer].image
            if im != None:
                abs_path = bpy.path.abspath(im.filepath, library=im.library)
                tex_set.add(abs_path)


    return tex_set

def make_atlas():

    objs = bpy.context.selected_objects

    textures = get_textures(objs)
    for tex in textures:
        dstpath = "D:/Blender/troensimulator/Berlin3ds/Berlin3ds/seperatepack"
        dst_name = "_".join(tex.split("\\")[-2:])
        shutil.copyfile(tex, os.path.join(dstpath, dst_name))

    str_tex = " ".join(textures)

    packer_config = """D:/Programme/CodeAndWeb/TexturePacker/bin/TexturePacker.exe --format json --size-constraints POT --data D:/Blender/troensimulator/Berlin3ds/Berlin3ds/seperatepack/{n}.json --multipack --pack-mode Good --max-size 4096 --texture-format tga  --verbose --sheet D:/Blender/troensimulator/Berlin3ds/Berlin3ds/seperatepack/{n}.tga  D:/Blender/troensimulator/Berlin3ds/Berlin3ds/seperatepack/"""

    print(packer_config)

    with open("D:\\Blender\\troensimulator\\Berlin3ds\\atlasgen.bat","w") as fd:
        fd.write(packer_config)


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



def get_or_make_material(obj, name_trimmed, image):
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
        mtex.uv_layer = 'mergeatlas'

    else:

        for i, slot in enumerate(obj.material_slots):
            if slot.material == None:
                obj.data.materials.append(textlas_material)
                slot_texatlas_mat = i
                break
            if trimmed(slot.material.name) == textlas_material.name:
                slot_texatlas_mat = i
                break
        else:
            slot_texatlas_mat = 0
            obj.data.materials.append(textlas_material)

    return slot_texatlas_mat

def trimmed(name):
    return name.split(".")[0]

def clear_materials(obj):
    #obj.data.materials.clear() #works since 2.69
    while obj.data.materials:
        obj.data.materials.pop(0,update_data=True)


def process_part(name, obj):
    atlas_path = "D:/Blender/troensimulator/Berlin3ds/Berlin3ds/seperatepack/"
    with open(atlas_path+name) as fp:
        value_dict = json.load(fp)


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
    standard_layer = bm.faces.layers.tex.get("UVMap")
    atlas_uv_tex = bm.loops.layers.uv["mergeatlas"]#obj.data.uv_textures["texatlas"]
    layer = bm.faces.layers.tex.get("mergeatlas")

    for face in bm.faces:
        if face[standard_layer].image == None:
            continue

        im = face[standard_layer].image
        abs_path = bpy.path.abspath(im.filepath, library=im.library)
        
        im_name = ("_".join(abs_path.split("\\")[-2:]))
        im_name = im_name.replace(".tga", ".jpg")

        frames =  value_dict["frames"]

        if im_name not in frames:
            continue
        print(abs_path)

        frame = frames[im_name]
        
        frame_x = frame["frame"]["x"]
        frame_top = frame["frame"]["y"]
        tile_w = frame["sourceSize"]["w"]
        tile_h = frame["sourceSize"]["h"]

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

        face.material_index = get_or_make_material(obj, trimmed(filename), image)


        
        
    bm.to_mesh(obj.data)
    bm.free()
    obj.data.update()

if __name__ == "__main__":

    atlas_path = "D:/Blender/troensimulator/Berlin3ds/Berlin3ds/seperatepack/"
    objs = bpy.context.selected_objects
    jsons = [ f for f in listdir(atlas_path) if isfile(join(atlas_path,f)) and f.split(".")[1] == "json" ]
    for obj in objs:
        clear_materials(obj)
        if "mergeatlas" not in obj.data.uv_textures:
            obj.data.uv_textures.new("mergeatlas")
        for name in jsons:
            process_part(name, obj)