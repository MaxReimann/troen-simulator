import bpy
import os
from bpy.props import BoolProperty, EnumProperty, StringProperty
from bpy_extras.io_utils import ExportHelper


### HOWTO: open this file in blender text editor. Edit your scene and make  sure only cubes are used (only translate and scale are supported for now)
## all cubes have to be prefixed by "Cube"

# This script will create newLevel.ive and newLevel.level (for osg and bullet) in data/levels/
# You can change the name of the level here:
levelName = "newLevel"

# make sure you got the TROEN and OSG_DIR environment variables
# make sure that you got osgconv.exe in OSG_DIR/bin (see dropbox)
osgPath = os.environ['OSG_DIR']
troenPath = os.environ['TROEN']
exportDir = troenPath + "data" + os.sep + "routes" + os.sep


#scale blender units by (this has to be the same as in the level exporter)
SCALE = 10.0

class AddPath(bpy.types.Operator):
    """Add a named Curve"""
    bl_idname = "troen.curve_add_named"
    bl_label = "Add Named Curve"
    bl_options = {'UNDO'}
    
    def invoke(self, context, event) :
        bpy.ops.curve.primitive_nurbs_path_add()
        for object in bpy.context.selected_objects:
            if object.name.startswith("Nurbs"):
                object.name = "NavigationRoute"

        return {"FINISHED"}

class ExportPath(bpy.types.Operator, ExportHelper):
    """Export Path to Troen (data\\routes)"""
    bl_idname = "troen.curve_export_troen"
    bl_label = "Export route"
    bl_options = {'REGISTER', 'UNDO'}

    # ExportHelper mixin class uses this
    filename_ext = ".txt"

    filter_glob = StringProperty(
            default="*.txt",
            options={'HIDDEN'},
            )

    filepath = StringProperty(
            name="File Path",
            description="Filepath used for exporting the file",
            maxlen=1024,
            subtype='FILE_PATH',
            default=exportDir,
            )

    # List of operator properties, the attributes will be assigned
    # to the class instance from the operator settings before calling.
    difficulty = EnumProperty(
            name="Route difficulty",
            description="Choose the difficulty of the route",
            items=(('OPT_A', "Easy", "Easy route"),
                   ('OPT_B', "Hard", "Not so easy route")),
            default='OPT_A',
            )

    def execute(self, context) :
        output = ""

        for obj in bpy.context.selected_objects:
            if obj.name.startswith("NavigationRoute"):
                global_mat = obj.matrix_world
                for point in obj.data.splines[0].bezier_points.data.points:
                    co = global_mat * point.co.xyz * SCALE
                    output += str(co[0]) + "\t" + str(co[1]) + "\t" + str(co[2]) + "\n"

                break

        with open(self.filepath, "w") as my_file:
            if self.difficulty == "OPT_A":
                my_file.write("difficulty: easy\n")
            else:
                my_file.write("difficulty: hard\n")

            my_file.write(output)


        return {"FINISHED"}


class CustomPanel(bpy.types.Panel):
    """Create paths and export them to Troen"""
    bl_label = "Troen Routes"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'TOOLS'



    def draw(self, context):
        layout = self.layout

        row = layout.row()
        row.label(text="Add and export routes:")

        split = layout.split()
        col = split.column(align=True)

        col.operator("troen.curve_add_named", text="Add Path", icon='CURVE_PATH')
        

        col.operator("troen.curve_export_troen", text="Export to Troen", icon='EXPORT')


def register():
    bpy.utils.register_class(AddPath)
    bpy.utils.register_class(ExportPath)
    bpy.utils.register_class(CustomPanel)

def unregister():
    bpy.utils.unregister_class(AddPath)
    bpy.utils.unregister_class(ExportPath)
    bpy.utils.unregister_class(CustomPanel)

if __name__ == "__main__":
    register()