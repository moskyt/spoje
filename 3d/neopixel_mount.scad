include <bosl2/std.scad>
$fn = 80;

eps = 0.01;

spacing = 24;

block_x = 20;
block_y = 40;
slit_x = 8;
slit_y = 0.8;
support_z = 5;

inner_z = 3;
outer_z = 3;

spacer_d = 10;

left(40)
difference() {
    cuboid([block_x, block_y, outer_z], anchor = BOTTOM);
    
    down(eps)
    ycopies(n = 2, l = spacing) 
    zcyl(l=100, d=spacer_d);
    
    zcyl(l=100, d=3.5);
    up(outer_z/2)
    zcyl(l=100, d=6, anchor = BOTTOM);
}

difference() {
    union() {
        cuboid([block_x, block_y, inner_z], anchor = BOTTOM);
        
        ycopies(n = 2, l = spacing) 
        up(inner_z)
        cuboid([slit_x/2, slit_x/2, support_z], anchor = BOTTOM);
    }
    
    ycopies(n = 2, l = spacing) {
        up(inner_z)
        cuboid([slit_x, slit_y*1.5, support_z+2*eps], anchor = BOTTOM);
        
        down(eps)
        xcopies(n = 4, l = slit_x) 
        cuboid([slit_y*2, slit_y*2, inner_z+2*eps], anchor = BOTTOM);
    }
    
    zcyl(l=100, d=3.5);
}