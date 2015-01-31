import bpy

def move_by_amount(x, y, z):
    from mathutils import Matrix
    matrix = Matrix.Translation((x, y, z))
    mesh_unique = set(obj.data for obj in bpy.context.selected_objects)
    for mesh in mesh_unique:
        mesh.transform(matrix)
        mesh.update()


move_by_amount(-385.48679, -5817.32959, 0)