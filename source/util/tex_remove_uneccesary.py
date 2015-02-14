import bpy

obj = bpy.context.active_object


bm = bmesh.new()
bm.from_mesh(obj.data)


for slot, mat in enumerate(ob.data.materials):
	if mat.active_texture == None:
		continue

	texpath = mat.active_texture.image.filepath

	for slot_comp, mat_comp in enumerate(obj.data.materials):
		if mat_comp.active_texture == None:
			continue
		if texpath == mat_comp.active_texture.image.filepath:
			for face in bm.faces:
				for loop in face.loops:
					if face.material_index == slot_comp:
						face.material_index = slot

    bm.to_mesh(obj.data)
    bm.free()
    obj.data.update()



