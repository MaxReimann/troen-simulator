import bpy


def copy_modifier(modifier_name):
    active_object = bpy.context.object
    selected_objects = [o for o in bpy.context.selected_objects
                        if o != active_object and o.type == active_object.type]

    for obj in selected_objects:
        for mSrc in active_object.modifiers:
            if mSrc.name != modifier_name:
                continue
            mDst = obj.modifiers.get(mSrc.name, None)
            if not mDst:
                mDst = obj.modifiers.new(mSrc.name, mSrc.type)

            # collect names of writable properties
            properties = [p.identifier for p in mSrc.bl_rna.properties
                          if not p.is_readonly]

            # copy those properties
            for prop in properties:
                setattr(mDst, prop, getattr(mSrc, prop))

copy_modifier("Subsurf")