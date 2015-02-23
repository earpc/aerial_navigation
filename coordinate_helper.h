
#include <stdio.h>
#include <cmath>

// These structures are based of the triangle similarity. Thus there is a relationship
// between the distance of the known object with apparent size and the same object at
// a know distance with measured size.

// we assume that the camera is pointed directly at the object, and it is level

class coordinate_helper { // helpful funtions for locating objects in 3d space
    
protected:
    // const for calibration
    static const float fov_width; // real world distance across the visible view at dist1
    static const float fov_height;
    static const float dist1;
    
    static const int resolution_width;
    static const int resolution_height;
    
public:
    class resolution_converter // converts pixels to real world distances
    {
        
    public:
        // thes functions use the triangle similarity
        static float get_distance(float world1, float dist1, float diameter_obj, float pixels_obj, float pixels_accross)
        {
            return dist1 * diameter_obj/world1 * pixels_accross/pixels_obj;
        }
        
        static float get_width(float world1, float dist1, float dist2, float pixels_obj, float pixels_across)
        {
            return world1 * dist2/dist1 * pixels_obj/pixels_across;
        }
    };
    static resolution_converter res_converter;
    
    float distance;
};

class sphere_helper : public coordinate_helper {
protected:
    
public:
    sphere_helper(float sphere_pixels_diameter_width, float sphere_diameter)
    {
        distance = res_converter.get_distance(fov_width,dist1,sphere_diameter,sphere_pixels_diameter_width,resolution_width);
    }
};

class wicket_helper : public coordinate_helper{
    
public:
    
    float angle_about_vertical_axis; // counter clockwise
    
    wicket_helper(cv::Vector_ wicket_pixels, cv::Vector_ wicket_size, float angle_rad_from_horizontal)
    {
        // width and height should be measured about the center of the wicket
        
        // use height of wicket to find distance
        
        // since we are looking down at the wicket use trig to find apparent height.
        
        float phi = 3.141519/2.0 - angle_rad_from_horizontal;
        float apparent_height = wicket_size.height * std::cos(phi);
        
        float apparent_distance = res_converter.get_distance(fov_height, dist1, apparent_height, wicket_pixels.y, resolution_height);
        
        float diff_dist = wicket_size.height/2.0 * std::sin(phi);
        distance = diff_dist + apparent_distance;
        
        // next calculate angle about the vertical axis
        // first find apparent width
        
        float apparent_width = res_converter.get_distance(fov_width, dist1, distance, wicket_pixels.x, resolution_width);
        
        // use trig to find angle since we know the true width
        
        float angle_plane_c = std::acos(apparent_width/wicket_size.x); // angle between the true cross bar
                                                                // and the plane normal to the camera
        
        float unit_c_x = std::sin(-angle_plane_c);
        float unit_c_y = std::cos(angle_plane_c);
        
        // project this vector onto ground plane
        // x => x * cos(angle from horzontal)
        
        float g_x = unit_c_x*std::cos(angle_rad_from_horizontal);
        float g_y = unit_c_y;
        
        angle_about_vertical_axis = std::atan(g_y/g_x);
    }
};
