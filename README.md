housing.sketch3d
================

For converting Floor plans to 3d SketchUp models.

----------------------------------

##Program Flow

Input Image -> Contours detection by removing thin lines -> Noise Detection on segments walls -> Rectification of Noise Points -> Walls are written to a SketchUp object.

<dt>noise Points </dt>
 <dd> The points which are not maintaing a constant slope are classified as noise Points. </dd>
 

##Detection of Noise Points

Displacement in the X and Y directions are calculated for each point in the contour separately and if the displacement maintains the same pattern they are classified as Good Points, otherwise they are detected as Noise.


##Rectification of Noise Points

  3 Phases of Correction is there- 
  
    - Corner Correction - corrects deformed junctions of walls (corners).
    
    - Parallel Correction - corrects deformed ends of walls (parallel ending walls). 
    
    - Merge Parallel Lines - corrects shifted line segments to a single line segment.
  
##Writing to SketchUp Model

  SketchUp SDK code uses coordinates from text file and creates a .skp object containing the walls.