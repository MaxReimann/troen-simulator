import bpy

active = bpy.context.selected_objects
for object in active:
    curve = object.data
    spline = curve.splines.active
    for vert in spline.points:
        vert.co.z = 0.0