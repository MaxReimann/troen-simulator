import bpy
import os
import math
from bpy.props import BoolProperty, EnumProperty, StringProperty
from bpy_extras.io_utils import ExportHelper
from mathutils import Vector


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

class AddBoundaries(bpy.types.Operator):
    """Add a street boundary to route"""
    bl_idname = "troen.curve_add_boundary"
    bl_label = "Add boundary to selected"
    bl_options = {'UNDO'}
    
    def invoke(self, context, event) :
        for object in bpy.context.selected_objects:
            if object.name.startswith("NavigationRoute"):
                self.add_boundary(object)

        return {"FINISHED"}

    def add_boundary(self, curve):
        spline = curve.data.splines[0]
        points = [x.co for x in spline.points]
        points = self.subdivide_adaptive(points, 5)
        length = 0.0
        for i in range(1,len(points)):
            length += (points[i-1] +  points[i]).length 
            print((points[i-1] +  points[i]).length)
        length /= len(points)
        length /= 25.0
        vectors = []
        

        for i in range(1,len(points)):
            vec = (curve.matrix_world * (points[i] - points[i-1])).xyz
            norm = vec.cross(Vector((0,0,1)))
            norm.normalize()
            norm*= 0.30

            direction = vec
            direction.normalize()
            rotZ = math.atan(direction.y / direction.x)
            if rotZ < 0:
                rotZ += 3.141592
            mid_point = (curve.matrix_world *(points[i-1]+points[i])/2.0).xyz
            #right cube
            start = mid_point+norm
            bpy.ops.mesh.primitive_cube_add(location=(start.x,start.y,start.z))
            cube = bpy.context.active_object
            cube.name = curve.name + "." + "route_boundary.r"
            cube.scale = (length,0.05,0.2)
            cube.rotation_euler = (0,0,rotZ)
            cube.layers = curve.layers

            #left cube
            start = mid_point-norm
            bpy.ops.mesh.primitive_cube_add(location=(start.x,start.y,start.z))
            cube = bpy.context.active_object
            cube.name = curve.name + "." + "route_boundary.l"
            cube.scale = (length,0.05,0.2)
            cube.rotation_euler = (0,0,rotZ)
            cube.layers = curve.layers


    def subdivide_adaptive(self, _input, level):
        def length(vec1, vec2):
            return math.sqrt((vec1.x-vec2.x)**2 + (vec1.y-vec2.y)**2 + (vec1.z-vec2.z)**2)
        points = _input
        thresh = 0.3
        for j in range(level):
            subdivided = []
            print("level %i\n\n"%j)
            for i in range(1,len(points)-1):
                if j < 3:
                    print(length(points[i - 1], points[i]))
                    print(points[i-1])
                    print(points[i])
                if length(points[i - 1], points[i]) > thresh :
                    p1 = (points[i - 1] + points[i]) / 2.0
                    p2 = (points[i] + points[i + 1]) / 2.0
                    p_i_new = (points[i] + (p1 + p2) / 2.0) / 2.0
                    subdivided.append(p1)
                    subdivided.append(p_i_new)
                else :
                    subdivided.append(points[i])
            points = []
            points.append(_input[0])
            points += subdivided
            points.append(_input[-1])
        return points

class ExportPath(bpy.types.Operator, ExportHelper):
    """Export Path to Troen (data\\routes)"""
    bl_idname = "troen.curve_export_troen"
    bl_label = "Export route"
    bl_options = {'REGISTER', 'UNDO'}

    # ExportHelper mixin class uses this
    filename_ext = ".route"

    filter_glob = StringProperty(
            default="*.route",
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

    export_bounds = BoolProperty(
            name="Export Boundaries",
            description="Boundaries will be exported in an extra file",
            default=True,
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

        if self.export_bounds:
            self.export_boundaries()


        return {"FINISHED"}

    def export_boundaries(self):
        routes = {}
        for ob in bpy.context.selectable_objects:
            if ob.name.startswith("NavigationRoute") and "boundary" in ob.name:
                name_split = ob.name.split(".")
                curve_name = name_split[0]
                if name_split[1] != "boundary":
                    curve_name = name_split[0] + "." + name_split[1]
                if curve_name not in routes:
                    routes[curve_name] = []

                routes[curve_name].append(ob)

        for bounds in routes.values():
            path = self.filepath.rsplit(".", 1)[0] + ".bounds" 
            with open(path, "w") as output_file:
                output_file.write(self.get_model_autogen(bounds))

    def get_model_autogen(self, obstacles):
        #write out the cubes location and dimensions
        auto_gen_code  = ""
        for ob_index, obstacle in enumerate(obstacles):
            auto_gen_code += self.create_box_collision_shape_str().format(
                                                          pos_x=str(obstacle.location.x*SCALE),
                                                          pos_y=str(obstacle.location.y*SCALE),
                                                          pos_z=str(obstacle.location.z*SCALE),
                                                          length_x=str(obstacle.dimensions.x*SCALE),
                                                          length_y=str(obstacle.dimensions.y*SCALE),
                                                          length_z=str(obstacle.dimensions.z*SCALE),
                                                          quat_x=str(obstacle.rotation_quaternion.x),
                                                          quat_y=str(obstacle.rotation_quaternion.y),
                                                          quat_z=str(obstacle.rotation_quaternion.z),
                                                          quat_w=str(obstacle.rotation_quaternion.w),
                                                          name=str(obstacle.name),
                                                          collisionType="LEVELWALLTYPE")
            if ob_index < len(obstacles) -1:
                auto_gen_code += "\n"
        return auto_gen_code

    def create_box_collision_shape_str(self):
        return """{pos_x}
{pos_y}
{pos_z}
{length_x}
{length_y}
{length_z}
{quat_x}
{quat_y}
{quat_z}
{quat_w}
{name}
{collisionType}"""


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
        col.operator("troen.curve_add_boundary", text="Add Boundary", icon='CURVE_PATH')
        

        col.operator("troen.curve_export_troen", text="Export to Troen", icon='EXPORT')


def register():
    bpy.utils.register_class(AddPath)
    bpy.utils.register_class(AddBoundaries)
    bpy.utils.register_class(ExportPath)
    bpy.utils.register_class(CustomPanel)

def unregister():
    bpy.utils.unregister_class(AddPath)
    bpy.utils.unregister_class(AddBoundaries)
    bpy.utils.unregister_class(ExportPath)
    bpy.utils.unregister_class(CustomPanel)

if __name__ == "__main__":
    register()